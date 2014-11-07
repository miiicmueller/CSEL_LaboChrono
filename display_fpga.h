#pragma once
#ifndef DISPLAY_FPGA_H
#define DISPLAY_FPGA_H
/**
 * EIA-FR - Microprocessor 2 laboratory
 *
 * Abstract: 	Display on 7-segment modules attached to the FPGA device
 *
 * Purpose:	Module to display on the two 7-segment modules attached to the
 *   		FPGA device of the APF27 extension card.
 *
 * Author: 	Daniel Gachet
 * Date: 	02.03.2012
 */

#include "chronometre.h"

/**
 * method to initialize the 7-segment display resources
 * should be called prior to use any other method of this module
 */
extern int display_fpga_init(void);


/**
 * method to cleanup all the 7-segment display resources
 */
extern void display_fpga_cleanup(void);


/**
 * method to display a value on one of the 7-segments
 */
extern void display_fpga_seg7 (uint32_t value, bool left);

#endif
