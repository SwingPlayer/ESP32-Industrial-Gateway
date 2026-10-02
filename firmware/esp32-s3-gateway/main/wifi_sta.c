#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "wifi_sta.h"
#include "freertos/event_groups.h"


static EventGroupHandle_t wifi_event_group;


#define TAG "WIFI"


// ============================================================
// 填你自己的Wi-Fi
// ============================================================

#define WIFI_SSID       "Wangluolaji"
#define WIFI_PASSWORD   "SEKAIDEKAI"


// ============================================================
// 当前Wi-Fi状态
// ============================================================

static bool s_wifi_connected = false;


// ============================================================
// Wi-Fi / IP事件处理函数
// ============================================================

static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
)
{
    // --------------------------------------------------------
    // Wi-Fi启动完成
    // --------------------------------------------------------

    if (
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START
    )
    {
        ESP_LOGI(
            TAG,
            "WiFi started, connecting..."
        );

        esp_wifi_connect();
    }


    // --------------------------------------------------------
    // Wi-Fi断开
    // --------------------------------------------------------

    else if (
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_DISCONNECTED
    )
    {
        s_wifi_connected = false;

        ESP_LOGW(
            TAG,
            "WiFi disconnected, reconnecting..."
        );


        /*
         * 自动重新连接
         */
        esp_wifi_connect();
    }


    // --------------------------------------------------------
    // 获取到IP地址
    // --------------------------------------------------------

    else if (
        event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP
    )
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;


        s_wifi_connected = true;


        ESP_LOGI(
            TAG,
            "WiFi connected"
        );


        ESP_LOGI(
            TAG,
            "IP address: " IPSTR,
            IP2STR(&event->ip_info.ip)
        );

        xEventGroupSetBits(
                wifi_event_group,
                WIFI_CONNECTED_BIT
        );
    }
}


// ============================================================
// Wi-Fi初始化
// ============================================================

void wifi_sta_init(void)
{
    // --------------------------------------------------------
    // TCP/IP网络接口初始化
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        esp_netif_init()
    );


    // --------------------------------------------------------
    // 默认事件循环
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        esp_event_loop_create_default()
    );


    // --------------------------------------------------------
    // 创建默认Wi-Fi STA网络接口
    // --------------------------------------------------------

    esp_netif_create_default_wifi_sta();


    // --------------------------------------------------------
    // Wi-Fi驱动初始化
    // --------------------------------------------------------

    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT();


    ESP_ERROR_CHECK(
        esp_wifi_init(&cfg)
    );


    // --------------------------------------------------------
    // 注册Wi-Fi事件
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL
        )
    );


    // --------------------------------------------------------
    // 注册IP事件
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL
        )
    );


    // --------------------------------------------------------
    // Wi-Fi配置
    // --------------------------------------------------------

    wifi_config_t wifi_config = {0};


    strcpy(
        (char *)wifi_config.sta.ssid,
        WIFI_SSID
    );


    strcpy(
        (char *)wifi_config.sta.password,
        WIFI_PASSWORD
    );


    // --------------------------------------------------------
    // 设置STA模式
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(
            WIFI_MODE_STA
        )
    );


    // --------------------------------------------------------
    // 设置Wi-Fi参数
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config
        )
    );


    // --------------------------------------------------------
    // 启动Wi-Fi
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        esp_wifi_start()
    );


    ESP_LOGI(
        TAG,
        "WiFi STA initialized"
    );

    wifi_event_group = xEventGroupCreate();
}


// ============================================================
// 查询Wi-Fi是否连接
// ============================================================

bool wifi_sta_is_connected(void)
{
    return s_wifi_connected;
}

EventGroupHandle_t wifi_get_event_group(void)
{
    return wifi_event_group;
}