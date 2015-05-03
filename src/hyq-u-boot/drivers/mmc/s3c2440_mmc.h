/*
 *  linux/drivers/mmc/s3c2440_mmc.h
 *
 *  Author: Vladimir Shebordaev, Igor Oblakov
 *  Copyright:  MontaVista Software Inc.
 *
 *  $Id: mmc_pxa.h,v 0.3.1.6 2002/09/25 19:25:48 ted Exp ted $
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#ifndef __MMC_S3C2440_H__
#define __MMC_S3C2440_H__

#include <asm/arch/regs-sdi.h>

//#ifdef DEBUG
#if 1
#define pr_debug(fmt, args...) printf(fmt, ##args)
#else
#define pr_debug(...) do { } while(0)
#endif

#define CONFIG_MMC_WIDE
#define MMC_DEFAULT_RCA		(1<<16)
#define MMC_BLOCK_SIZE		512

#define CMD_F_RESP	        0x01
#define CMD_F_RESP_LONG	    0x02
#define CMD_F_RESP_R7       CMD_F_RESP

#define FIFO_FILL(host) ((host->SDIFSTA & S3C2410_SDIFSTA_COUNTMASK) >> 2)


typedef struct sd_cid {
	char		pnm_0;	/* product name MSB*/
	char		oid_1;	/* OEM/application ID */
	char		oid_0;
	uint8_t		mid;	/* manufacturer ID */

	char		pnm_4;  /* product name LSB*/
	char		pnm_3;
	char		pnm_2;
	char		pnm_1;

	uint8_t		psn_2;	/* product serial number */
	uint8_t		psn_1;
	uint8_t		psn_0;	/* MSB */
	uint8_t		prv;	/* product revision */

	uint8_t		crc;	/* CRC7 checksum, b0 is unused and set to 1 */
	uint8_t		mdt_1;	/* manufacturing date, LSB, RRRRyyyy yyyymmmm */
	uint8_t		mdt_0;	/* MSB */
	uint8_t		psn_3;	/* LSB */
}sd_cid_t;


typedef struct sd_csd
{
	uchar	ecc:2,
		file_format:2,
		tmp_write_protect:1,
		perm_write_protect:1,
		copy:1,
		file_format_grp:1;
	uint64_t content_prot_app:1,
		rsvd3:4,
		write_bl_partial:1,
		write_bl_len:4,
		r2w_factor:3,
		default_ecc:2,
		wp_grp_enable:1,
		wp_grp_size:5,
		erase_grp_mult:5,
		erase_grp_size:5,
		c_size_mult1:3,
		vdd_w_curr_max:3,
		vdd_w_curr_min:3,
		vdd_r_curr_max:3,
		vdd_r_curr_min:3,
		c_size:12,
		rsvd2:2,
		dsr_imp:1,
		read_blk_misalign:1,
		write_blk_misalign:1,
		read_bl_partial:1;

	ushort	read_bl_len:4,
		ccc:12;
	uchar	tran_speed;
	uchar	nsac;
	uchar	taac;
	uchar	rsvd1:2,
  		spec_vers:4,
		csd_structure:2;
}sd_csd_t;

#endif /* __MMC_S3C2440_H__ */
