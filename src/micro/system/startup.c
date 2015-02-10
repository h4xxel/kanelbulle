#include "LPC11xx.h"

/* Note to self: Acztronicz? */

int main(int ram, char **argv);
void system_reset();
void TIMER16_0_IRQHandler(void);

extern int __stack_end;

__attribute__((section(".after_vectors")))
void __startup() {
	extern int __data_init_start;
	extern int __data_start;
	extern int __data_end;
	extern int __bss_start;
	extern int __bss_end;
	int *cur, *target, *data;
	uint32_t ram;
	
	LPC_GPIO3->DIR |= 0x20;
	LPC_GPIO3->MASKED_ACCESS[0x20] = 0x0;
	

	/* Clear .bss */
	cur = &__bss_start;
	target = &__bss_end;
	ram = target - cur;
	while (cur != target)
		*cur++ = 0;
	
	/* Initialize variables with compiler time defined content */
	data = &__data_init_start;
	cur = &__data_start;
	target = &__data_end;
	ram += target - cur;
	while (cur != target)
		*cur++ = *data++;
	
	/* Done setting everything up! */
	main(ram, 0);
	for (;;);

	return;
}

void unhandled_irq(void) {
	return;
}

void systick_irq();

extern void (* const __vectors[])(void);
void (* const __vectors[32])(void)  __attribute__ ((section(".vectors"))) = {
	(void (*)(void)) & __stack_end,	
	__startup,
	unhandled_irq,                            // The NMI handler
	unhandled_irq,                      // The hard fault handler
	0,                      				// Reserved
	0,                      				// Reserved
	0,                      				// Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	0,                                      // Reserved
	unhandled_irq,                      	// SVCall handler
	0,                      				// Reserved
	0,                                      // Reserved
	unhandled_irq,                      	// The PendSV handler
	systick_irq,                      	// The SysTick handler
	unhandled_irq,                      // PIO0_0  Wakeup
	unhandled_irq,                      // PIO0_1  Wakeup
	unhandled_irq,                      // PIO0_2  Wakeup
	unhandled_irq,                      // PIO0_3  Wakeup
	unhandled_irq,                      // PIO0_4  Wakeup
	unhandled_irq,                      // PIO0_5  Wakeup
	unhandled_irq,                      // PIO0_6  Wakeup
	unhandled_irq,                      // PIO0_7  Wakeup
	unhandled_irq,                      // PIO0_8  Wakeup
	unhandled_irq,                      // PIO0_9  Wakeup
	unhandled_irq,                      // PIO0_10 Wakeup
	unhandled_irq,                      // PIO0_11 Wakeup
	unhandled_irq,                      // PIO1_0  Wakeup
	unhandled_irq,							// C_CAN Interrupt
	unhandled_irq, 						// SPI/SSP1 Interrupt
	unhandled_irq,                      	// I2C0
	#if 0
	unhandled_irq,                   // CT16B0 (16-bit Timer 0)          <--------------------- want
	unhandled_irq,                   // CT16B1 (16-bit Timer 1)
	unhandled_irq,                   // CT32B0 (32-bit Timer 0)
	unhandled_irq,                   // CT32B1 (32-bit Timer 1)
	unhandled_irq,                      	// SPI/SSP0 Interrupt
	unhandled_irq,                      	// UART0
	0, 				                     	// Reserved
	0,                      				// Reserved
	unhandled_irq,                      	// ADC   (A/D Converter)
	unhandled_irq,                      	// WDT   (Watchdog Timer)
	unhandled_irq,                      	// BOD   (Brownout Detect)
	0,                      				// Reserved
	unhandled_irq,                     // PIO INT3
	unhandled_irq,                     // PIO INT2
	unhandled_irq,                     // PIO INT1
	unhandled_irq,                     // PIO INT0
	#endif
};

