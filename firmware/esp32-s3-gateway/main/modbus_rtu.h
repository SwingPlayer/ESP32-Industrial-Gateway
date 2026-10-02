#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>
#include <stdbool.h>

void modbus_init(void);

bool modbus_read_input_registers(
    uint8_t slave_addr,
    uint16_t start_reg,
    uint16_t reg_count,
    uint8_t *data,
    uint16_t data_size
);

#endif
