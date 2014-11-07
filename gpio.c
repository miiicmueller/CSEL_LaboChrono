/**
 * EIA-FR - Microprocessor 1 laboratory
 *
 * Abstract: i.MX27 GPIO
 *
 * Author: 	Daniel Gachet
 * Date: 	16.05.2011
 */

#include "gpio.h"
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

/* -- Internal types and constant definition -------------------------------- */

/**
 * GPIO Controller Register Definition (port A to F)
 */
struct gpio_ctrl
    {
	uint32_t ddir;
	uint32_t ocr[2];
	uint32_t iconfa[2];
	uint32_t iconfb[2];
	uint32_t dr;
	uint32_t gius;
	uint32_t ssr;
	uint32_t icr[2];
	uint32_t imr;
	uint32_t isr;
	uint32_t gpr;
	uint32_t swr;
	uint32_t puen;
    };
static struct gpio_ctrl* gpio_phys[] =
    {
	    (struct gpio_ctrl*) 0x10015000,
	    (struct gpio_ctrl*) 0x10015100,
	    (struct gpio_ctrl*) 0x10015200,
	    (struct gpio_ctrl*) 0x10015300,
	    (struct gpio_ctrl*) 0x10015400,
	    (struct gpio_ctrl*) 0x10015500
    };
static struct gpio_ctrl* gpio[GPIO_NB_OF_PORTS];

/**
 * GPIO ISR Handler Structure Definition
 */
struct gpio_isr_handlers
    {
	gpio_isr_t routine; /* application specific interrupt routine */
	void* handle; /* application specific parameter */
	enum gpio_modes mode; /* pin operation mode */
	char name[32];
    };
static struct gpio_isr_handlers handlers[GPIO_NB_OF_PORTS][32];

/* -- Internal methods definition ------------------------------------------- */

static void gpio_isr(void* handle __attribute__ ((unused)))
    {
    enum gpio_ports port;
    int pin;
    for (port = 0; port < GPIO_NB_OF_PORTS; port++)
	{
	struct gpio_ctrl* gpioc = gpio[port];
	uint32_t isr = gpioc->isr & gpioc->imr; /* compute interrupt sources */
	gpioc->isr = isr; /* acknowledged all pending sources */

	for (pin = 0; pin < 32; pin++)
	    {
	    if ((isr & (1 << pin)) != 0)
		{
		if (handlers[port][pin].routine != 0)
		    {
		    handlers[port][pin].routine(handlers[port][pin].handle);
		    }
		}
	    }
	}
    }

/* -- Public methods definition --------------------------------------------- */

int gpio_init()
    {
    for (int i = 0; i < GPIO_NB_OF_PORTS; i++)
	{
	gpio[i] = ioremap(gpio_phys[i], sizeof(struct gpio_ctrl));
	}

    return 0;
    }

/* ------------------------------------------------------------------------- */

void gpio_cleanup()
    {
    for (int i = 0; i < GPIO_NB_OF_PORTS; i++)
	{
	iounmap(gpio[i]);
	}
    }

/* ------------------------------------------------------------------------- */

int gpio_configure(enum gpio_ports port, uint32_t bitmask, enum gpio_modes mode)
    {
    int status = -1;
    int i;

    if (port < GPIO_NB_OF_PORTS)
	{
	struct gpio_ctrl* gpioc = gpio[port];
	switch (mode)
	    {
	case GPIO_OUTPUT:
	    gpioc->ddir |= bitmask;
	    for (i = 0; i < 32; i++)
		{
		if ((bitmask & 1 << i) != 0)
		    {
		    gpioc->ocr[i / 16] |= 3 << ((i % 16) << 1);
		    }
		}
	    gpioc->gius |= bitmask;
	    status = 0;
	    break;

	case GPIO_INPUT:
	    gpioc->ddir &= ~bitmask;
	    for (i = 0; i < 32; i++)
		if ((bitmask & 1 << i) != 0)
		    {
		    gpioc->iconfa[i / 16] &= ~(0x3 << ((i % 16) << 1));
		    gpioc->iconfb[i / 16] &= ~(0x3 << ((i % 16) << 1));
		    }
	    gpioc->gius |= bitmask;
	    status = 0;
	    break;
	    }
	}

    return status;
    }

/* ------------------------------------------------------------------------- */

uint32_t gpio_getbits(enum gpio_ports port)
    {
    if (port < GPIO_NB_OF_PORTS)
	{
	return gpio[port]->ssr & ~(gpio[port]->ddir);
	}
    return 0;
    }

/* ------------------------------------------------------------------------- */

void gpio_setbits(enum gpio_ports port, uint32_t bitmask)
    {
    if (port < GPIO_NB_OF_PORTS)
	{
	gpio[port]->dr |= bitmask;
	}
    }

/* ------------------------------------------------------------------------- */

void gpio_resetbits(enum gpio_ports port, uint32_t bitmask)
    {
    if (port < GPIO_NB_OF_PORTS)
	{
	gpio[port]->dr &= ~(bitmask);
	}
    }

/* ------------------------------------------------------------------------- */

int gpio_attach(enum gpio_ports port, uint32_t pin,
	enum gpio_interrupt_modes mode, gpio_isr_t routine, void* handle)
    {
    int status = -1;
    struct gpio_ctrl* gpioc = 0;

    if ((port <= GPIO_NB_OF_PORTS) && (pin < 32)
	    && (handlers[port][pin].routine == 0))
	{
	handlers[port][pin].routine = routine;
	handlers[port][pin].handle = handle;
	snprintf(handlers[port][pin].name, 32, "%d", (port * 32 + pin));

	request_irq(gpio_to_irq(port * 32 + pin), handlers[port][pin].routine,
		0, handlers[port][pin].name, handlers[port][pin].handle);

	gpioc = gpio[port];
	gpioc->imr &= ~(1 << pin);
	gpioc->isr |= (1 << pin);
	gpioc->ddir &= ~(1 << pin);

	gpioc->iconfa[pin / 16] &= ~(0x3 << ((pin % 16) << 1));
	gpioc->iconfa[pin / 16] |= (0x1 << ((pin % 16) << 1));
	gpioc->iconfb[pin / 16] &= ~(0x3 << ((pin % 16) << 1));
	gpioc->iconfb[pin / 16] |= (0x1 << ((pin % 16) << 1));

	gpioc->icr[pin / 16] &= ~(0x3 << ((pin % 16) << 1));
	gpioc->icr[pin / 16] |= ((mode - GPIO_IRQ_RISING) << ((pin % 16) << 1));

	gpioc->gius |= (1 << pin);

	status = 0;
	}

    return status;
    }

/* ------------------------------------------------------------------------- */

void gpio_detach(enum gpio_ports port, uint32_t pin)
    {
    if ((port < GPIO_NB_OF_PORTS) && (pin < 32))
	{
	struct gpio_ctrl* gpioc = gpio[port];
	gpioc->imr &= ~(1 << pin);
	gpioc->ddir &= ~(1 << pin);

	gpioc->iconfa[pin / 16] &= ~(0x3 << ((pin % 16) << 1));
	gpioc->iconfb[pin / 16] &= ~(0x3 << ((pin % 16) << 1));

	free_irq(gpio_to_irq(port * 32 + pin), handlers[port][pin].handle);

	handlers[port][pin].routine = 0;
	handlers[port][pin].handle = 0;
	*handlers[port][pin].name = 0;
	}
    }

/* ------------------------------------------------------------------------- */

void gpio_enable(enum gpio_ports port, uint32_t pin)
    {
    if ((port < GPIO_NB_OF_PORTS) && (pin < 32))
	{
	gpio[port]->imr |= 1 << pin;
	}
    }

/* ------------------------------------------------------------------------- */

void gpio_disable(enum gpio_ports port, uint32_t pin)
    {
    if ((port < GPIO_NB_OF_PORTS) && (pin < 32))
	{
	gpio[port]->imr &= ~(1 << pin);
	gpio[port]->isr |= (1 << pin);
	}
    }

