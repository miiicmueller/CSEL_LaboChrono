#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include "chronometre.h"

//For eclipse developpement
#define ECLIPSE_IGNORE

#ifdef ECLIPSE_IGNORE
#define __init
#define __exit

#endif

/**
 * Driver input function
 */
static int __init mlaboChrono_init(void)
    {
    return 0;
    }

/**
 * Driver exit function
 */
static int __exit mlaboChrono_exit(void)
    {

    return 0;
    }

module_init(mlaboChrono_init);
module_exit(mlaboChrono_exit);

MODULE_AUTHOR("Cyrille Savy/Michael Mueller");
MODULE_LICENSE("GPL");

