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

#ifndef __UART_H__
#define	__UART_H__

#include <stdint.h>

void uart_init();
void uart_send_raw(unsigned char *data, int bytes);
void uart_send_char(unsigned char c);
void uart_send_string(char *s);
void uart_send_hex(unsigned int data);
int uart_printf(char *format, ...);
uint8_t uart_recv_char(void);
void uart_recv_raw(unsigned char *buff, int bytes);
int16_t uart_recv_try(void);
void uart_loop();


#endif
