/*
 * char_skel.h
 *
 *  Created on: Oct 17, 2014
 *      Author: lmi
 */

#ifndef CHRONO_H_
#define CHRONO_H_


#define TRACE_DEBUG

//Debugging switch
#ifdef TRACE_DEBUG
#define debugprint(fmt, args...) printk(KERN_DEBUG   "[Chrono DBG] " fmt "\r\n", ##args)
#else
#define debugprint(fmt, args...)
#endif


enum functions {stop, start, reset};
struct stopwatch
{
	enum functions function;

	uint32_t counter_value;
	bool is_counting_enabled;

	uint32_t display_value;
	bool is_left;
};

#endif /* CHRONO_H_ */
