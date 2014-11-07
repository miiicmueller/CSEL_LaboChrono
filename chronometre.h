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
#define debugprint(fmt, args...) printk(KERN_DEBUG   "[Char skel DBG] " fmt "\r\n", ##args)
#else
#define debugprint(fmt, args...)
#endif




#endif /* CHRONO_H_ */
