#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "modbus_rtu.h"
#include "temp_humi_sensor.h"

#include "nvs_flash.h"
#include "wifi_sta.h"
#include "my_mqtt.h"
#include "time_sync.h"

#define TAG "MAIN"


// ============================================================
// 状态判定阈值
// ============================================================

// 连续失败3次，判定设备离线
#define OFFLINE_FAIL_THRESHOLD       3

// 连续成功2次，判定设备上线
#define ONLINE_SUCCESS_THRESHOLD     2


// ============================================================
// 设备配置
// ============================================================

typedef struct
{
    uint8_t device_id;      // 网关内部设备编号
    uint8_t slave_addr;     // Modbus从站地址

} device_config_t;


/*
 * 当前只有一个真实温湿度传感器：
 *
 * Device ID = 1
 * Modbus地址 = 0x01
 *
 * 以后有第二个设备，例如地址02：
 *
 * {2, 0x02},
 */
static const device_config_t device_list[] =
{
    {1, 0x01},
    {2, 0x02},
};


#define DEVICE_COUNT \
    (sizeof(device_list) / sizeof(device_list[0]))


// ============================================================
// Queue消息结构体
// ============================================================

typedef struct
{
    /*
     * device_list[]中的下标
     *
     * 例如：
     * device_index = 0
     * 对应 device_list[0]
     */
    uint8_t device_index;


    /*
     * 网关内部设备ID
     */
    uint8_t device_id;


    /*
     * 本次Modbus读取是否成功
     *
     * true  = 本次通信成功
     * false = 本次通信失败
     */
    bool online;


    float temperature;

    float humidity;


    /*
     * 本次采集开始时间
     *
     * 单位为FreeRTOS Tick
     */
    TickType_t timestamp;

} sensor_message_t;


// ============================================================
// 每台设备自己的运行状态
// ============================================================

typedef struct
{
    /*
     * 是否已经确认过设备初始状态
     */
    bool state_known;


    /*
     * 当前确认后的最终状态
     */
    bool online;


    /*
     * 连续成功次数
     */
    uint8_t success_count;


    /*
     * 连续失败次数
     */
    uint8_t fail_count;

} device_state_t;


// ============================================================
// 每台设备轮询状态
// ============================================================

typedef struct
{
    uint8_t fail_count;

    bool slow_retry;

    TickType_t next_retry_tick;

} poll_state_t;


static poll_state_t poll_states[DEVICE_COUNT] = {0};

#define OFFLINE_RETRY_MS 10000

// ============================================================
// 全局变量
// ============================================================

static QueueHandle_t sensor_queue = NULL;


/*
 * 每个设备都有自己独立的状态
 */
static device_state_t device_states[DEVICE_COUNT] = {0};


// ============================================================
// 任务1：RS485设备轮询任务
// ============================================================

static void sensor_task(void *arg)
{
    temp_humi_data_t sensor_data;

    sensor_message_t message;


    while (1)
    {
        // ====================================================
        // 一轮依次访问所有设备
        // ====================================================

        for (uint8_t i = 0; i < DEVICE_COUNT; i++)
        {
            const device_config_t *device =
                &device_list[i];


            // -----------------------------------------------
            // 填写设备信息
            // -----------------------------------------------

            message.device_index = i;

            message.device_id =
                device->device_id;


            // -----------------------------------------------
            // 判断设备是否进入低频重试模式
            // -----------------------------------------------

            poll_state_t *poll =
            &poll_states[i];

            if (poll->slow_retry)
            {
                TickType_t now =
                    xTaskGetTickCount();

                /*
                * 还没到下一次重试时间
                */
                if ((int32_t)(
                        now - poll->next_retry_tick
                    ) < 0)
                {
                    continue;
                }
            }



            /*
             * 当前时间记录的是：
             *
             * 本次采集开始的时刻
             */
            message.timestamp =
                xTaskGetTickCount();


            // -----------------------------------------------
            // 读取温湿度
            // -----------------------------------------------

            if (temp_humi_read(
                    device->slave_addr,
                    &sensor_data
                ))
            {
                /*
                 * 本次通信成功
                 */

                poll->fail_count = 0;

                poll->slow_retry = false;

                poll->next_retry_tick  = 0;

                message.online = true;


                message.temperature =
                    sensor_data.temperature;


                message.humidity =
                    sensor_data.humidity;
                
            }
            else
            {
                message.online = false;

                message.temperature = 0.0f;
                message.humidity = 0.0f;


                poll->fail_count++;


                /*
                * 连续失败达到阈值，
                * 或者本来就已经处于低频重试状态
                */
                if (
                    poll->fail_count >= OFFLINE_FAIL_THRESHOLD
                    ||
                    poll->slow_retry
                )
                {
                    poll->slow_retry = true;


                    /*
                    * 从现在开始，
                    * 10秒以后再尝试。
                    */
                    poll->next_retry_tick =
                        xTaskGetTickCount()
                        + pdMS_TO_TICKS(
                            OFFLINE_RETRY_MS
                        );
                }
            }

            // -----------------------------------------------
            // 将本次设备采集结果发送到Queue
            // -----------------------------------------------

            if (xQueueSend(
                    sensor_queue,
                    &message,
                    pdMS_TO_TICKS(100)
                ) != pdPASS)
            {
                ESP_LOGW(
                    TAG,
                    "Sensor queue full"
                );
            }


            /*
             * 不同设备之间留100ms间隔
             *
             * 当前是同一个任务顺序轮询，
             * 所以不会发生两个设备同时发送的问题。
             */
            vTaskDelay(
                pdMS_TO_TICKS(100)
            );
        }


        /*
         * 一轮所有设备采集完成后，
         * 等待2秒，再开始下一轮。
         */
        vTaskDelay(
            pdMS_TO_TICKS(2000)
        );
    }
}


// ============================================================
// 任务2：数据处理 + 设备状态管理任务
// ============================================================

static void data_task(void *arg)
{
    sensor_message_t message;


    while (1)
    {
        /*
         * 没有数据时阻塞，
         * 不会一直占用CPU。
         */
        if (xQueueReceive(
                sensor_queue,
                &message,
                portMAX_DELAY
            ) != pdPASS)
        {
            continue;
        }


        // ====================================================
        // 获取该设备自己的状态结构
        // ====================================================

        device_state_t *state =
            &device_states[
                message.device_index
            ];


        // ====================================================
        // 本次通信成功
        // ====================================================

        if (message.online)
        {
            /*
             * 成功次数+1
             */
            state->success_count++;


            /*
             * 一旦成功，
             * 连续失败计数立即清零。
             */
            state->fail_count = 0;


            // ------------------------------------------------
            // 情况1：
            // 系统刚启动，还不知道设备初始状态
            // ------------------------------------------------

            if (!state->state_known)
            {
                /*
                 * 连续成功2次，
                 * 才认为初始状态是ONLINE。
                 */
                if (
                    state->success_count >=
                    ONLINE_SUCCESS_THRESHOLD
                )
                {
                    state->online = true;

                    state->state_known = true;


                    ESP_LOGI(
                        TAG,
                        "Device %d initial state: ONLINE",
                        message.device_id
                    );


                    /*
                     * 状态已经确认，
                     * 重新清零计数器。
                     */
                    state->success_count = 0;
                }
            }


            // ------------------------------------------------
            // 情况2：
            // 当前已经确认是OFFLINE
            // ------------------------------------------------

            else if (!state->online)
            {
                /*
                 * 连续成功2次，
                 * 才真正恢复ONLINE。
                 */
                if (
                    state->success_count >=
                    ONLINE_SUCCESS_THRESHOLD
                )
                {
                    state->online = true;


                    ESP_LOGW(
                        TAG,
                        "Device %d: OFFLINE -> ONLINE",
                        message.device_id
                    );


                    state->success_count = 0;
                }
            }


            // ------------------------------------------------
            // 正常数据处理
            // ------------------------------------------------

            printf(
                "Device %d | %.2f C | %.2f %%RH | %lu ms\n",

                message.device_id,

                message.temperature,

                message.humidity,

                (unsigned long)(
                    message.timestamp *
                    portTICK_PERIOD_MS
                )
            );
            mqtt_publish_sensor(
                message.device_id,
                message.temperature,
                message.humidity);
        }


        // ====================================================
        // 本次通信失败
        // ====================================================

        else
        {
            /*
             * 连续失败次数+1
             */
            state->fail_count++;


            /*
             * 一旦失败，
             * 连续成功计数立即清零。
             */
            state->success_count = 0;


            // ------------------------------------------------
            // 情况1：
            // 系统刚启动，还不知道设备状态
            // ------------------------------------------------

            if (!state->state_known)
            {
                /*
                 * 连续失败3次，
                 * 才确认设备初始状态为OFFLINE。
                 */
                if (
                    state->fail_count >=
                    OFFLINE_FAIL_THRESHOLD
                )
                {
                    state->online = false;

                    state->state_known = true;


                    ESP_LOGW(
                        TAG,
                        "Device %d initial state: OFFLINE",
                        message.device_id
                    );


                    state->fail_count = 0;
                }
            }


            // ------------------------------------------------
            // 情况2：
            // 当前设备已经处于ONLINE
            // ------------------------------------------------

            else if (state->online)
            {
                /*
                 * 连续失败3次，
                 * 才真正判定OFFLINE。
                 */
                if (
                    state->fail_count >=
                    OFFLINE_FAIL_THRESHOLD
                )
                {
                    state->online = false;


                    ESP_LOGW(
                        TAG,
                        "Device %d: ONLINE -> OFFLINE",
                        message.device_id
                    );


                    state->fail_count = 0;
                }
            }
        }
    }
}


// ============================================================
// app_main
// ============================================================

void app_main(void)
{
     // ========================================================
    // 1. 初始化NVS
    // Wi-Fi驱动需要使用NVS
    // ========================================================

    esp_err_t ret =
        nvs_flash_init();


    if (
        ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND
    )
    {
        ESP_ERROR_CHECK(
            nvs_flash_erase()
        );

        ESP_ERROR_CHECK(
            nvs_flash_init()
        );
    }


    // ========================================================
    // 2. 初始化Wi-Fi
    // ========================================================

    wifi_sta_init();

    // ========================================================
    // 3. 初始化MQTT
    // ========================================================

    EventGroupHandle_t wifi_event_group;

    wifi_event_group = wifi_get_event_group();


    xEventGroupWaitBits(
        wifi_event_group,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY
    );


    ESP_LOGI(TAG,"Starting MQTT...");

    time_sync_init();

    mqtt_app_start();

    // ========================================================
    // 4. 初始化Modbus
    // ========================================================

    modbus_init();


    // ========================================================
    // 4. 创建Queue
    // ========================================================

    /*
     * 最多缓存10条设备消息。
     *
     * 后面设备增加以后，
     * 比原来的5条留更多余量。
     */
    sensor_queue =
        xQueueCreate(
            10,
            sizeof(sensor_message_t)
        );


    if (sensor_queue == NULL)
    {
        ESP_LOGE(
            TAG,
            "Queue create failed"
        );

        return;
    }


    // ========================================================
    // 3. 创建RS485轮询任务
    // ========================================================

    xTaskCreatePinnedToCore(
        sensor_task,

        "sensor_task",

        4096,

        NULL,

        5,

        NULL,

        0
    );


    // ========================================================
    // 4. 创建数据处理任务
    // ========================================================

    xTaskCreatePinnedToCore(
        data_task,

        "data_task",

        4096,

        NULL,

        4,

        NULL,

        1
    );
}