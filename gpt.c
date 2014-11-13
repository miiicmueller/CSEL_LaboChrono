/*
 * gpt.c
 *
 *  Created on: Nov 7, 2014
 *      Author: lmi
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <mach/mx27.h>
#include "gpt.h"
#include "display.h"

static struct resource *aIOMemResource = NULL;

irqreturn_t ISR_GPT_handler(int irq, void *dev_id)
    {
    gptDevStruct *aGpt = (gptDevStruct*) dev_id;

    debugprint("GPT  ISR");

    writel(GPT_TSTAT_COMP, &aGpt->gptRegisters->tstat);

    return IRQ_WAKE_THREAD;

    }

void mGpt_EnableGpts(gptDevStruct *aGpt)
    {
    uint32_t aTemp = readl(&aGpt->gptRegisters->tctl);
    writel(aTemp | GPT_TCTL_TEN, &aGpt->gptRegisters->tctl);
    }

void mGpt_DisableGpts(gptDevStruct *aGpt)
    {
    uint32_t aTemp = readl(&aGpt->gptRegisters->tctl);
    writel(aTemp & ~(GPT_TCTL_TEN), &aGpt->gptRegisters->tctl);

    }

/**
 * Base time = 1ms
 */
void mGpt_Open(gptDevStruct *aGpt, uint32_t aPerdiodGpt)
    {
    writel(aPerdiodGpt, &aGpt->gptRegisters->tcmp);
    }

int mGpt_Setup(gptDevStruct *aGpt, gptTypeEnum aGptTypeEnum,
	struct stopwatch* aStopWatchStruct,
	irqreturn_t* (aIrqThread)(int irq, void* dev_id))
    {

    debugprint("Gpt initialization");

    //Tell the kernel about the memory region used
    aIOMemResource = request_mem_region(getGptBaseAddress(aGptTypeEnum),
	    GPT_REG_SIZE, "Gpt 2");

    if (aIOMemResource == NULL)
	{
	printk(KERN_ERR "request_mem_region ! Gpt is not accessible");
	// Error happened
	return -1;
	}

    //Remap the physical memory to the right place
    aGpt->gptRegisters = ioremap(getGptBaseAddress(aGptTypeEnum), 4096);

    // Test if the returned pointer is null
    if ((aGpt->gptRegisters == NULL))
	{
	printk(
		KERN_ERR "Ioremap failed ! Memory for Gpt is not accessible");
	//free the memory locaiton
	release_mem_region(getGptBaseAddress(aGptTypeEnum), GPT_REG_SIZE);

	// Error happened
	return -1;
	}

    // GPT configuration
    writel(GPT_TCTL_SWR, &aGpt->gptRegisters->tctl); /* force a software reset */
    while ((readl(&aGpt->gptRegisters->tctl) & GPT_TCTL_SWR) != 0)
	;

    writel(GPT_TCTL_CLKSRC_32K | GPT_TCTL_COMP_EN, &aGpt->gptRegisters->tctl);
    writel(32 - 1, &aGpt->gptRegisters->tprer); /* --> 1ms clock */

    // Register the interrupts handlers
    if (0
	    != request_threaded_irq(aGptTypeEnum, ISR_GPT_handler, aIrqThread,
		    0, "gpt_irq", aGpt))
	{
	printk(
	KERN_ERR "IRQ request for GPT failed !");

	//Umap the memory

	iounmap(aGpt->gptRegisters);
	release_mem_region(getGptBaseAddress(aGptTypeEnum), GPT_REG_SIZE);

	return -1;
	}

    aGpt->stopWatch = aStopWatchStruct;
    aGpt->gptType = aGptTypeEnum;

    return 0;
    }

void mGpt_Release(gptDevStruct *aGpt)

    {
    // Stop the timers
    mGpt_DisableGpts(aGpt);

    //Release irq
    free_irq(aGpt->gptType, aGpt);

    //Umap the memory
    iounmap(aGpt->gptRegisters);
    release_mem_region(getGptBaseAddress(aGpt->gptType), GPT_REG_SIZE);
    release_mem_region(getGptBaseAddress(aGpt->gptType), GPT_REG_SIZE);

    }
