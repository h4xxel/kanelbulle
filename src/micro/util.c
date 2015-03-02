/*
Copyright 2014, 2015 Steven Arnow <stevena@kth.se>
Copyright 2014 Axel Isaksson <axelis@kth.se>

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
 
#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"
#include <stdbool.h>

volatile inline int *gpio_reg_addr(unsigned int port, unsigned int offset) {
	return (volatile int *) (0x50000000 + port * 0x10000 + offset);
}


inline void gpio_set_io(int port, int pin, int status) {
	*gpio_reg_addr(port, 0x8000) |= (1 << pin);
	*gpio_reg_addr(port, 0x8000) ^= (1 << pin);
	*gpio_reg_addr(port, 0x8000) |= (status << pin);
}


inline void gpio_set_pin(int port, int pin, int data) {
	*gpio_reg_addr(port, 0x4 << pin) = data;
}


void util_delay_small(int us) {
	int n;

	LPC_TMR16B0->TCR |= 0x3;
	LPC_TMR16B0->PR = 0;
	n = SYSTEM_CLOCK / 1000000 * us;
	LPC_TMR16B0->TCR &= (~0x2);
	LPC_TMR16B0->CTCR = 0x0;
	while ((LPC_TMR16B0->TC) < n);
	LPC_TMR16B0->TCR = 0x3;

	return;
}


void util_delay(int us) {
	int us_out;

	for (; us > 0;) {
		if (us > 1000)
			us_out = 1000, us -= 1000;
		else
			us_out = us, us = 0;
		util_delay_small(us_out);
	}
}


void *memcpy(void *dest, void *src, int bytes) {
	char *dst, *sr;
	
	dst = dest;
	sr = src;
	dest += bytes;
	while (dst != dest)
		*(dst++) = *(sr++);
	return dest;
}


void system_reset() {
	while (!(LPC_GPIO3->DATA & 0x2));
	while(!(LPC_UART->LSR & 0x40));
	NVIC_SystemReset();

	for (;;);
	return;
}


int atoi(const char *buff) {
	int i;
	bool neg;

	for (; *buff == ' '; buff++);

	(neg = (*buff == '-'));
	if (neg)
		buff++;
	
	i = 0;
	for (; *buff >= '0' && *buff <= '9'; buff++)
		i *= 10, i += (*buff - '0');
	if (neg)
		i *= -1;
	return i;
}
