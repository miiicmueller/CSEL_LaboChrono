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
#include "chronometre.h"
#include "gpt.h"
#include "display.h"

//For eclipse developpement
#define ECLIPSE_IGNORE

#ifdef ECLIPSE_IGNORE
#define __init
#define __exit

#endif

static struct stopwatch stopwatch;

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

//    int_init();
//    aitc_init();
//    gpio_init();

    mGpt_Setup(&stopwatch);

    display_init();

//
//    gpio_attach(GPIO_PORT_E, 3, GPIO_IRQ_FALLING, gpio_start, &stopwatch);
//    gpio_enable(GPIO_PORT_E, 3);
//    gpio_attach(GPIO_PORT_E, 4, GPIO_IRQ_FALLING, gpio_stop, &stopwatch);
//    gpio_enable(GPIO_PORT_E, 4);
//    gpio_attach(GPIO_PORT_E, 6, GPIO_IRQ_FALLING, gpio_reset, &stopwatch);
//    gpio_enable(GPIO_PORT_E, 6);

    mGpt_Open(100, 10);
    mGpt_EnableGpts();

    debugprint("Real application running...");
//    int_enable();

    return 0;
    }

/**
 * Driver exit function
 */
static int __exit mlaboChrono_exit(void)
    {

    mGpt_Release();
    return 0;
    }

module_init(mlaboChrono_init);
module_exit(mlaboChrono_exit);

MODULE_AUTHOR("Savy/Mueller");
MODULE_LICENSE("GPL");

