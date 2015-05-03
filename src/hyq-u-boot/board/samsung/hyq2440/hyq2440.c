/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
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
#include <netdev.h>
#include <s3c2440.h>	//==hyq
#include <video_fb.h>	//==hyq

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 2

#if FCLK_SPEED==0		/* Fout = 203MHz, Fin = 12MHz for Audio */
#define M_MDIV	0xC3
#define M_PDIV	0x4
#define M_SDIV	0x1
#elif FCLK_SPEED==1		/* Fout = 202.8MHz */
#define M_MDIV	0xA1
#define M_PDIV	0x3
#define M_SDIV	0x1
#elif FCLK_SPEED==2        /* Fout = 405MHz */
#define M_MDIV    0x7F     //这三个值根据S3C2440芯片手册“PLL VALUE SELECTION TABLE”部分进行设置
#define M_PDIV    0x2
#define M_SDIV    0x1
#endif

#define USB_CLOCK 2

#if USB_CLOCK==0
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif USB_CLOCK==1
#define U_M_MDIV	0x48
#define U_M_PDIV	0x3
#define U_M_SDIV	0x2
#elif USB_CLOCK==2         /* Fout = 48MHz */
#define U_M_MDIV    0x38   //这三个值根据S3C2440芯片手册“PLL VALUE SELECTION TABLE”部分进行设置
#define U_M_PDIV    0x2
#define U_M_SDIV    0x2
#endif

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	clk_power->LOCKTIME = 0xFFFFFF;

	/* configure MPLL */
	clk_power->MPLLCON = ((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV);

	/* some delay between MPLL and UPLL */
	delay (4000);

	/* configure UPLL */
	clk_power->UPLLCON = ((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV);

	/* some delay between MPLL and UPLL */
	delay (8000);

	/* set up the I/O ports */
	gpio->GPACON = 0x007FFFFF;

	gpio->GPBCON = 0x00295555;	//=10 1001  0110 1010 1010 1010; 296555
	gpio->GPBUP = 0x000007FF;

	gpio->GPCCON = 0xAAAAA6AA;	//==VD LCD 32bit
	gpio->GPCDAT &= ~(1<<5);
	gpio->GPCUP = 0x0000FFFF;

	gpio->GPDCON = 0xAAAAAAAA;	//==VD[8~23] 32bit
	gpio->GPDUP = 0x0000FFFF;

	gpio->GPECON = 0xAAAAAAAA;	//==IIC SPI SD IIS AC 32bit
	gpio->GPEUP = 0x00003FFF;

	gpio->GPFCON = 0x000055AA;	//==EINT[0~7]	16bit
	gpio->GPFUP = 0x000000FF;

	gpio->GPGCON = 0xFF95FFBA;	//==EINT[8~23] 32bit	SPI LCD_PWRDN
	gpio->GPGUP = 0x0000FFFF;

	gpio->GPHCON = 0x0016FAAA;	//==RXD[0~2] TXD[0~2] 22bit
	gpio->GPHUP = 0x000007FF;

#if defined (CONFIG_S3C2440)
	gpio->GPJCON = 0x02AAAAAA;	//==CAM 26bit
	gpio->GPJUP = 0x00001FFF;
#endif

	gpio->EXTINT0=0x22222222;
	gpio->EXTINT1=0x22222222;
	gpio->EXTINT2=0x22222222;

	/* arch number of SMDK2410-Board */
	gd->bd->bi_arch_number = MACH_TYPE_MINI2440;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}


//==hyq lcd
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_CLKVAL 7

#define LCD_HFPD 0x4
#define LCD_HBPD 0x44
#define LCD_HSPW 0x01

#define LCD_VBPD 10
#define LCD_VFPD 4
#define LCD_VSPW 1

#define LCD_CON5 ( (1<<11) | (1<<9) | (1<<8) | (1<<3) | (1<<0) ) 

void board_video_init(GraphicDevice *pGD) 
{ 
	struct s3c24x0_lcd * const lcd	 = s3c24x0_get_base_lcd(); 


	lcd->LCDCON1 = ( (LCD_CLKVAL<<8) | (3<<5) | (0xc<<1) ) ; 
	lcd->LCDCON2 = (LCD_VBPD<<24)|((LCD_HEIGHT-1)<<14)|((LCD_VFPD)<<6)|((LCD_VSPW)<<0);
	lcd->LCDCON3 = (LCD_HBPD<<19)|((LCD_WIDTH-1)<<8)|(LCD_HFPD<<0);
	lcd->LCDCON4 = (LCD_HSPW<<0);

	lcd->LCDCON5 = LCD_CON5; 
} 



//extern int dm9000_initialize(bd_t *bis);

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_CS8900
	rc = cs8900_initialize(0, CONFIG_CS8900_BASE);
#endif
#ifdef CONFIG_DRIVER_DM9000
	rc = dm9000_initialize(bis);
#endif
	return rc;
}
#endif
