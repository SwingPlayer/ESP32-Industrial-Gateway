#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "modbus_rtu.h"


#define TAG "MODBUS"


// ============================================================
// UART / RS485 配置
// ============================================================

#define UART_PORT               UART_NUM_1
#define UART_TX_PIN             17
#define UART_RX_PIN             18

#define BAUD_RATE               9600

#define UART_RX_BUF_SIZE        256


// ============================================================
// Modbus配置
// ============================================================

#define MODBUS_TIMEOUT_MS       500
#define UART_READ_WAIT_MS       20
#define MODBUS_RX_MAX_LEN       64


// ============================================================
// 内部函数声明
// ============================================================

static uint16_t modbus_crc16(
    const uint8_t *data,
    uint16_t length
);

static void print_hex(
    const uint8_t *data,
    int len
);

static int modbus_receive_frame(
    uint8_t *rx_data,
    int rx_max_len,
    uint8_t expected_slave,
    uint8_t expected_func,
    uint8_t expected_byte_count,
    int timeout_ms
);


// ============================================================
// CRC16
// ============================================================

static uint16_t modbus_crc16(
    const uint8_t *data,
    uint16_t length
)
{
    uint16_t crc = 0xFFFF;

    for (int i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}


// ============================================================
// 十六进制打印
// ============================================================

static void print_hex(
    const uint8_t *data,
    int len
)
{
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", data[i]);
    }

    printf("\n");
}


// ============================================================
// UART初始化
// ============================================================

void modbus_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };


    ESP_ERROR_CHECK(
        uart_driver_install(
            UART_PORT,
            UART_RX_BUF_SIZE,
            0,
            0,
            NULL,
            0
        )
    );


    ESP_ERROR_CHECK(
        uart_param_config(
            UART_PORT,
            &uart_config
        )
    );


    ESP_ERROR_CHECK(
        uart_set_pin(
            UART_PORT,
            UART_TX_PIN,
            UART_RX_PIN,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE
        )
    );


    ESP_LOGI(TAG, "UART initialized");
}


// ============================================================
// 接收完整Modbus响应
// ============================================================

static int modbus_receive_frame(
    uint8_t *rx_data,
    int rx_max_len,
    uint8_t expected_slave,
    uint8_t expected_func,
    uint8_t expected_byte_count,
    int timeout_ms
)
{
    int total_len = 0;

    int expected_len =
        3 + expected_byte_count + 2;


    if (expected_len > rx_max_len)
    {
        ESP_LOGE(
            TAG,
            "Expected frame too large"
        );

        return -1;
    }


    int64_t start_time =
        esp_timer_get_time();


    while (1)
    {
        // ---------- 总超时 ----------

        int64_t now =
            esp_timer_get_time();

        int elapsed_ms =
            (int)((now - start_time) / 1000);


        if (elapsed_ms >= timeout_ms)
        {
            break;
        }


        // ---------- 接收UART数据 ----------

        if (total_len < rx_max_len)
        {
            int len =
                uart_read_bytes(
                    UART_PORT,
                    &rx_data[total_len],
                    rx_max_len - total_len,
                    pdMS_TO_TICKS(
                        UART_READ_WAIT_MS
                    )
                );


            if (len > 0)
            {
                total_len += len;
            }
        }


        // ---------- 寻找正确帧 ----------

        while (total_len > 0)
        {
            // 地址不正确

            if (rx_data[0] != expected_slave)
            {
                printf(
                    "Discard: %02X (wrong address)\n",
                    rx_data[0]
                );


                memmove(
                    rx_data,
                    &rx_data[1],
                    total_len - 1
                );


                total_len--;

                continue;
            }


            // 功能码还没收到

            if (total_len < 2)
            {
                break;
            }


            // ---------- 异常响应 ----------

            if (
                rx_data[1] ==
                (expected_func | 0x80)
            )
            {
                if (total_len < 5)
                {
                    break;
                }


                uint16_t crc_calc =
                    modbus_crc16(
                        rx_data,
                        3
                    );


                uint16_t crc_recv =
                    rx_data[3] |
                    ((uint16_t)rx_data[4] << 8);


                if (crc_calc == crc_recv)
                {
                    return 5;
                }


                memmove(
                    rx_data,
                    &rx_data[1],
                    total_len - 1
                );


                total_len--;

                continue;
            }


            // ---------- 功能码检查 ----------

            if (rx_data[1] != expected_func)
            {
                printf(
                    "Discard frame head: "
                    "%02X %02X "
                    "(wrong function)\n",
                    rx_data[0],
                    rx_data[1]
                );


                memmove(
                    rx_data,
                    &rx_data[1],
                    total_len - 1
                );


                total_len--;

                continue;
            }


            // Byte Count还没收到

            if (total_len < 3)
            {
                break;
            }


            // ---------- Byte Count检查 ----------

            if (
                rx_data[2] !=
                expected_byte_count
            )
            {
                printf(
                    "Discard frame head: "
                    "%02X %02X %02X "
                    "(wrong byte count)\n",
                    rx_data[0],
                    rx_data[1],
                    rx_data[2]
                );


                memmove(
                    rx_data,
                    &rx_data[1],
                    total_len - 1
                );


                total_len--;

                continue;
            }


            // ---------- 帧还没收完整 ----------

            if (total_len < expected_len)
            {
                break;
            }


            // ---------- CRC ----------

            uint16_t crc_calc =
                modbus_crc16(
                    rx_data,
                    expected_len - 2
                );


            uint16_t crc_recv =
                rx_data[expected_len - 2] |
                ((uint16_t)
                 rx_data[expected_len - 1]
                 << 8);


            if (crc_calc == crc_recv)
            {
                return expected_len;
            }


            printf(
                "CRC candidate error, "
                "continue searching...\n"
            );


            memmove(
                rx_data,
                &rx_data[1],
                total_len - 1
            );


            total_len--;
        }
    }


    // ---------- 超时 ----------

    if (total_len == 0)
    {
        ESP_LOGE(
            TAG,
            "Modbus timeout: no valid response"
        );
    }
    else
    {
        ESP_LOGE(
            TAG,
            "Modbus timeout: %d bytes remaining",
            total_len
        );


        printf("RX remaining: ");

        print_hex(
            rx_data,
            total_len
        );
    }


    return 0;
}


// ============================================================
// 通用04功能码：读取输入寄存器
// ============================================================

bool modbus_read_input_registers(
    uint8_t slave_addr,
    uint16_t start_reg,
    uint16_t reg_count,
    uint8_t *data,
    uint16_t data_size
)
{
    // ---------- 参数检查 ----------

    if (data == NULL)
    {
        ESP_LOGE(
            TAG,
            "Data pointer is NULL"
        );

        return false;
    }


    if (
        reg_count == 0 ||
        reg_count > 125
    )
    {
        ESP_LOGE(
            TAG,
            "Invalid register count: %u",
            reg_count
        );

        return false;
    }


    uint16_t expected_data_bytes =
        reg_count * 2;


    if (data_size < expected_data_bytes)
    {
        ESP_LOGE(
            TAG,
            "Data buffer too small"
        );

        return false;
    }


    uint16_t expected_frame_len =
        3 + expected_data_bytes + 2;


    if (
        expected_frame_len >
        MODBUS_RX_MAX_LEN
    )
    {
        ESP_LOGE(
            TAG,
            "Response too large"
        );

        return false;
    }


    // ---------- 组请求帧 ----------

    uint8_t tx_data[8];


    tx_data[0] =
        slave_addr;


    tx_data[1] =
        0x04;


    tx_data[2] =
        (start_reg >> 8)
        & 0xFF;


    tx_data[3] =
        start_reg
        & 0xFF;


    tx_data[4] =
        (reg_count >> 8)
        & 0xFF;


    tx_data[5] =
        reg_count
        & 0xFF;


    // ---------- CRC ----------

    uint16_t crc =
        modbus_crc16(
            tx_data,
            6
        );


    tx_data[6] =
        crc & 0xFF;


    tx_data[7] =
        (crc >> 8) & 0xFF;


    // ---------- 打印请求 ----------

    printf("\n");

    printf(
        "========== MODBUS READ ==========\n"
    );


    printf("TX: ");

    print_hex(
        tx_data,
        sizeof(tx_data)
    );


    // ---------- 清空旧数据 ----------

    uart_flush_input(
        UART_PORT
    );


    // ---------- 发送 ----------

    int tx_len =
        uart_write_bytes(
            UART_PORT,
            (const char *)tx_data,
            sizeof(tx_data)
        );


    if (
        tx_len !=
        sizeof(tx_data)
    )
    {
        ESP_LOGE(
            TAG,
            "UART send failed"
        );

        return false;
    }


    // ---------- 等待发送结束 ----------

    esp_err_t ret =
        uart_wait_tx_done(
            UART_PORT,
            pdMS_TO_TICKS(100)
        );


    if (ret != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "UART TX timeout"
        );

        return false;
    }


    // ---------- 接收 ----------

    uint8_t rx_data[
        MODBUS_RX_MAX_LEN
    ] = {0};


    int frame_len =
        modbus_receive_frame(
            rx_data,
            sizeof(rx_data),

            slave_addr,

            0x04,

            expected_data_bytes,

            MODBUS_TIMEOUT_MS
        );


    if (frame_len <= 0)
    {
        return false;
    }


    printf(
        "RX LEN = %d\n",
        frame_len
    );


    printf("RX: ");

    print_hex(
        rx_data,
        frame_len
    );


    // ---------- Modbus异常响应 ----------

    if (rx_data[1] == 0x84)
    {
        ESP_LOGE(
            TAG,
            "Modbus exception code: 0x%02X",
            rx_data[2]
        );

        return false;
    }


    // ---------- 最终检查 ----------

    if (
        rx_data[0] != slave_addr ||
        rx_data[1] != 0x04 ||
        rx_data[2] != expected_data_bytes
    )
    {
        ESP_LOGE(
            TAG,
            "Invalid Modbus response"
        );

        return false;
    }


    if (
        frame_len !=
        expected_frame_len
    )
    {
        ESP_LOGE(
            TAG,
            "Invalid frame length"
        );

        return false;
    }


    // ---------- 只返回真正数据区 ----------

    memcpy(
        data,
        &rx_data[3],
        expected_data_bytes
    );


    return true;
}
