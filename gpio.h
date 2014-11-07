#pragma once
#ifndef GPIO_H
#define GPIO_H
/**
 * EIA-FR - Microprocessor 2 laboratory
 *
 * Abstract: 	i.MX27 GPIO
 *
 * Purpose:	Module to deal with the general purpose I/O (GPIO) the i.MX27.
 *
 * Author: 	Daniel Gachet
 * Date: 	02.03.2012
 */

#include <linux/types.h>

/**
 * List of GPIO ports of the i.MX27
 */
enum gpio_ports {GPIO_PORT_A, GPIO_PORT_B, GPIO_PORT_C,
		 GPIO_PORT_D, GPIO_PORT_E, GPIO_PORT_F,
		 GPIO_NB_OF_PORTS};

/**
 * GPIO operation modes
 */
enum gpio_modes {GPIO_INPUT, 
		 GPIO_OUTPUT/*, 
		 GPIO_IRQ_RISING, 
		 GPIO_IRQ_FALLING, 
		 GPIO_IRQ_HIGH, 
		 GPIO_IRQ_LOW*/};

/**
 * GPIO interrupt modes
 */
enum gpio_interrupt_modes {GPIO_IRQ_RISING, 
			   GPIO_IRQ_FALLING, 
			   GPIO_IRQ_HIGH, 
			   GPIO_IRQ_LOW};

/**
 * Prototype of the interrupt service routine
 */
typedef void (*gpio_isr_t) (void* handle);


/**
 * method to initialize the GPIO resources
 * should be called prior any other methods of this module
 */
extern int gpio_init();


/**
 * method to release all the GPIO resources
 */
extern void gpio_cleanup(void);


/**
 * method to configure I/O ports either as output or as interrupt source (falling edge).
 *
 * @param port port number to be configured
 * @param bitmask list of pins to be configured
 * @param mode operation mode of the pins to be configured
 * @return execution status (0=success, -1=error)
 */
extern int gpio_configure (enum gpio_ports port, uint32_t bitmask, enum gpio_modes mode);

/**
 * method to get the status of the specified pin
 *
 * @param port port number to be configured
 * @return bitmask 
 */
extern uint32_t gpio_getbits (enum gpio_ports port);


/**
 * method to set all specified pins to 1
 *
 * @param port port number to be configured
 * @param bitmask list of pins to be configured
 */
extern void gpio_setbits (enum gpio_ports port, uint32_t bitmask);


/**
 * method to reset all specified pins to 0
 *
 * @param port port number to be configured
 * @param bitmask list of pins to be configured
 */
extern void gpio_resetbits (enum gpio_ports port, uint32_t bitmask);


/**
 * method to attach an interrupt service routine a pin previously 
 * configured as interrupt source
 *
 * @param port port number to which the ISR should be attached
 * @param pin pin number to which the ISR should be attached
 * @param mode interrupt mode
 * @param routine application specific interrupt service routine
 * @param handle paramter passed as argument to the ISR when called
 * @return execution status (0=success, -1=error)
 */
extern int gpio_attach (
	enum gpio_ports port, 
	uint32_t pin, 
	enum gpio_interrupt_modes mode, 
	gpio_isr_t routine, 
	void* handle);

/**
 * method to detach the interrupt service routine
 *
 * @param port port number from which the ISR should be detached
 * @param pin pin number from which the ISR should be detached
 */
extern void gpio_detach (enum gpio_ports port, uint32_t pin);

/**
 * method to enable interrupt on the specified pin
 *
 * @param port port number to be enabled
 * @param pin pin number to be enabled
 */
extern void gpio_enable(enum gpio_ports port, uint32_t pin);


/**
 * method to disable interrupt on the specified pin
 *
 * @param port port number to be disabled
 * @param pin pin number to be disabled
 */
extern void gpio_disable(enum gpio_ports port, uint32_t pin);


#endif
