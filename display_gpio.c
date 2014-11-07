/**
 * EIA-FR - Microprocessor 2 laboratory
 *
 * Abstract: 	Display on 7-segment modules attached to the GPIO of the i.MX27
 *
 * Author: 	Daniel Gachet
 * Date: 	02.03.2012
 */

#include "display_gpio.h"
#include "gpio.h"


/* -- Internal types and constant definition --------------------------------
 * 7-segment: DIG1, DIG2, DPx,  SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G 
 *            PE10, PE11, PB17, PC30,  PC31,  PB10,  PB11,  PB12,  PB13,  PB14 
 */
static const struct gpio_seg7 {
	uint32_t portc;
	uint32_t portb;
    } gpio_seg7[] = {
	{1<<30 | 1<<31,  1<<10 | 1<<11 | 1<<12 | 1<<13},
	{1<<31, 	 1<<10},
	{1<<30 | 1<<31,  1<<11 | 1<<12 | 1<<14},
	{1<<30 | 1<<31,  1<<10 | 1<<11 | 1<<14},
	{1<<31, 	 1<<10 | 1<<13 | 1<<14},
	{1<<30, 	 1<<10 | 1<<11 | 1<<13 | 1<<14},
	{1<<30, 	 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14},
	{1<<30 | 1<<31,  1<<10},
	{1<<30 | 1<<31,  1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14},
	{1<<30 | 1<<31,  1<<10 | 1<<11 | 1<<13 | 1<<14}
};


/* -- Public methods definition --------------------------------------------- */

int display_gpio_init(void)
{
	gpio_configure (GPIO_PORT_E, 1<<10 | 1<<11 | 1<<17, GPIO_OUTPUT);
	gpio_configure (GPIO_PORT_C, 1<<30 | 1<<31, GPIO_OUTPUT);
	gpio_configure (GPIO_PORT_B, 1<<10 | 1<<11 | 1<<12, GPIO_OUTPUT);
	gpio_configure (GPIO_PORT_B, 1<<13 | 1<<14 | 1<<17, GPIO_OUTPUT);
	return 0;
}

/* ------------------------------------------------------------------------- */

void display_gpio_cleanup(void)
{
	gpio_setbits (GPIO_PORT_E, 1<<10 | 1<<11);
	gpio_resetbits (GPIO_PORT_C, 1<<30 | 1<<31);
	gpio_resetbits (GPIO_PORT_B, 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<17);
	gpio_resetbits (GPIO_PORT_E, 1<<10 | 1<<11);
}

/* ------------------------------------------------------------------------- */

void display_gpio_seg7 (uint32_t value, bool left)
{
	gpio_resetbits (GPIO_PORT_E, 1<<10 | 1<<11);
	gpio_resetbits (GPIO_PORT_C, 1<<30 | 1<<31);
	gpio_resetbits (GPIO_PORT_B, 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<17);

	gpio_setbits (GPIO_PORT_E, left ? 1<<10 : 1<<11);

	gpio_setbits (GPIO_PORT_C, gpio_seg7[value].portc);
	gpio_setbits (GPIO_PORT_B, gpio_seg7[value].portb);
}
