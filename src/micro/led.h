#ifndef __LED_H_
#define __LED_H_

typedef struct Led Led;
struct Led {
	uint16_t r;
	uint16_t g;
	uint16_t b;
};

void led_init(uint8_t addr);
void led_set(uint8_t addr, Led *led);



#endif
