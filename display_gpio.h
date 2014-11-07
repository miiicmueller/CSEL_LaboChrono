#pragma once
#ifndef DISPLAY_GPIO_H
#define DISPLAY_GPIO_H
/**
 * EIA-FR - Microprocessor 2 laboratory
 *
 * Abstract: 	Display on 7-segment modules attached to the GPIO of the i.MX27
 *
 * Purpose:	Module to display on the two 7-segment modules attached to the
 *   		GPIO interface of the i.MX27 connected to the APF27 extension 
 *		card.
 *
 * Author: 	Daniel Gachet
 * Date: 	02.03.2012
 */
#include <linux/types.h>
#include <linux/stddef.h>

/**
 * method to initialize the 7-segment display resources
 * should be called prior to use any other method of this module
 */
extern int display_gpio_init(void);

/**
 * method to cleanup all the 7-segment display resources
 */
extern void display_gpio_cleanup(void);

/**
 * method to display a value on one of the 7-segments
 */
extern void display_gpio_seg7 (uint32_t value, bool left);

#endif
