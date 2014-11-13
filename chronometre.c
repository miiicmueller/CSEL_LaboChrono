#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/stddef.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include "chronometre.h"
#include "gpt.h"
#include "display.h"
#include "gpio.h"

//For eclipse developpement
#define ECLIPSE_IGNORE

#ifdef ECLIPSE_IGNORE
#define __init
#define __exit

#endif

static struct stopwatch stopwatch;
static gptDevStruct gGpt2;
static gptDevStruct gGpt3;

static irqreturn_t gpio_start(int irq, void* handle)
    {
    struct stopwatch* oref = (struct stopwatch*) handle;
    oref->function = start;
    return IRQ_HANDLED;
    }
irqreturn_t gpio_stop(int irq, void* handle)
    {
    struct stopwatch* oref = (struct stopwatch*) handle;
    oref->function = stop;
    return IRQ_HANDLED;
    }
irqreturn_t gpio_reset(int irq, void* handle)
    {
    struct stopwatch* oref = (struct stopwatch*) handle;
    oref->function = reset;
    return IRQ_HANDLED;
    }

irqreturn_t ISR_GPT2_thread(int irq, void *dev_id)
    {
    gptDevStruct *gGpt = (gptDevStruct*) dev_id;
    debugprint("GPT 2 Thread");
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

irqreturn_t ISR_GPT3_thread(int irq, void *dev_id)
    {
    gptDevStruct *gGpt = (gptDevStruct*) dev_id;
    debugprint("GPT 3 Thread");
    display_seg7(gGpt->stopWatch->display_value, gGpt->stopWatch->is_left);
    debugprint("Gptt left = %d", gGpt->stopWatch->is_left);
    gGpt->stopWatch->is_left = (bool) !gGpt->stopWatch->is_left;
    return IRQ_HANDLED;
    }

/**
 * Driver input function
 */
static int __init mlaboChrono_init(void)
    {

    debugprint("\n");
    debugprint("EIA-FR Chronometer on APF27\n");
    debugprint("---------------------------\n");

    /* real application... */
    stopwatch.function = stop;
    stopwatch.is_counting_enabled = false;
    stopwatch.counter_value = 0;
    stopwatch.display_value = 0;
    stopwatch.is_left = false;

    mGpt_Setup(&gGpt2, kGpt2, &stopwatch, ISR_GPT2_thread);
    mGpt_Setup(&gGpt3, kGpt3, &stopwatch, ISR_GPT3_thread);

    display_init();

    mGpt_Open(&gGpt2, 100);
    mGpt_Open(&gGpt3, 10);

    gpio_attach(GPIO_PORT_E, 3, GPIO_IRQ_FALLING, gpio_start, &stopwatch);
    gpio_enable(GPIO_PORT_E, 3);
    gpio_attach(GPIO_PORT_E, 4, GPIO_IRQ_FALLING, gpio_stop, &stopwatch);
    gpio_enable(GPIO_PORT_E, 4);
    gpio_attach(GPIO_PORT_E, 6, GPIO_IRQ_FALLING, gpio_reset, &stopwatch);
    gpio_enable(GPIO_PORT_E, 6);

    mGpt_Open(100, 10);
    mGpt_EnableGpts(&gGpt2);
    mGpt_EnableGpts(&gGpt3);

    debugprint("Real application running...");

    return 0;
    }

/**
 * Driver exit function
 */
static int __exit mlaboChrono_exit(void)
    {
    gpio_cleanup();

    mGpt_Release(&gGpt2);
    mGpt_Release(&gGpt3);

    return 0;
    }

module_init(mlaboChrono_init);
module_exit(mlaboChrono_exit);

MODULE_AUTHOR("Savy/Mueller");
MODULE_LICENSE("GPL");

