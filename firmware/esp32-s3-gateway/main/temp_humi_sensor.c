#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "temp_humi_sensor.h"
#include "modbus_rtu.h"


#define TAG "TEMP_HUMI"


// ============================================================
// 当前温湿度传感器的Modbus参数
// ============================================================

// 起始寄存器
#define SENSOR_START_REG        0x0002

// 温度 + 湿度一共4个寄存器
#define SENSOR_REG_COUNT        4

// 4个寄存器 × 2字节 = 8字节
#define SENSOR_DATA_SIZE        8


// ============================================================
// 大端4字节 → float
// ============================================================

static float bytes_to_float_be(
    const uint8_t *data
)
{
    uint32_t value =
        ((uint32_t)data[0] << 24) |
        ((uint32_t)data[1] << 16) |
        ((uint32_t)data[2] << 8)  |
        ((uint32_t)data[3]);


    float result;


    memcpy(
        &result,
        &value,
        sizeof(float)
    );


    return result;
}


// ============================================================
// 读取温湿度
// ============================================================

bool temp_humi_read(
    uint8_t slave_addr,
    temp_humi_data_t *sensor_data
)
{
    // ---------- 参数检查 ----------

    if (sensor_data == NULL)
    {
        ESP_LOGE(
            TAG,
            "sensor_data is NULL"
        );

        return false;
    }


    // ---------- 接收原始8字节数据 ----------

    uint8_t raw_data[
        SENSOR_DATA_SIZE
    ];


    bool success =
        modbus_read_input_registers(
            slave_addr,
            SENSOR_START_REG,
            SENSOR_REG_COUNT,
            raw_data,
            sizeof(raw_data)
        );

    if (!success)
    {
        ESP_LOGE(
            TAG,
            "Sensor read failed"
        );

        return false;
    }


    // ---------- 前4字节：温度 ----------

    sensor_data->temperature =
        bytes_to_float_be(
            &raw_data[0]
        );


    // ---------- 后4字节：湿度 ----------

    sensor_data->humidity =
        bytes_to_float_be(
            &raw_data[4]
        );


    return true;
}
