/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2410 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H


/*---------------------------------��༶����---------------------------------*/
#undef  CONFIG_SKIP_LOWLEVEL_INIT	    /* undef for developing */
#define CONFIG_DEBUG_LL	                1   //==������
#define CONFIG_S3C2440_NAND_SKIP_BAD    1   //==��������
#define	CONFIG_HYQ2440_LED	            1	//==����Ʒ�           


/*----------------------------------��ϵ����----------------------------------*/
#define CONFIG_ARM920T		1	/* This is an ARM920T Core	*/
#define	CONFIG_S3C2440		1	/* in a SAMSUNG S3C2440 SoC  */
#define	CONFIG_HYQ2440		1	/* in a HYQ2440 Board  */


/*--------------------------------ʱ������-------------------------------------*/
#define CONFIG_SYS_CLK_FREQ	12000000/* the SMDK2410 has 12MHz input clock */


/*---------------------------------MMU����--------------------------------------*/
//#define USE_920T_MMU		1


/*---------------------------------�ж�����------------------------------------ */
//#undef CONFIG_USE_IRQ		/* we don't need IRQ/FIQ stuff */
#define CONFIG_USE_IRQ	    1


/*---------------------------------��������------------------------------------- */
#include <config_cmd_default.h>
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_ELF
#define CONFIG_CMD_HELLO    //==hyq
#define CONFIG_CMD_MENU     //==hyq


/*---------------------------------������������--------------------------------*/
//#define CONFIG_BOOTARGS	"noinitrd root=/dev/nfs rw nfsroot=192.168.1.10:/home/rootfs ip=192.168.1.19:192.168.1.10::255.255.255.0 console=ttySAC0,115200 init=/linuxrc mem=64M"
#define CONFIG_BOOTARGS 	"noinitrd root=/dev/mtdblock4 init=/linuxrc console=ttySAC0"
#define CONFIG_BOOTCOMMAND	"nand read 0x30007fc0 0x100000 0x500000\;bootm 0x30007fc0" 
#define CONFIG_BOOTDELAY	2
#define CONFIG_BAUDRATE		115200
#define CONFIG_ETHADDR	    08:00:3e:26:0a:5b 
#define CONFIG_IPADDR		192.168.1.19
#define CONFIG_SERVERIP		192.168.1.10
#define CONFIG_GATEWAYIP	192.168.1.1
#define CONFIG_NETMASK      255.255.255.0
//#define CONFIG_BOOTFILE	"uImage"
//#define CONFIG_LOADADD    192.168.1.10
//#define CONFIG_ENV_OVERWRITE
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"logo=nand read 0x31000000 0x80000 0x80000\;bmp d 0x31000000\0"				\
	"stdin=serial\0"					\
	"stdout=serial\0"					\
	"stderr=serial\0"					\
	""
//#define	CONFIG_ENV_IS_IN_FLASH	1
#define	CONFIG_ENV_IS_IN_NAND   1
//#define	CONFIG_ENV_IS_IN_EEPROM   1
#define CONFIG_ENV_SIZE		    0x20000	/* Total Size of Environment Sector */
#define CONFIG_ENV_ADDR		    0x60000
#define CONFIG_ENV_OFFSET		CONFIG_ENV_ADDR


/*---------------------------------�ں�����������--------------------------------*/
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_TAG


/*-----------------------------------������--------------------------------------*/
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128*1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128


/*---------------------------------ջ����-------------------------------------- */
#define CONFIG_STACKSIZE	    (128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif


/*---------------------------------�ڴ�����--------------------------------------*/
#define CONFIG_NR_DRAM_BANKS	 1	   /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		    0x30000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	    0x04000000 /* 64 MB */


/*---------------------------------������������---------------------------------*/
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1          1	/* we use SERIAL 1 on SMDK2410 */


/*---------------------------------I2C��EEPROM--------------------------------------*/
#if 1
#define CONFIG_CMD_I2C
#define CONFIG_DRIVER_S3C24X0_I2C   1    /* we use the buildin I2C controller */
#define CONFIG_HARD_I2C             1        /* I2C with hardware support */
#define CONFIG_SYS_I2C_SPEED        100000    /* I2C speed and slave address */
#define CONFIG_SYS_I2C_SLAVE        0x0
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS    10   /* and takes up to 10 msec */

#if 1
#define CONFIG_CMD_EEPROM
#define CONFIG_SYS_I2C_EEPROM_ADDR  0x50    /* ?? */ 
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN     1    /*���ݵ�ַ����*/
#define CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW    0x07
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS       4
#define CONFIG_SYS_EEPROM_PAGE_WRITE_ENABLE 
#endif
#endif


/*---------------------------------������������----------------------------------*/
#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI                1
#define CONFIG_DRIVER_DM9000            1
#define CONFIG_DM9000_BASE              0x20000300              //����Ƭѡ��ַ
#define DM9000_IO                       CONFIG_DM9000_BASE
#define DM9000_DATA                     (CONFIG_DM9000_BASE+4)  //�������ݵ�ַ
#define CONFIG_DM9000_NO_SROM           1
//#define CONFIG_DM9000_USE_16BIT
#undef  CONFIG_DM9000_DEBUG


/*--------------------------------ʵʱʱ����������------------------------------*/
#define	CONFIG_RTC_S3C24X0	1


/*--------------------------------Nor Flash��������-----------------------------*/
#define PHYS_FLASH_1		        0x00000000 /* Flash Bank #1 */
#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_AMD_LV160B	        1
#define PHYS_FLASH_SIZE		        0x00200000 /* 2MB */
#define CONFIG_SYS_MAX_FLASH_SECT	(35)	/* max number of sectors on one chip */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(5*CONFIG_SYS_HZ) /* Timeout for Flash Erase */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(5*CONFIG_SYS_HZ) /* Timeout for Flash Write */


/*---------------------------------- Nand Flash��������---------------------------*/
#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BASE            0x4E000000 
#define CONFIG_SYS_MAX_NAND_DEVICE      1	/* Max number of NAND devices  */
#define CONFIG_MTD_NAND_YAFFS2          1		//����һ�������Yaffs2֧�ֵĺ�
//#define CONFIG_SYS_DIRECT_NAND_TFTP
#define CONFIG_NAND_S3C2440		        //==����s3c2440_nand.c����
//#define CONFIG_S3C2440_NAND_HWECC     /*��nand flashӲ��ecc֧��*/
//#define CONFIG_SYS_NAND_ECCSIZE     256    /*ÿ��ecc���ݿ��С,��Ӧ�ں˵�chip->ecc.size*/
//#define CONFIG_SYS_NAND_ECCBYTES    3    /*У����Ϊ3���ֽ�,��Ӧ�ں˵�chip->ecc.bytes*/


/*---------------------------------USB Slave��������---------------------------------*/
#ifdef	CONFIG_USE_IRQ
#define CONFIG_USB_SLAVE	1
//#define CONFIG_USB_DEMO
#define CONFIG_USB_UART
//#define CONFIG_USB_HID_KBD
#endif


/*---------------------------------�ļ�ϵͳ����--------------------------------- */
#if 1
#define CONFIG_CMD_FAT
#define CONFIG_SUPPORT_VFAT
#define CONFIG_DOS_PARTITION
#define CONFIG_SYS_DEVICE_DEREGISTER
#define LITTLEENDIAN
#endif


/*---------------------------------USB������������--------------------------------- */
#if 1
#define CONFIG_CMD_USB
#define CONFIG_USB_OHCI
#define CONFIG_USB_STORAGE
//#define CONFIG_USB_TTY
//#define CONFIG_USB_KEYBOARD
#endif


/*---------------------------------SD����������-------------------------------------- */
#if 1
#define CONFIG_CMD_MMC
#define CONFIG_MMC		1
#define CONFIG_S3C2440_MMC	1	/* Enabling the 2440 MMC driver */
#define CFG_MMC_BASE		0xff000000
#endif


/*----------------------------------LCD��������-------------------------------------- */
#if 1
#define CONFIG_VIDEO 
#define CONFIG_CFB_CONSOLE

#define CONFIG_VIDEO_S3C2440
#define LCD_VIDEO_ADDR         0x33d00000
#define CONFIG_DISPLAY_LOGO

//#define VIDEO_FB_LITTLE_ENDIAN 
//#define CFG_CONSOLE_INFO_QUIET
//#define CONFIG_CONSOLE_CURSOR
//#define CONFIG_CONSOLE_EXTRA_INFO  //==����logo�������������Ϣ
//#define CONFIG_CONSOLE_TIME	//==�����Ͻ���ʾʱ��/������Ϣ (��Ҫ CONFIG_CMD_DATE)

//#define CONFIG_SILENT_CONSOLE

//#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE    (320*240+1024+100) /* 100 = slack */

//#define VIDEO_FB_16BPP_PIXEL_SWAP 

//#define CONFIG_VIDEO_LOGO //==�����Ͻ���ʾLinux logo

//#define CONFIG_VIDEO_SW_CURSOR 

//#define CONFIG_VIDEO_BMP_LOGO //==ʹ��bmp_logo.h��Ϊlogo(Ĭ����linux_logo.h)��
#define CONFIG_CMD_BMP 


//#define CONFIG_VIDEO_BMP_GZIP 
//#define CONFIG_CMD_UNZIP

#define CONFIG_SPLASH_SCREEN
//#define CONFIG_SPLASH_SCREEN_ALIGN	//==����ͼƬ��ʾλ��


#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/*for PC-keyboard*/
#define VIDEO_KBD_INIT_FCT     0 
#define VIDEO_TSTC_FCT         serial_tstc 
#define VIDEO_GETC_FCT         serial_getc
#endif


/*---------------------------------��������-------------------------------------- */
#define CONFIG_CMDLINE_EDITING	    //history
#define CONFIG_AUTO_COMPLETE	    //tab
#define	CONFIG_SYS_LONGHELP			/* undef to save memory		*/
#define	CONFIG_SYS_PROMPT		    "[������@hyq2440#]"	/* Monitor Command Prompt	*/
#define	CONFIG_SYS_CBSIZE		    256		/* Console I/O Buffer Size	*/
#define	CONFIG_SYS_PBSIZE           (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		    16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE		    CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/
#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM	*/
#define	CONFIG_SYS_LOAD_ADDR		0x30008000	/* default load address	*/
#define	CONFIG_SYS_HZ			    1000
/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#endif	/* __CONFIG_H */
