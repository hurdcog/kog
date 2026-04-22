/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
  Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef	_I386_PIT_H_
#define	_I386_PIT_H_

#if	defined(AT386) || defined(ATX86_64)
/* 
 * Definitions for 8254 Programmable Interrupt Timer ports on AT 386
 * The 8254 contains three independent 16-bit counters that can be programmed
 * to operate in various modes for timing, counting, and waveform generation.
 */
#define PITCTR0_PORT	0x40		/* counter 0 port - system timer */
#define PITCTR1_PORT	0x41		/* counter 1 port - memory refresh */
#define PITCTR2_PORT	0x42		/* counter 2 port - speaker tone */
#define PITCTL_PORT	0x43		/* PIT control port - mode/command register */
#define PITAUX_PORT	0x61		/* PIT auxiliary port - speaker control */

/* Bit values used in auxiliary control port for timer 2 (speaker) */
#define PITAUX_GATE2	0x01		/* aux port, PIT gate 2 input enable */
#define PITAUX_OUT2	0x02		/* aux port, PIT clock out 2 enable */
#define PITAUX_VAL	0x20		/* aux port, speaker output enable */
#endif	/* defined(AT386) */

/* PIT counter selection and control bit masks */
#define PIT_C0          0x00            /* select counter 0 (system timer) */
#define PIT_LOADMODE	0x30		/* load least significant byte followed
					 * by most significant byte */
#define PIT_NDIVMODE	0x04		/* divide by N counter mode */

/* PIT operating modes for Timer 1 (used for delay calculations in countdown mode) */
#define PIT_C1          0x40            /* select counter 1 (memory refresh) */
#define PIT_READMODE	0x30		/* read or load least significant byte
					 * followed by most significant byte */

/* PIT timer operating modes */
#define PIT_SQUAREMODE	0x06		/* square-wave mode (mode 3) */
#define PIT_RATEMODE	0x04		/* rate generator mode (mode 2) */
#define PIT_ONESHOTMODE	0x02		/* one-shot mode (mode 1) */

/* Timer 2 selection (used for speaker control) */
#define PIT_C2		0x80            /* select counter 2 (speaker) */

#define POST_PORT	0x80		/* diagnostic port for tiny i/o delay */

/*
 * PIT frequency constants
 * Clock speed for the timer in Hz. The 8254 PIT runs at a fixed frequency
 * of 1.193182 MHz on PC-compatible hardware.
 */
#if	defined(AT386) || defined(ATX86_64)
#define CLKNUM		1193182		/* PIT base frequency in Hz */
#endif	/* AT386 */

extern void clkstart(void);
extern void pit_prepare_sleep(int hz);
extern void pit_sleep(void);
extern void pit_udelay(int usec);
extern void pit_mdelay(int msec);

#endif /* _I386_PIT_H_ */
