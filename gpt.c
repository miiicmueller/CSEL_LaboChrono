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

static gptDevStruct gGpt2;
static gptDevStruct gGpt3;

static struct resource *aIOMemResource = NULL;

irqreturn_t ISR_GPT2_thread(int irq, void *dev_id)
    {
    gptDevStruct *gGpt = (gptDevStruct*) dev_id;

    //debugprint("GPT 2 Thread");

    if (gGpt->stopWatch->function == start)
	gGpt->stopWatch->is_counting_enabled = true;
    if (gGpt->stopWatch->function == reset)
	{
	gGpt->stopWatch->is_counting_enabled = false;
	gGpt->stopWatch->counter_value = 0;
	}

    if (gGpt->stopWatch->is_counting_enabled)
	gGpt->stopWatch->counter_value++;
    if (gGpt->stopWatch->function != stop)
	gGpt->stopWatch->display_value = gGpt->stopWatch->counter_value;

    return IRQ_HANDLED;

    }

irqreturn_t ISR_GPT_handler(int irq, void *dev_id)
    {
    gptDevStruct *gGpt = (gptDevStruct*) dev_id;

    //debugprint("GPT  ISR");

    writel(GPT_TSTAT_COMP, &gGpt->gptRegisters->tstat);

    return IRQ_WAKE_THREAD;

    }

irqreturn_t ISR_GPT3_thread(int irq, void *dev_id)
    {
    gptDevStruct *gGpt = (gptDevStruct*) dev_id;
    //debugprint("GPT 3 Thread");

    display_seg7(gGpt->stopWatch->display_value, gGpt->stopWatch->is_left);

    //debugprint("Gptt left = %d", gGpt->stopWatch->is_left);
    gGpt->stopWatch->is_left = (bool) !gGpt->stopWatch->is_left;

    return IRQ_HANDLED;
    }

void mGpt_EnableGpts(void)
    {
    uint32_t aTemp = readl(&gGpt2.gptRegisters->tctl);
    writel(aTemp | GPT_TCTL_TEN, &gGpt2.gptRegisters->tctl);

    aTemp = readl(&gGpt2.gptRegisters->tctl);
    writel(aTemp | GPT_TCTL_TEN, &gGpt3.gptRegisters->tctl);

    }

void mGpt_DisableGpts(void)
    {
    uint32_t aTemp = readl(&gGpt2.gptRegisters->tctl);
    writel(aTemp & ~(GPT_TCTL_TEN), &gGpt2.gptRegisters->tctl);

    aTemp = readl(&gGpt3.gptRegisters->tctl);
    writel(aTemp & ~(GPT_TCTL_TEN), &gGpt3.gptRegisters->tctl);

    }

/**
 * Base time = 1ms
 */
void mGpt_Open(uint32_t aPerdiodGpt1, uint32_t aPerdiodGpt2)
    {
    writel(aPerdiodGpt1, &gGpt2.gptRegisters->tcmp);
    writel(aPerdiodGpt2, &gGpt3.gptRegisters->tcmp);
    }

int mGpt_Setup(struct stopwatch* aStopWatchStruct)
    {

    debugprint("Gpt initialization");

    //Tell the kernel about the memory region used
    aIOMemResource = request_mem_region(GPT2_BASE_ADDR, GPT_REG_SIZE, "Gpt 2");

    if (aIOMemResource == NULL)
	{
	printk(KERN_ERR "request_mem_region ! Gpt 2 is not accessible");

	// Error happened
	return -1;
	}

    aIOMemResource = request_mem_region(GPT3_BASE_ADDR, GPT_REG_SIZE, "Gpt 3");

    if (aIOMemResource == NULL)
	{
	printk(KERN_ERR "request_mem_region ! Gpt 3 is not accessible");

	// Error happened
	return -1;
	}

    //Remap the physical memory to the right place
    gGpt2.gptRegisters = ioremap(GPT2_BASE_ADDR, 4096);
    gGpt3.gptRegisters = ioremap(GPT3_BASE_ADDR, 4096);

    // Test if the returned pointer is null
    if ((gGpt2.gptRegisters == NULL) || (gGpt3.gptRegisters == NULL))
	{
	printk(
		KERN_ERR "Ioremap failed ! Memory for GPT 2 or GPT 3 is not accessible");
	//free the memory locaiton
	release_mem_region(GPT2_BASE_ADDR, GPT_REG_SIZE);
	release_mem_region(GPT3_BASE_ADDR, GPT_REG_SIZE);

	// Error happened
	return -1;
	}

    debugprint("ioremap sucess. Virtual memory address : %08x and %08x",
	    (unsigned int ) gGpt2.gptRegisters,
	    (unsigned int ) gGpt3.gptRegisters);

    // GPT1 configuration
    writel(GPT_TCTL_SWR, &gGpt2.gptRegisters->tctl); /* force a software reset */
    while ((readl(&gGpt2.gptRegisters->tctl) & GPT_TCTL_SWR) != 0)
	;

    writel(GPT_TCTL_CLKSRC_32K | GPT_TCTL_COMP_EN, &gGpt2.gptRegisters->tctl);
    writel(32 - 1, &gGpt2.gptRegisters->tprer); /* --> 1ms clock */

    // GPT2 configuration
    writel(GPT_TCTL_SWR, &gGpt3.gptRegisters->tctl); /* force a software reset */
    while ((readl(&gGpt3.gptRegisters->tctl) & GPT_TCTL_SWR) != 0)
	;

    writel(GPT_TCTL_CLKSRC_32K | GPT_TCTL_COMP_EN, &gGpt3.gptRegisters->tctl);
    writel(32 - 1, &gGpt3.gptRegisters->tprer); /* --> 1ms clock */

    // Register the interrupts handlers
    if (0
	    != request_threaded_irq(MX27_INT_GPT2, ISR_GPT_handler,
		    ISR_GPT2_thread, 0, "gpt2_irq", &gGpt2))
	{
	printk(
	KERN_ERR "IRQ request for GPT2 failed !");

	//Umap the memory
	iounmap(gGpt2.gptRegisters);
	iounmap(gGpt3.gptRegisters);
	release_mem_region(GPT2_BASE_ADDR, GPT_REG_SIZE);
	release_mem_region(GPT3_BASE_ADDR, GPT_REG_SIZE);

	return -1;
	}

    if (0
	    != request_threaded_irq(MX27_INT_GPT3, ISR_GPT_handler,
		    ISR_GPT3_thread, 0, "gpt3_irq", &gGpt3))
	{
	printk(
	KERN_ERR "IRQ request for GPT3 failed !");

	//Release irq
	free_irq(MX27_INT_GPT2, &gGpt2);
	//Umap the memory
	iounmap(gGpt2.gptRegisters);
	iounmap(gGpt3.gptRegisters);
	release_mem_region(GPT2_BASE_ADDR, GPT_REG_SIZE);
	release_mem_region(GPT3_BASE_ADDR, GPT_REG_SIZE);
	return -1;
	}

    gGpt2.stopWatch = aStopWatchStruct;
    gGpt3.stopWatch = aStopWatchStruct;

    return 0;
    }

void mGpt_Release(void)

    {
    // Stop the timers
    mGpt_DisableGpts();

    //Release irq
    free_irq(MX27_INT_GPT2, &gGpt2);
    free_irq(MX27_INT_GPT3, &gGpt3);

    //Umap the memory
    iounmap(gGpt2.gptRegisters);
    iounmap(gGpt3.gptRegisters);
    release_mem_region(GPT2_BASE_ADDR, GPT_REG_SIZE);
    release_mem_region(GPT3_BASE_ADDR, GPT_REG_SIZE);

    }
