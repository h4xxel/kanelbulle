#ifndef __I2C_H_
#define __i2C_H_

#include <stdbool.h>
#include <stdint.h>

void i2c_init();
void i2c_send_stop();
bool i2c_write_byte(uint8_t data);
uint8_t i2c_read_byte();
bool i2c_select_slave(uint8_t slave);


#endif
