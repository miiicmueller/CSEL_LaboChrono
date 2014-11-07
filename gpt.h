/*
 * gpt.h
 *
 *  Created on: Nov 7, 2014
 *      Author: lmi
 */

#ifndef GPT_H_
#define GPT_H_

#include "chronometre.h"
#include <linux/stddef.h>

/**
 * GPT Control Register (TCTL) bit fields
 */
#define GPT_TCTL_SWR		(1<<15)		/* software reset */
#define GPT_TCTL_CC		(1<<10)		/* counter clear */
#define GPT_TCTL_OM		(1<<9)		/* output mode: 0=active low pulse/clock, 1=toggle mode */
#define GPT_TCTL_FRR		(1<<8)		/* free-run/restart mode: 0=restart, 1=free-run */
#define GPT_TCTL_CAP_MSK	(0b11<<6)	/* capture edge mask*/
#define GPT_TCTL_CAP_RISING	(0b01<<6)	/*   --> capture on rising edge */
#define GPT_TCTL_CAP_FALLING	(0b10<<6)	/*   --> capture on falling edge */
#define GPT_TCTL_CAP_BOTH	(0b11<<6)	/*   --> capture on both edges */
#define GPT_TCTL_CAP_EN		(1<<5)		/* capture interrupt enable */
#define GPT_TCTL_COMP_EN	(1<<4)		/* compare interrupt enable */
#define GPT_TCTL_CLKSRC_MSK	(0b111<<1)	/* clock source mask */
#define GPT_TCTL_CLKSRC_CLK1	(0b001<<1)	/*   --> PERCLK1 to prescaler */
#define GPT_TCTL_CLKSRC_CLK1_4	(0b010<<1)	/*   --> PERCLK1 divided by 4 to prescaler */
#define GPT_TCTL_CLKSRC_TIN	(0b011<<1)	/*   --> TIN to prescaler */
#define GPT_TCTL_CLKSRC_32K	(0b100<<1)	/*   --> 32kHz clock to prescaler */
#define GPT_TCTL_TEN		(1<<0)		/* timer enable */

/**
 * GPT Control Register (TCTL) bit fields
 */
#define GPT_TSTAT_CAPT		(1<<1)		/* capture event acknowledge */
#define GPT_TSTAT_COMP		(1<<0)		/* compare event acknowledge */

#define GPT1_BASE_ADDR  0x10003000
#define GPT2_BASE_ADDR  0x10004000
#define GPT3_BASE_ADDR  0x10005000
#define GPT4_BASE_ADDR  0x10019000
#define GPT5_BASE_ADDR  0x1001a000
#define GPT6_BASE_ADDR  0x1001f000

#define GPT_REG_SIZE 	0x18

/**
 * GPT Register Definition (timer 1 to 6)
 */
typedef struct
    {
	uint32_t tctl; /* control register */
	uint32_t tprer; /* prescaler */
	uint32_t tcmp; /* compare register */
	uint32_t tcr; /* capture register */
	uint32_t tcn; /* counter register */
	uint32_t tstat; /* status register */
    } gptRegsStruct;

typedef struct
    {
	gptRegsStruct* gptRegisters;
	struct stopwatch* stopWatch;
    } gptDevStruct;

/**
 * Functions
 */

void mGpt_EnableGpts();
void mGpt_DisableGpts();

/**
 * Base time = 1ms
 */
void mGpt_Open(uint32_t aPerdiodGpt1, uint32_t aPerdiodGpt2);
int mGpt_Setup(struct stopwatch* aStopWatchStruct);
void mGpt_Release(void);

#endif /* GPT_H_ */
