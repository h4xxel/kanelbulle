/*
Copyright 2014 Steven Arnow <stevena@kth.se>
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

#include <stdint.h>
#include "system/LPC11xx.h"
#include "util.h"
#include "uart.h"
#include "main.h"
#include "led.h"
#include "i2c.h"

int global_timer;

void initialize(void) {
	/* TODO: Set CPU clock etc. */
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);
	LPC_SYSCON->SYSAHBCLKDIV = 0x1;
	LPC_SYSCON->PDRUNCFG &= ~0x80;
	LPC_SYSCON->SYSPLLCTRL = 0x23;
	LPC_SYSCON->SYSPLLCLKSEL = 0x0;
	LPC_SYSCON->SYSPLLCLKUEN = 0x0;
	LPC_SYSCON->SYSPLLCLKUEN = 0x1;
	while(!(LPC_SYSCON->SYSPLLSTAT & 1));
	LPC_SYSCON->MAINCLKSEL = 0x3;
	LPC_SYSCON->MAINCLKUEN = 0x1;
	
	/*********** Enable UART0 **********/
	uart_init();
	
	/* Enable timers */
	LPC_SYSCON->SYSAHBCLKDIV |= (1 << 10);
	LPC_SYSCON->SYSAHBCLKDIV |= (1 << 9);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);

	/*Disable systick*/
	SysTick->CTRL = 0;
}

void systick_irq() {
	global_timer++;
}

void systick_enable() {
	/* Trig 8000 times per second */
	SysTick->LOAD = SYSTEM_CLOCK / 8000;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x1 | 0x2 | 0x4;
}

Led led[5] = {};
struct {
	int produced;
	int consumed;
} energy;

void update_leds() {
	int prod, cons, i;
	
	prod = energy.produced/20;
	cons = energy.consumed/20;
	
	for(i = 0; i < 5; i++) {
		led[i].g = i <= prod ? 0xFFF : 0x0;
		led[i].r = i > cons ? 0xFFF : 0x0;
	}
	
	//led_set(0xAA, led);
}

int do_cmd(const char *cmd) {
	switch(*cmd) {
		case 'P':
			energy.produced = atoi(cmd + 2);
			break;
		case 'C':
			energy.consumed = atoi(cmd + 2);
			break;
		case 'S':
			break;
		default:
			return -1;
	}
	return 0;
}

void read_cmd() {
	char buffer[33];
	buffer[32] = 0x0;
	int i = 0;
	
	for(;;) {
		update_leds();
		while(i < 32) {
			if((buffer[i] = uart_recv_char()) == '\n') {
				buffer[i] = 0;
				i = 0;
				if(do_cmd(buffer) < 0)
					uart_send_string("N\n");
				else
					uart_send_string("A\n");
				continue;
			}
		}
		while(uart_recv_char() != '\n');
		uart_send_string("N\n");
		i = 0;
	}		
}

int main(int ram, char **argv) {
	int i;
	initialize();
	util_delay(2000000);

	systick_enable();
	uart_printf("AT");
	uart_recv_char();
	uart_recv_char();
	uart_printf("AT+NAMESolplugg");
	/* OK */
	for (i = 0; i < 2; i++)
		uart_recv_char();
	/* setname */
	for (i = 0; i < 7; i++)
		uart_recv_char();
	uart_printf("AT+PIN0000");
	for (i = 0; i < 2; i++)
		uart_recv_char();
	for (i = 0; i < 7; i++)
		uart_recv_char();
	
	//i2c_init();
	//led_init(0xAA);
	read_cmd();
	
	return 0;
}
