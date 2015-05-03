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
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
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
#if defined(CONFIG_S3C2400) || \
    defined(CONFIG_S3C2410) || \
	defined(CONFIG_S3C2440) || \
    defined(CONFIG_TRAB)

#include <asm/io.h>

#if defined(CONFIG_S3C2400)
#include <s3c2400.h>
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#elif defined(CONFIG_S3C2440)
#include <s3c2440.h>
#endif


int timer_load_val = 0;	    //==每次递减值，恒定不变15625
static ulong timer_clk;	    //==定时器4时钟频率50/(2*16)
static ulong timestamp;	    //==时间戳，从初始化到当前流逝的总时间计数
static ulong lastdec;		//==上次get_timer_masked函数的时间计数


/* macro to read the 16 bit timer */
static inline ulong READ_TIMER(void)
{
	struct s3c24x0_timers *timers = s3c24x0_get_base_timers();

	return readl(&timers->TCNTO4) & 0xffff;
}


/********************************************************************
函数功能：定时器4初始化。
入口参数：无
返    回：无。
备    注：定时器4没有输出功能。
********************************************************************/
int timer_init(void)
{
	struct s3c24x0_timers *timers = s3c24x0_get_base_timers();
	ulong tmr;

	/* use PWM Timer 4 because it has no output */
	/* prescaler for Timer 4 is 16 */
	writel(0x0f00, &timers->TCFG0);
	if (timer_load_val == 0) {
		/*
		 * for 10 ms clock period @ PCLK with 4 bit divider = 1/2
		 * (default) and prescaler = 16. Should be 10390
		 * @33.25MHz and 15625 @ 50 MHz
		 */
		timer_load_val = get_PCLK() / (2 * 16 * 100);	//==定时器时钟/100；定时1/100秒，即10ms
		timer_clk = get_PCLK() / (2 * 16);
	}
	/* load value for 10 ms timeout */
	lastdec = timer_load_val;
	writel(timer_load_val, &timers->TCNTB4);
	/* auto load, manual update of Timer 4 */
	tmr = (readl(&timers->TCON) & ~0x0700000) | 0x0600000;
	writel(tmr, &timers->TCON);
	/* auto load, start Timer 4 */
	tmr = (tmr & ~0x0700000) | 0x0500000;
	writel(tmr, &timers->TCON);
	timestamp = 0;

	return (0);
}

/*
 * timer without interrupts
 */

void reset_timer(void)
{
	reset_timer_masked();
}

ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

void set_timer(ulong t)
{
	timestamp = t;
}


/********************************************************************
函数功能：微妙级延迟。
入口参数：
    usec：延迟几微妙
返    回：无。
备    注：默认延迟大于1000us。
********************************************************************/
void udelay(unsigned long usec)
{
	ulong tmo;
	ulong start = get_ticks();

	tmo = usec / 1000;		        //==定时多少ms
	tmo *= (timer_load_val * 100);  //==定时tmo毫秒对应的计数
	tmo /= 1000;

	while ((ulong) (get_ticks() - start) < tmo)
		/*NOP*/;
}


/********************************************************************
函数功能：复位全局时间。
入口参数：无
返    回：无。
备    注：无。
********************************************************************/
void reset_timer_masked(void)
{
	/* reset time */
	lastdec = READ_TIMER();
	timestamp = 0;
}


/********************************************************************
函数功能：获取时间。
入口参数：无
返    回：多少ms。
备    注：无。
********************************************************************/
ulong get_timer_masked(void)
{
	ulong tmr = get_ticks();		//==得到目前总的时间计数

	return tmr / (timer_clk / CONFIG_SYS_HZ);		//==返回总的时间ms
}


/********************************************************************
函数功能：微妙级延迟。
入口参数：
    usec：延迟几微妙
返    回：无。
备    注：无。
********************************************************************/
void udelay_masked(unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {                 //==延迟1ms以上
		tmo = usec / 1000;              //==等价多少ms
		tmo *= (timer_load_val * 100);  //==乘以1s计数
		tmo /= 1000;
	} else {
		tmo = usec * (timer_load_val * 100);
		tmo /= (1000 * 1000);
	}

	endtime = get_ticks() + tmo;

	do {
		ulong now = get_ticks();
		diff = endtime - now;
	} while (diff >= 0);
}


/********************************************************************
函数功能：获取时间计数值。
入口参数：无
返    回：总的时间计数。
备    注：无。
********************************************************************/
unsigned long long get_ticks(void)
{
	ulong now = READ_TIMER();

	if (lastdec >= now) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + timer_load_val - now;
	}
	lastdec = now;

	return timestamp;
}


/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	ulong tbclk;

#if defined(CONFIG_SMDK2400) || defined(CONFIG_TRAB) 
	tbclk = timer_load_val * 100;
#elif defined(CONFIG_SBC2410X) || \
      defined(CONFIG_SMDK2410) || \
	  defined(CONFIG_HYQ2440)  || \
      defined(CONFIG_VCMA9)
	tbclk = CONFIG_SYS_HZ;
#else
#	error "tbclk not configured"
#endif

	return tbclk;
}


/*
 * reset the cpu by setting up the watchdog timer and let him time out
 */
void reset_cpu(ulong ignored)
{
	struct s3c24x0_watchdog *watchdog;

#ifdef CONFIG_TRAB
	extern void disable_vfd(void);

	disable_vfd();
#endif

	watchdog = s3c24x0_get_base_watchdog();

	/* Disable watchdog */
	writel(0x0000, &watchdog->WTCON);

	/* Initialize watchdog timer count register */
	writel(0x0001, &watchdog->WTCNT);

	/* Enable watchdog timer; assert reset at timer timeout */
	writel(0x0021, &watchdog->WTCON);

	while (1)
		/* loop forever and wait for reset to happen */;

	/*NOTREACHED*/
}

#endif /* defined(CONFIG_S3C2400)  ||
	  defined (CONFIG_S3C2410) ||
	  defined (CONFIG_TRAB) */
