#ifndef TEMP_HUMI_SENSOR_H
#define TEMP_HUMI_SENSOR_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    float temperature;
    float humidity;

} temp_humi_data_t;


bool temp_humi_read(
    uint8_t slave_addr,
    temp_humi_data_t *sensor_data
);

#endif