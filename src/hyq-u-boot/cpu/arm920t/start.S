/*
 *  armboot - Startup Code for ARM920 CPU-core
 *
 *  Copyright (c) 2001	Marius Gr鰃er <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Z黳ke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <garyj@denx.de>
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
#include <config.h>

/*
 *************************************************************************
 *
 * 异常向量表
 * Jump vector table as in table 3.1 in [1]
 *
 *************************************************************************
 */
.globl _start
_start:	b	start_code
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

	.balignl 16,0xdeadbeef


/*
 *************************************************************************
 *
 * Startup Code (called from the ARM reset exception vector)
 *
 * do important init only if we don't start from memory!
 * relocate armboot to ram
 * setup stack
 * jump to second stage
 *
 *************************************************************************
 */
_TEXT_BASE:
	.word	TEXT_BASE

.globl  _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl  _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl  IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl  FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif



/*
 *************************************************************************
 *
 * the actual start code
 * 复位
 *
 *************************************************************************
 */
start_code:
	/*********** set the cpu to SVC32 mode***********/
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0


/*
 *************************************************************************
 *
 * turn off the watchdog
 * 关看门狗
 *
 *************************************************************************
 */
#define pWTCON	0x53000000

	ldr	r0, =pWTCON
	mov	r1, #0x0
	str	r1, [r0]


/*
 *************************************************************************
 *
 * mask all IRQs by setting all bits in the INTMR - default
 * 关中断和子中断
 *
 *************************************************************************
 */
#define INTMSK		0x4A000008	/* Interupt-Controller base addresses */
#define INTSUBMSK	0x4A00001C

	mov	r1, #0xffffffff
	ldr	r0, =INTMSK
	str	r1, [r0]

	ldr	r1, =0x7fff
	ldr	r0, =INTSUBMSK
	str	r1, [r0]


/*
 *************************************************************************
 *
 * 设置时钟频率
 * FCLK:HCLK:PCLK = 1:4:8
 *
 *************************************************************************
 */
#define CLKDIVN	 	0x4C000014	/* clock divisor register */
#define MPLLCON   	0x4C000004   
#define UPLLCON   	0x4C000008 
#define CAMDIVN	  	0x4C000018

	ldr	r0, =CLKDIVN          //FCLK:HCLK:PCLK = 1:4:8
	mov  r1, #5
	str	r1, [r0]

	mrc	p15, 0, r1, c1, c0, 0	
	orr	r1, r1, #0xc0000000		
	mcr	p15, 0, r1, c1, c0, 0

	ldr	r0, =CAMDIVN          
	mov  r1, #0
	str	r1, [r0]

	ldr	r0, =MPLLCON		
	ldr	r1, =0x7F021		 //405MHz  
	str	r1, [r0]

	ldr	r0, =UPLLCON		  
	ldr	r1, =0x38022		//48MHz 
	str	r1, [r0]



/*
 *************************************************************************
 *
 * Set up the stack
 * 设置堆栈
 *
 *************************************************************************
 */
stack_setup:
	ldr	r0, _TEXT_BASE		/* upper 512 KiB: relocated uboot   */
	sub	r0, r0, #CONFIG_SYS_MALLOC_LEN	/* malloc area              */
	sub	r0, r0, #CONFIG_SYS_GBL_DATA_SIZE /* bdinfo                 */
#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */



/*
 *************************************************************************
 *
 * 初始化串口
 *
 *************************************************************************
 */
#ifdef CONFIG_DEBUG_LL
	bl	InitUART	
#endif


/*
 *************************************************************************
 *
 * 初始化LED
 *
 *************************************************************************
 */
#if defined(CONFIG_HYQ2440_LED)
#define GPBCON 0x56000010
#define GPBDAT 0x56000014
#define GPBUP  0x56000018 

	ldr	r0, =GPBUP
	ldr	r1, =0x7FF    //即：二进制11111111111，关闭PB口上拉
	str	r1, [r0]

	ldr	r0, =GPBCON   //配置PB5、6、7、8为输出口，对应PBCON寄存器的第10-17位
	ldr	r1, =0x154FD  //即：二进制01 0101 0100 1111 1101
	str	r1, [r0]
#endif


/*
 *************************************************************************
 *
 * 检测启动位置,准备复制代码到RAM
 *
 *************************************************************************
 */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp	r0, r1			/* don't reloc during debug         */
	beq	boot_from_ram1			/*从33f80000的内存位置启动*/
	cmp	r0,#0x30000000
	bge	boot_from_ram2			/*从非33f80000的内存位置启动>=0x30000000*/

/*
 *************************************************************************
 *
 * 非内存启动，所以在这里初始化内存
 * we do sys-critical inits only at reboot, not when booting from ram!
 * 初始化内存 关MMU和cache
 *
 *************************************************************************
 */
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	bl	cpu_init_crit
#endif

/***************** CHECK_BOOT_FLASH *****************************/
	ldr	r1, =( (4<<28)|(3<<4)|(3<<2) )		/* address of Internal SRAM  0x4000003C*/
	mov	r0, #0		/* r0 = 0 */
	str	r0, [r1]

	mov	r1, #0x3c		/* address of men  0x0000003C*/
	ldr	r0, [r1]
	cmp	r0, #0
	bne	boot_from_nor

	/* recovery  */
	ldr	r0, =(0xdeadbeef)
	ldr	r1, =( (4<<28)|(3<<4)|(3<<2) )
	str	r0, [r1]

/********************boot from nand flash*******************************/
#define LENGTH_UBOOT 0x40000
#define NAND_REG_BASE 0x4E000000

	ldr	r0, =TEXT_BASE
	mov	r1, #0x0
	ldr	r2, =LENGTH_UBOOT
	bl	nand_read_ll
	tst	r0, #0x0
	beq	ok_nand_read

bad_nand_read:
loop1:
	b	loop1

ok_nand_read:
	mov	r0, #0
	ldr	r1, =TEXT_BASE
	ldr	r2, [r0], #4
	ldr	r3, [r1], #4
	teq	r2, r3
	bne	notmatch

	ldr	r0, =GPBDAT
	ldr	r1, =0xe0    //即：二进制0 1110 0000，点灯LED4
	str	r1, [r0] 

#ifdef CONFIG_DEBUG_LL
	ldr	r1, SerBase
	ldr	r5, =STR_NAND
	bl	PrintString
#endif

	b	clear_bss

notmatch:
loop2:
	b	loop2


/*
 *************************************************************************
 *
 * 从Nor启动，点灯LED3，打印调试信息
 *
 *************************************************************************
 */
boot_from_nor:
	ldr	r0, =GPBDAT
	ldr	r1, =0x160   
	str	r1, [r0] 
#ifdef CONFIG_DEBUG_LL
	ldr	r1, SerBase
	ldr	r5, =STR_NOR
	bl	PrintString
#endif
	b	relocate


/*
 *************************************************************************
 *
 * 从非0x33f80000内存启动，点灯LED2，打印调试信息
 *
 *************************************************************************
 */
boot_from_ram2:
	ldr	r0, =GPBDAT
	ldr	r1, =0x1a0  
	str	r1, [r0] 
#ifdef CONFIG_DEBUG_LL
	ldr	r1, SerBase
	ldr	r5, =STR_RAM2
	bl	PrintString
#endif
	b	relocate


/*
 *************************************************************************
 *
 * 从0x33f80000启动，点灯LED1，打印调试信息
 *
 *************************************************************************
 */
boot_from_ram1:
	ldr	r0, =GPBDAT
	ldr	r1, =0x1C0 
	str	r1, [r0]
#ifdef CONFIG_DEBUG_LL
	ldr	r1, SerBase
	ldr	r5, =STR_RAM1
	bl	PrintString
#endif
	b	clear_bss
//====hyq end


/*
 *************************************************************************
 *
 * boot from nor flash or ram but not 0x33f80000
 *
 *************************************************************************
 */
relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
					/* don't reloc during debug         */
	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot            */
	add	r2, r0, r2		/* r2 <- source end address         */

copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop


/*
 *************************************************************************
 *
 * BBS清零
 *
 *************************************************************************
 */
clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov	r2, #0x00000000		/* clear                            */

clbss_l:
	str	r2, [r0]		/* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l

#ifdef CONFIG_DEBUG_LL
	ldr	r1, SerBase
	ldr	r5, =STR_MAIN
	bl	PrintString
#endif

	ldr	pc, _start_armboot	//==跳入C代码

_start_armboot:	.word start_armboot



/*--------------------子函数开始----------------------------/
/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
cpu_init_crit:
	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300	@ clear bits 13, 9:8 (--V- --RS)
	bic	r0, r0, #0x00000087	@ clear bits 7, 2:0 (B--- -CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (A) Align
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-Cache
	mcr	p15, 0, r0, c1, c0, 0

	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a lowlevel_init.S in your board directory.
	 */
	mov	ip, lr

	bl	lowlevel_init

	mov	lr, ip
	mov	pc, lr
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */


#ifdef CONFIG_DEBUG_LL
/*
 *************************************************************************
 * 初始化串口函数
 *************************************************************************
 */
#define GPHCON		0x56000070
#define GPHUP		0x56000078

#define UART_CTL_BASE		0x50000000
#define UART0_CTL_BASE		UART_CTL_BASE

/* Offset */
#define oULCON			0x00		/* R/W, UART line control register */
#define oUCON			0x04	 	/* R/W, UART control register */
#define oUFCON			0x08		/* R/W, UART FIFO control register */
#define oUMCON			0x0C		/* R/W, UART modem control register */
#define oUTRSTAT		0x10	 	/* R  , UART Tx/Rx status register */
#define oUTXHL			0x20	 	/*   W, UART transmit(little-end) buffer */
#define oUTXHB			0x23	 	/*   W, UART transmit(big-end) buffer */

#define oUBRDIV			0x28	/* R/W, Baud rate divisor register */

#define UTRSTAT_TX_EMPTY	(1 << 2)


InitUART:
	ldr	r0, =GPHCON
	ldr	r1, =0x0016faaa
	str	r1, [r0]
	ldr	r0, =GPHUP
	ldr	r1, =0x000007ff
	str	r1, [r0]

	ldr	r1, SerBase
	mov	r2, #0x0
	str	r2, [r1, #oUFCON]
	mov	r2, #0x0
	str	r2, [r1, #oUMCON]
	mov	r2, #0x3
	str	r2, [r1, #oULCON]
	ldr	r2, =0x245
	str	r2, [r1, #oUCON]

#define PCLK	50000000	//50M
#define UART_CLK   PCLK
#define UART_BAUD_RATE 115200
#define UART_BRD  ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

	mov	r2, #UART_BRD
	str	r2, [r1, #oUBRDIV]

	mov	r3, #100
	mov	r2, #0x0
1:	sub	r3, r3, #0x1
	tst	r2, r3
	bne	1b

	mov	pc, lr
#endif


/*
 *************************************************************************
 *
 * 中断操作函数
 * Interrupt handling
 *
 *************************************************************************
 */

@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC	0x13
#define I_BIT		0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	ldr	r2, _armboot_start
	sub	r2, r2, #(CONFIG_STACKSIZE)
	sub	r2, r2, #(CONFIG_SYS_MALLOC_LEN)
	/* set base 2 words into abort stack */
	sub	r2, r2, #(CONFIG_SYS_GBL_DATA_SIZE+8)
	ldmia	r2, {r2 - r3}			@ get pc, cpsr
	add	r0, sp, #S_FRAME_SIZE		@ restore sp_SVC

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r7, sp, #S_PC
	stmdb	r7, {sp, lr}^			@ Calling SP, LR
	str	lr, [r7, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r7, #4]			@ Save CPSR
	str	r0, [r7, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	/* return & move spsr_svc into cpsr */
	subs	pc, lr, #4
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack
	sub	r13, r13, #(CONFIG_STACKSIZE)
	sub	r13, r13, #(CONFIG_SYS_MALLOC_LEN)
	/* reserve a couple spots in abort stack */
	sub	r13, r13, #(CONFIG_SYS_GBL_DATA_SIZE+8)

	str	lr, [r13]			@ save caller lr / spsr
	mrs	lr, spsr
	str	lr, [r13, #4]

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13
	mov	lr, pc
	movs	pc, lr
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm


/*
 *************************************************************************
 *
 * 异常操作
 * exception handlers
 *
 *************************************************************************
 */
	.align  5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

#ifdef CONFIG_USE_IRQ
	.align	5
irq:

/* use IRQ for USB and DMA */
	sub    lr, lr, #4                     @ the return address
	ldr    sp, IRQ_STACK_START            @ the stack for irq
	stmdb    sp!,     { r0-r12,lr }     @ save registers
    
	ldr    lr,    =int_return             @ set the return addr

	ldr    pc, =do_irq                @ call the isr
	
int_return:
	ldmia    sp!,     { r0-r12,pc }^     @ return from interrupt

	.align	5
fiq:
	get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	irq_save_user_regs
	bl	do_fiq
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq

#endif




/*
 *************************************************************************
 *
 * 汇编级串口打印
 *
 *************************************************************************
 */
#ifdef CONFIG_DEBUG_LL

/*************打印16进制表，R0包含值，R1串口基地址**********/
PrintHexNibble:
	adr	r2, HEX_TO_ASCII_TABLE
	and	r0, r0, #0xF
	ldr	r0, [r2, r0]	@ convert to ascii
	b	PrintChar

/*************打印字符,R0保存字符，R1保存串口基地址**********/
PrintChar:
TXBusy:
	ldr	r2, [r1, #oUTRSTAT]
	and	r2, r2, #UTRSTAT_TX_EMPTY
	tst	r2, #UTRSTAT_TX_EMPTY
	beq	TXBusy	
	str	r0, [r1, #oUTXHL]
	mov	pc, lr

/*
 *************************************************************************
 *
 * PrintChar使用：R0字符值，R1串口寄存器地址，R2
 * R3是R0的备份，R4子函数返回地址，R5字符串地址
 *
 *************************************************************************
 */
PrintString:
	mov r4, lr
LoopStr:
	ldr	r0, [r5]
	ldr	r3, [r5]
	and	r0, r0, #0xff
	cmp	r0, #0				/*ascii值为0，字符串结束*/
	moveq	pc, r4			
	bl	PrintChar			

	mov	r0, r3, LSR #8		/* shift word right 8 bits */
	and	r0, r0, #0xff
	cmp	r0, #0
	moveq	pc, r4
	bl	PrintChar

	mov	r0, r3, LSR #16		/* shift word right 16 bits */
	and	r0, r0, #0xff
	cmp	r0, #0
	moveq	pc, r4
	bl	PrintChar
	
	mov	r0, r3, LSR #24		/* shift word right 24 bits */
	and	r0, r0, #0xff
	cmp	r0, #0
	moveq	pc, r4
	bl	PrintChar

	add	r5, r5, #4
	b LoopStr


/*************打印一个16进制字，R0保存字，R1保存串口基地址**********/
PrintHexWord:
	mov	r4, lr
	mov	r3, r0
	mov	r0, r3, LSR #28
	bl	PrintHexNibble
	mov	r0, r3, LSR #24
	bl	PrintHexNibble
	mov	r0, r3, LSR #20
	bl	PrintHexNibble
	mov	r0, r3, LSR #16
	bl	PrintHexNibble
	mov	r0, r3, LSR #12
	bl	PrintHexNibble
	mov	r0, r3, LSR #8
	bl	PrintHexNibble
	mov	r0, r3, LSR #4
	bl	PrintHexNibble
	mov	r0, r3
	bl	PrintHexNibble

	mov	r0, #'\r'
	bl	PrintChar

	mov	r0, #'\n'
	bl	PrintChar

	mov	pc, r4

#endif





/****************数据域******************/

#ifdef CONFIG_DEBUG_LL

HEX_TO_ASCII_TABLE:
	.ascii	"0123456789ABCDEF"

STR_UNDEF:
	.ascii	"\rUNDF\n\0\0"
STR_SWI:
	.ascii	"\rSWI\n\0\0\0"
STR_PREFETCH_ABORT:
	.ascii	"\rPABT\n\0\0"
STR_DATA_ABORT:
	.ascii	"\rDABT\n\0\0"
STR_IRQ:
	.ascii	"\rIRQ\n\0\0\0"
STR_FIQ:
	.ascii	"\rFIQ\n\0\0\0"
STR_NOT_USED:
	.ascii	"\rNUSD\n\0\0"

STR_MAIN:
	.ascii	 "\rstart_to_main\n\0"
STR_NAND:
	.ascii	 "\rboot_from_nand\n\0\0\0\0"
STR_NOR:
	.ascii	 "\rboot_from_nor\n\0"
STR_RAM1:
	.ascii	 "\rboot_from_ram1\n\0\0\0\0"
STR_RAM2:
	.ascii	 "\rboot_from_ram2\n\0\0\0\0"

.align 4
SerBase:
	.long UART0_CTL_BASE
#endif
