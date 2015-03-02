#include "system/LPC11xx.h"
#include <stdint.h>
#include "led.h"
#include "i2c.h"

typedef enum LedReg LedReg;
enum LedReg {
	LED_REG_MODE1 = 0x0,
	LED_REG_MODE2,
	LED_REG_SUBADDR1,
	LED_REG_SUBADDR2,
	LED_REG_SUBADDR3,
	LED_REG_ALLCALLADDR,
	LED_REG_LED,
	
	LED_REG_ALL_LED_ON_L = 0xFA,
	LED_REG_ALL_LED_ON_H,
	LED_REG_ALL_LED_OFF_L,
	LED_REG_ALL_LED_OFF_H,
	LED_REG_PRE_SCALE = 0xFE,
	LED_REG_TEST_MODE,
};

void led_init(uint8_t addr) {
	i2c_select_slave(addr);
	i2c_write_byte(LED_REG_MODE1);
	i2c_write_byte(0x20);
	i2c_send_stop();
	
	i2c_select_slave(addr);
	i2c_write_byte(LED_REG_MODE2);
	i2c_write_byte(0x4);
	i2c_send_stop();
	
	LPC_IOCON->PIO0_3 = 0x0;
	LPC_GPIO0->DIR |= (1 << 3);
	
	LPC_GPIO0->MASKED_ACCESS[(1 << 3)] = 0x0;
}

void led_set(uint8_t addr, Led *led) {
	int i;
	
	i2c_select_slave(addr);
	/*Address of first led*/
	i2c_write_byte(LED_REG_LED);
	
	for(i = 0; i < 5; i++) {
		i2c_write_byte(0x0);
		i2c_write_byte(0x0);
		i2c_write_byte(led[i].b);
		i2c_write_byte(led[i].b >> 8);
	}
	
	for(i = 0; i < 5; i++) {
		i2c_write_byte(0x0);
		i2c_write_byte(0x0);
		i2c_write_byte(led[i].g);
		i2c_write_byte(led[i].g >> 8);
	}
	
	for(i = 0; i < 5; i++) {
		i2c_write_byte(0x0);
		i2c_write_byte(0x0);
		i2c_write_byte(led[i].r);
		i2c_write_byte(led[i].r >> 8);
	}
	
	i2c_send_stop();
}
