/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#if defined(CONFIG_S3C2400)
#include <s3c2400.h>
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#elif defined(CONFIG_S3C2440)
#include <s3c2440.h>
#endif
#include <asm/proc-armv/ptrace.h>

#include <asm/io.h>	//==hyq
#include "interrupts.h"


extern void USBIntHandler(void);	//==USBD中断处理函数
void (*isr_handle_array[32])(void);	//==中断处理函数指针


/*************2440清除中断标志***************/
void ClearPending(int bit) 
{
	struct s3c24x0_interrupt * const intregs = s3c24x0_get_base_interrupt();
	intregs->SRCPND = bit;
	intregs->INTPND = bit;
}


/*************2440中断处理函数***************/
void do_irq (struct pt_regs *pt_regs)
{
//	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
	struct s3c24x0_interrupt * const intregs = s3c24x0_get_base_interrupt();
	unsigned long oft = intregs->INTOFFSET;
#if 0    
	// clean int
	if( oft == 4 ) 
		gpio->EINTPEND = 1<<7;	//EINT4-7 in IRQ4 (EINTPEND[3:0] is reserve)
	intregs->SRCPND = 1<<oft;	
	intregs->INTPND	= intregs->INTPND;	 
#endif
	/* run the isr */
	isr_handle_array[oft]();
}


/*************2440默认中断处理函数***************/
void Dummy_isr(void)
{
	struct s3c24x0_interrupt * const intregs = s3c24x0_get_base_interrupt();
	printf("Dummy_isr error, interrupt number: %d, INTMSK = 0x%x\n", intregs->INTOFFSET, intregs->INTMSK);
	while(1);
}


/*************2440中断初始化***************/
int arch_interrupt_init (void)
{
	int i = 0;
	struct s3c24x0_interrupt * const intregs = s3c24x0_get_base_interrupt();
    
	for (i = 0; i < sizeof(isr_handle_array) / sizeof(isr_handle_array[0]); i++ )
	{
		isr_handle_array[i] = Dummy_isr;
	}

	intregs->INTMOD=0x0;	      // All=IRQ mode
	intregs->INTMSK=BIT_ALLMSK;	  // All interrupt is masked.

#ifdef CONFIG_USB_SLAVE
	isr_handle_array[ISR_USBD_OFT] = USBIntHandler;
	ClearPending(BIT_USBD);
#endif    
    return 0;
}

