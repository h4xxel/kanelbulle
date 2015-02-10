/*
Copyright 2015 Steven Arnow <stevena@kth.se>

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

#include <stdint.h>
#include <stdbool.h>
#include "system/LPC11xx.h"

void i2c_init() {
	LPC_I2C->CONCLR = 0x6C;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
	LPC_SYSCON->PRESETCTRL |= 0x2;

	/* Set pin functions, set I2C duty cycle */
	LPC_IOCON->PIO0_4 = 0x1;
	LPC_IOCON->PIO0_5 = 0x1;
	LPC_I2C->SCLH = 60;
	LPC_I2C->SCLL = 60;

	/* Start listening on I2C */
	LPC_I2C->CONSET |= (1<<6);
}


void i2c_send_stop() {
	LPC_I2C->CONSET = 0x10;
}


bool i2c_write_byte(uint8_t data) {
	LPC_I2C->DAT = data;
	/* Clear SI */
	LPC_I2C->CONCLR = 0x8;
	LPC_I2C->DAT = data;

	/* Wait for reply */
	for (;;) {
		switch (LPC_I2C->STAT & 0xF8) {
			case 0x18:	/* ACK recv */
				return true;
			case 0x20:	/* nACK, device not ready */
				/* Send stop flag to clear the bus */
				LPC_I2C->CONSET |= (1<<4);
				return false;
			case 0x38:
				return false;
			default:
				continue;
			
		}
	}

	return false;
}


uint8_t i2c_read_byte() {
	LPC_I2C->CONCLR = (0x8);
	
	/* Wait for data */
	for (;;) {
		switch (LPC_I2C->STAT & 0xF8) {
			case 0x50:
				return LPC_I2C->DAT;
			case 0x58:
				return 0xFF;
			default:
				continue;
		}
	}
}


bool i2c_select_slave(uint8_t slave) {
	/* Wait for sent start condition */
	LPC_I2C->CONSET |= (1<<5);
	while ((LPC_I2C->STAT & 0xF8) != 0x8);
	LPC_I2C->CONCLR = 0x20;

	LPC_I2C->DAT = slave;

	/* Clear start condition, signal that data is ready to send */
	LPC_I2C->CONCLR = 0x20;

	i2c_write_byte(slave);
	/* Should not happen */
	return false;
}

/*
void compass_test() {
	i2c_select_slave(0x3C);
	i2c_write_byte(0x
}*/
