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
#include <stdarg.h>
#include <limits.h>
#include "system/LPC11xx.h"
#include "uart.h"
#include "util.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PRINT_TYPE(type, argtype) \
s = int_to_string((type) va_arg(va, argtype), buf + 24, base); \
j = 24 + buf - s; \
if(!width) \
	width = j; \
else \
	while(width > j)  {\
		uart_send_char(pad); \
		width--; \
	} \
uart_send_raw((unsigned char *) s, width)


void uart_init() {
	unsigned int regval;
	
	LPC_SYSCON->UARTCLKDIV = 1;
	/* Enable RXD, TXD on the IO pins */
	LPC_IOCON->PIO1_6 &= ~0x7;
	LPC_IOCON->PIO1_6 |= 1;
	LPC_IOCON->PIO1_7 &= ~0x7;
	LPC_IOCON->PIO1_7 |= 1;

	/* Enable UART clock bit */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12);
	/* Set up FIFO */
	LPC_UART->FCR = 0x7;
	/* Set line control (stop bits etc.) */
	LPC_UART->LCR = 0x83;
	regval = ((SYSTEM_CLOCK/LPC_SYSCON->SYSAHBCLKDIV)/16/UART_BAUD_RATE);
	LPC_UART->FDR = 0x10;
	LPC_UART->DLL = regval & 0xFF;
	LPC_UART->DLM = (regval >> 8) & 0xFF;
	LPC_UART->LCR = 0x3;
}


void uart_send_char(unsigned char c) {
	while (!(LPC_UART->LSR & (1 << 5)));
	LPC_UART->THR = c;
}

void uart_send_raw(unsigned char *data, int bytes) {
	int i;
	for (i = 0; i < bytes; i++) {
		while (!(LPC_UART->LSR & (1 << 5)));
		LPC_UART->THR = data[i];
	}

	return;
}

void uart_send_string (char *s) {
	while(*s) {
		while (!(LPC_UART->LSR & (1 << 5)));
		LPC_UART->THR = *s++;
	}
}

static char *int_to_string(unsigned long long int n, char *s, int base) {
	char i;
	do {
		s--;
		i = n  % base;
		i += (i > 9 ? 'A' - 10 : '0');
		*s = i;
		n /= base;
	} while(n);
	return s;
}

int uart_printf(char *format, ...) {
	#ifdef DEBUG
	//TODO: handle signed values
	unsigned char pad, c;
	int i;
	unsigned int j;
	va_list va;
	
	enum {
		LENGTH_CHAR,
		LENGTH_SHORT,
		LENGTH_INT,
		LENGTH_LONG,
		LENGTH_LONG_LONG,
		LENGTH_INTMAX_T,
		LENGTH_SIZE_T,
		LENGTH_PTRDIFF_T,
	} length;
	int width;
	char prefix, base, *s;
	char buf[25];
	buf[24] = 0;
	
	va_start(va, format);
	for(i=0; (c = *format++); i++) {
		if(c != '%') {
			uart_send_char(c);
			continue;
		}
		length = LENGTH_INT;
		width = 0;
		prefix = 0;
		pad = ' ';
		
		while(1) {
			switch(c = *format++) {
				case 0:
					goto end;
				case '%':
					uart_send_char(c);
					goto next;
				case '#':
					prefix = 1;
					break;
				case '0':
					if(!width) {
						pad = '0';
						break;
					}
				case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					width = width*10 + (c - '0');
					break;
				case 'h':
					length = length == LENGTH_SHORT ? LENGTH_CHAR : LENGTH_SHORT;
					break;
				case 'l':
					length = length == LENGTH_LONG ? LENGTH_LONG_LONG : LENGTH_LONG;
					break;
				case 'j':
					length = LENGTH_INTMAX_T;
					break;
				case 'z':
					length = LENGTH_SIZE_T;
					break;
				case 't':
					length = LENGTH_PTRDIFF_T;
					break;
				case 'o':
					base = 8;
					if(prefix)
						uart_send_char('0');
					goto baseconv;
				case 'p':
					length = sizeof(void *);
					prefix = 1;
				case 'x':
				case 'X':
					base = 16;
					if(prefix)
						uart_send_string("0x");
					goto baseconv;
				case 'u':
				case 'd':
				case 'i':
					base = 10;
					baseconv:
					switch(length) {
						case LENGTH_CHAR:
							PRINT_TYPE(unsigned char, unsigned int);
							break;
						case LENGTH_SHORT:
							PRINT_TYPE(unsigned short, unsigned int);
							break;
						case LENGTH_INT:
							PRINT_TYPE(unsigned int, unsigned int);
							break;
						case LENGTH_LONG:
							PRINT_TYPE(unsigned long, unsigned long);
							break;
						case LENGTH_LONG_LONG:
							PRINT_TYPE(unsigned long long, unsigned long long);
							break;
						default:
							break;
					}
					goto next;
				case 's':
					uart_send_string(va_arg(va, char *));
					goto next;
			}
		}
		next:;
	}
	end:
	va_end(va);
	return i;
	#else
	return 0;
	#endif
}

uint8_t uart_recv_char(void) {
	while(!(LPC_UART->LSR & 1));
	return LPC_UART->RBR;
}


void uart_recv_raw(unsigned char *buff, int bytes) {
	int i;

	for (i = 0; i < bytes; i++)
		buff[i] = uart_recv_char();
	
	return;
}


uint16_t uart_recv_try(void) {
	if (LPC_UART->LSR & 1)
		return LPC_UART->RBR | 0x100;
	return 0;
}
