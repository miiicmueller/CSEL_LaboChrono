#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H
/**
 * EIA-FR - Microprocessor 2 laboratory
 *
 * Abstract: 	Display on 7-segment modules
 *
 * Purpose:	Module to display on the four 7-segment modules of 
 *		the APF27 extension card.
 *
 * Author: 	Daniel Gachet
 * Date: 	02.03.2012
 */

#include <linux/types.h>

/**
 * Method to initialize the 7-segment display resources
 */
extern int display_init(void);

/**
 * Method to cleanup the 7-segment display resources
 */
extern void display_cleanup(void);


/**
 * Method to display a value on the four 7-segments
 */
extern void display_seg7 (uint32_t value, bool left);

#endif
