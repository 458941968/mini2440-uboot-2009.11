/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
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


#define FLASH_BANK_SIZE		PHYS_FLASH_SIZE     //==Nor Flash大小
#define MAIN_SECT_SIZE      0x10000	            //==Nor Flash主扇区大小64K

#define CMD_UNLOCK_START    0x000000AA  //==解锁步骤一
#define CMD_UNLOCK_ACK		0x00000055	//==解锁步骤二
#define CMD_UNLOCK_BYPASS	0x00000020  //==跳过解锁步骤
#define CMD_RESET		    0x000000F0	//==复位为读模式
#define CMD_ERASE_START		0x00000080  //==擦除开始
#define CMD_ERASE_SECTOR	0x00000030  //==扇区擦除确认
#define CMD_WRITE		    0x000000A0  //==写
#define CMD_AUTOSELECT		0x90        //==自动选择模式

#define MEM_FLASH_ADDR1		(*(volatile u16 *)(CONFIG_SYS_FLASH_BASE + (0x00000555 << 1)))
#define MEM_FLASH_ADDR2		(*(volatile u16 *)(CONFIG_SYS_FLASH_BASE + (0x000002AA << 1)))

#define BIT_ERASE_DONE		0x00000080	//==1000 0000-------------DQ7
#define BIT_RDY_MASK		0x00000080
#define BIT_PROGRAM_ERROR	0x00000020	//==0010 0000---------DQ5

#define READY 1
#define ERR   2
#define TIMEOUT   4


flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];    //==描述flash结构体
/*-----------------------------------------------------------------------
 * -->size
 * -->sector_count
 * -->flash_id
 * -->start[]
 * --> protect[]
 */


/********************************************************************
函数功能：读Nor Flash的ID。
入口参数：
  id_buf：Flash ID缓冲区  
返    回：成功0。
备    注：无。
********************************************************************/
unsigned long flash_get_id(unsigned long *id_buf)
{
	MEM_FLASH_ADDR1 = CMD_UNLOCK_START;
	MEM_FLASH_ADDR2 = CMD_UNLOCK_ACK;
	MEM_FLASH_ADDR1 = CMD_AUTOSELECT;

	*id_buf = (*(volatile unsigned short *)(CONFIG_SYS_FLASH_BASE + (0x100<<1))) << 16;
	*id_buf |=  (*(volatile unsigned short *)(CONFIG_SYS_FLASH_BASE + (0x101<<1)));

	return ERR_OK;
}


/********************************************************************
函数功能：初始化Nor Flash。
入口参数：无  
返    回：总的Flash大小。
备    注：无。
********************************************************************/
ulong flash_init (void)
{
	int i, j;
	ulong size = 0;
	unsigned long flash_id = 0;	//==add by hyq

	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
		ulong flashbase = 0;

		flash_get_id( &flash_id );          //==add by hyq
		*(volatile u16*)0x0 = CMD_RESET;    //==读ID后要复位才能返回读模式
		debug( "\rNor flash ID is 0x%lx\n", flash_id);

		flash_info[i].flash_id = flash_id;
#if 0	//==add by hyq
#if defined(CONFIG_AMD_LV400)
			(AMD_MANUFACT & FLASH_VENDMASK) |
			(AMD_ID_LV400B & FLASH_TYPEMASK);
#elif defined(CONFIG_AMD_LV800)
			(AMD_MANUFACT & FLASH_VENDMASK) |
			(AMD_ID_LV800B & FLASH_TYPEMASK);
#elif defined(CONFIG_AMD_LV160B)
			(AMD_MANUFACT & FLASH_VENDMASK) |
			(AMD_ID_LV160B & FLASH_TYPEMASK);
#else
#error "Unknown flash configured"
#endif
#endif	//==add by hyq

		flash_info[i].size = FLASH_BANK_SIZE;
		flash_info[i].sector_count = CONFIG_SYS_MAX_FLASH_SECT;
		memset (flash_info[i].protect, 0, CONFIG_SYS_MAX_FLASH_SECT);
		if (i == 0)
			flashbase = PHYS_FLASH_1;
		else
			panic ("configured too many flash banks!\n");
		for (j = 0; j < flash_info[i].sector_count; j++) {
			if (j <= 3) {
				/* 1st one is 16 KB */
				if (j == 0) {
					flash_info[i].start[j] =
						flashbase + 0;
				}

				/* 2nd and 3rd are both 8 KB */
				if ((j == 1) || (j == 2)) {
					flash_info[i].start[j] =
						flashbase + 0x4000 + (j -
								      1) *
						0x2000;
				}

				/* 4th 32 KB */
				if (j == 3) {
					flash_info[i].start[j] =
						flashbase + 0x8000;
				}
			} else {
				flash_info[i].start[j] =
					flashbase + (j - 3) * MAIN_SECT_SIZE;
			}
		}
		size += flash_info[i].size;
	}

	flash_protect (FLAG_PROTECT_SET,
		       CONFIG_SYS_FLASH_BASE,
		       CONFIG_SYS_FLASH_BASE + monitor_flash_len - 1,
		       &flash_info[0]);

	flash_protect (FLAG_PROTECT_SET,
		       CONFIG_ENV_ADDR,
		       CONFIG_ENV_ADDR + CONFIG_ENV_SIZE - 1, &flash_info[0]);

	return size;
}


/********************************************************************
函数功能：打印Nor Flash信息。
入口参数：
	info：描述Flash结构体  
返    回：无。
备    注：无。
********************************************************************/
void flash_print_info (flash_info_t * info)
{
	int i;

	switch (info->flash_id & FLASH_VENDMASK) {
	case (AMD_MANUFACT & FLASH_VENDMASK):
		printf ("AMD: ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case (AMD_ID_LV400B & FLASH_TYPEMASK):
		printf ("1x Amd29LV400BB (4Mbit)\n");
		break;
	case (AMD_ID_LV800B & FLASH_TYPEMASK):
		printf ("1x Amd29LV800BB (8Mbit)\n");
		break;
	case (AMD_ID_LV160B & FLASH_TYPEMASK):
		printf ("1x (Amd)S29AL16**2B (16Mbit)\n");
		break;
	default:
		printf ("Unknown Chip Type\n");
		goto Done;
		break;
	}

	printf ("  Size: %ld MB in %d Sectors\n",
		info->size >> 20, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; i++) {
		if ((i % 5) == 0) {
			printf ("\n   ");
		}
		printf (" %08lX%s", info->start[i],
			info->protect[i] ? " (RO)" : "     ");
	}
	printf ("\n");

      Done:;
}


/********************************************************************
函数功能：Nor Flash擦除序列。
入口参数：
	info：描述Flash结构体           addr1 addr2 addr1 add1 addr2 user
 s_first：扇区的开始地址            aa-----55----80----aa---55----30
  s_last：扇区的结束地址
返    回：返回Flahs操作状态，成功返回0。
备    注：DQ7 编程时 0：正在进行	 1：完成。
********************************************************************/
int flash_erase (flash_info_t * info, int s_first, int s_last)
{
	ushort result;
	int iflag, cflag, prot, sect;
	int rc = ERR_OK;
	int chip;

	/* first look for protection bits */

	if (info->flash_id == FLASH_UNKNOWN)                //==参数检查
		return ERR_UNKNOWN_FLASH_TYPE;

	if ((s_first < 0) || (s_first > s_last)) {
		return ERR_INVAL;
	}

	if ((info->flash_id & FLASH_VENDMASK) !=
	    (AMD_MANUFACT & FLASH_VENDMASK)) {
		return ERR_UNKNOWN_FLASH_VENDOR;
	}

	prot = 0;                                           //==保护检查
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}
	if (prot)
		return ERR_PROTECTED;                           

	/*
	 * Disable interrupts which might cause a timeout
	 * here. Remember that our exception vectors are
	 * at address 0 in the flash, and we don't want a
	 * (ticker) exception to happen while the flash
	 * chip is in programming mode.
	 */
	cflag = icache_status ();                           //==禁止指令cache和中断
	icache_disable ();
	iflag = disable_interrupts ();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last && !ctrlc (); sect++) {
		printf ("Erasing sector %2d ... ", sect);

		/* arm simple, non interrupt dependent timer */
		reset_timer_masked ();		//==复位时间计数

		if (info->protect[sect] == 0) {	/* not protected */
			vu_short *addr = (vu_short *) (info->start[sect]);

			MEM_FLASH_ADDR1 = CMD_UNLOCK_START;
			MEM_FLASH_ADDR2 = CMD_UNLOCK_ACK;
			MEM_FLASH_ADDR1 = CMD_ERASE_START;

			MEM_FLASH_ADDR1 = CMD_UNLOCK_START;
			MEM_FLASH_ADDR2 = CMD_UNLOCK_ACK;
			*addr = CMD_ERASE_SECTOR;

			/* wait until flash is ready */
			chip = 0;

			do {
                result = *addr;

                /* check timeout */
                if (get_timer_masked () > CONFIG_SYS_FLASH_ERASE_TOUT){ //==时间大于5秒
                    MEM_FLASH_ADDR1 = CMD_RESET;
                    chip = TIMEOUT;
                    break;
                }

                if (!chip && (result & 0xFFFF) & BIT_ERASE_DONE)        //==正常擦除完成
                    chip = READY;

                if (!chip && (result & 0xFFFF) & BIT_PROGRAM_ERROR)     //==擦除出差
                    chip = ERR;

			} while (!chip);

			MEM_FLASH_ADDR1 = CMD_RESET;

			if (chip == ERR) {
				rc = ERR_PROG_ERROR;
				goto outahere;
			}
			if (chip == TIMEOUT) {
				rc = ERR_TIMOUT;
				goto outahere;
			}

			printf ("ok.\n");
		} else {	/* it was protected */

			printf ("protected!\n");
		}
	}

	if (ctrlc ())
		printf ("User Interrupt!\n");

      outahere:                             
	/* allow flash to settle - wait 10 ms */
	udelay_masked (10000);

	if (iflag)                              //==重新开启指令cache和中断
		enable_interrupts ();

	if (cflag)
		icache_enable ();

	return rc;
}


/********************************************************************
函数功能：Nor Flash编程序列。
入口参数：
	info：描述Flash结构体                 addr1 addr2 addr1 user
    dest：Flash地址                       aa----55----a0----data
	data：要写入的数据
返    回：返回Flahs操作状态，成功返回0。
备    注：DQ7 编程时 0：正在进行	 1：完成。
********************************************************************/
static int write_hword (flash_info_t * info, ulong dest, ushort data)
{
	vu_short *addr = (vu_short *) dest;
	ushort result;
	int rc = ERR_OK;
	int cflag, iflag;
	int chip;

	/*
	 * Check if Flash is (sufficiently) erased
	 */
	result = *addr;     
	if ((result & data) != data)
		return ERR_NOT_ERASED;


	/*
	 * Disable interrupts which might cause a timeout
	 * here. Remember that our exception vectors are
	 * at address 0 in the flash, and we don't want a
	 * (ticker) exception to happen while the flash
	 * chip is in programming mode.
	 */
	cflag = icache_status ();
	icache_disable ();
	iflag = disable_interrupts ();

	MEM_FLASH_ADDR1 = CMD_UNLOCK_START;
	MEM_FLASH_ADDR2 = CMD_UNLOCK_ACK;
#if 0
	MEM_FLASH_ADDR1 = CMD_WRITE;
	*addr = data;
#else	 /*使用绕开解锁方式编程*/
	MEM_FLASH_ADDR1 = CMD_UNLOCK_BYPASS;
	*addr = CMD_WRITE;
	*addr = data;
#endif


	/* arm simple, non interrupt dependent timer */
	reset_timer_masked ();		//==复位时间计数

	/* wait until flash is ready */
	chip = 0;
	do {
		result = *addr;

		/* check timeout */
		if (get_timer_masked () > CONFIG_SYS_FLASH_ERASE_TOUT) {//==超过5秒
			chip = ERR | TIMEOUT;
			break;
		}

		if (!chip && ((result & 0x80) == (data & 0x80)))
			chip = READY;

		if (!chip && ((result & 0xFFFF) & BIT_PROGRAM_ERROR)) {
			result = *addr;

			if ((result & 0x80) == (data & 0x80))
				chip = READY;
			else
				chip = ERR;
		}

	} while (!chip);

	*addr = CMD_RESET;

	if (chip == ERR || *addr != data)
		rc = ERR_PROG_ERROR;

	if (iflag)
		enable_interrupts ();

	if (cflag)
		icache_enable ();

	return rc;
}


/********************************************************************
函数功能：复制内存数据到Flash。
入口参数：
	info：描述Flash结构体
	 src：内存地址
    addr：Flash地址
	 cnt：要写入的数据个数
返    回：无。
备    注：无。
********************************************************************/
int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	ulong cp, wp;
	int l;
	int i, rc;
	ushort data;

	wp = (addr & ~1);	/* get lower word aligned address */

	/*
	 * handle unaligned start bytes 开始字节不对齐
	 */
	if ((l = addr - wp) != 0) {

		data = 0;

		for (i = 0, cp = wp; i < l; ++i, ++cp) {
			data = (data >> 8) | (*(uchar *) cp << 8);
		}

		for (; i < 2 && cnt > 0; ++i) {
			data = (data >> 8) | (*src++ << 8);
			--cnt;
			++cp;
		}

		for (; cnt == 0 && i < 2; ++i, ++cp) {
			data = (data >> 8) | (*(uchar *) cp << 8);
		}

		if ((rc = write_hword (info, wp, data)) != 0) {
			return (rc);
		}
		wp += 2;
	}

	/*
	 * handle word aligned part 对齐字节
	 */
	while (cnt >= 2) {
		data = *((vu_short *) src);     //==取出2个字节的内存数据
		if ((rc = write_hword (info, wp, data)) != 0) { //==一次写2个字节
			return (rc);    //==出错
		}
		src += 2;
		wp += 2;
		cnt -= 2;
	}

	if (cnt == 0) {
		return ERR_OK;  //==正常写完
	}

	/*
	 * handle unaligned tail bytes 结尾地址不对齐
	 */
	data = 0;
	for (i = 0, cp = wp; i < 2 && cnt > 0; ++i, ++cp) {
		data = (data >> 8) | (*src++ << 8);
		--cnt;
	}

	for (; i < 2; ++i, ++cp) {
		data = (data >> 8) | (*(uchar *) cp << 8);
	}

	return write_hword (info, wp, data);
}
