/**
 * EIA-FR - Microprocessor 2 laboratory
 *
 * Abstract: Display on 7-segment modules attached to the FPGA device
 *
 * Author: 	Daniel Gachet
 * Date: 	16.05.2011
 */
#include <linux/io.h>
#include <linux/ioport.h>
#include "display_fpga.h"

/* -- Internal types and constant definition -------------------------------- */

/**
 * 7-segment display and LED interface
 */
struct fpga_ctrl
    {
	uint16_t reserved1[(0x08 - 0x00) / 2];
	uint16_t seg7_rw;
	uint16_t seg7_ctrl;
	uint16_t seg7_id;
	uint16_t reserved2[1];
	uint16_t leds_rw;
	uint16_t leds_ctrl;
	uint16_t leds_id;
    };
static struct fpga_ctrl* fpga = (struct fpga_ctrl*) 0xd6000000;
/* 7-segment: segment definition

 +-- seg A --+
 |           |
 seg F       seg B
 |           |
 +-- seg G --+
 |           |
 seg E       seg C
 |           |
 +-- seg D --+
 */

#define SEG_A 0x008
#define SEG_B 0x010
#define SEG_C 0x020
#define SEG_D 0x040
#define SEG_E 0x080
#define SEG_F 0x100
#define SEG_G 0x200
static const uint16_t seg_7[] =
    {
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F, /* 0 */
    SEG_B + SEG_C, /* 1 */
    SEG_A + SEG_B + SEG_D + SEG_E + SEG_G, /* 2 */
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_G, /* 3 */
    SEG_B + SEG_C + SEG_F + SEG_G, /* 4 */
    SEG_A + SEG_C + SEG_D + SEG_F + SEG_G, /* 5 */
    SEG_A + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, /* 6 */
    SEG_A + SEG_B + SEG_C, /* 7 */
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, /* 8 */
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G, /* 9 */
    SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G, /* A */
    SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, /* b */
    SEG_A + SEG_D + SEG_E + SEG_F, /* C */
    SEG_B + SEG_C + SEG_D + SEG_E + SEG_G, /* d */
    SEG_A + SEG_D + SEG_E + SEG_F + SEG_G, /* E */
    SEG_A + SEG_E + SEG_F + SEG_G,
    /* F */
    };

/* -- Public methods definition --------------------------------------------- */

int display_fpga_init(void)
    {
    request_mem_region(0xd6000000, 0x100, "fpga");
    fpga = ioremap(0xd6000000, 4096);
    fpga->leds_ctrl = 0xff;
    fpga->leds_rw = 0;
    fpga->seg7_ctrl = 0x3ff;
    fpga->seg7_rw = 0;
    return 0;
    }

/* ------------------------------------------------------------------------- */

void display_fpga_cleanup(void)
    {
    fpga->leds_rw = 0;
    fpga->seg7_rw = 3;

    iounmap(fpga);
    release_mem_region(0xd6000000, 0x100);
    }

/* ------------------------------------------------------------------------- */

void display_fpga_seg7(uint32_t value, bool left)
    {
    if (left)
	fpga->seg7_rw = seg_7[value] + 0x2;
    else
	fpga->seg7_rw = seg_7[value] + 0x1;
    }
