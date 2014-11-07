/**
 * EIA-FR - Microprocessor 2 laboratory
 *
 * Abstract: 	Display on 7-segment modules
 *
 * Author: 	Daniel Gachet
 * Date: 	02.03.2012
 */

#include "display.h"
#include "display_fpga.h"
#include <linux/stddef.h>
#include "display_gpio.h"

/* -- Public methods definition --------------------------------------------- */

int display_init(void)
    {
    if (display_fpga_init() == 0)
	{
	return display_gpio_init();
	}
    return -1;
    }

/* ------------------------------------------------------------------------- */

void display_cleanup(void)
    {
    display_gpio_cleanup();
    display_fpga_cleanup();
    }

/* ------------------------------------------------------------------------- */

void display_seg7(uint32_t value, bool left)
    {
    if (left)
	{
	display_fpga_seg7((value / 10) % 10, true);
	display_gpio_seg7((value / 1000) % 10, true);
	}
    else
	{
	display_fpga_seg7((value / 1) % 10, false);
	display_gpio_seg7((value / 100) % 10, false);
	}
    }
