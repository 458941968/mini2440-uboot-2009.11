/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
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

#include <nand.h>
#include <s3c2440.h>
#include <asm/io.h>

#define	NF_BASE		0x4e000000	//==hyq
ulong IO_ADDR_W = NF_BASE;		//==hyq

#if defined(CONFIG_S3C2410)
#define S3C2410_NFCONF_EN          (1<<15)
#define S3C2410_NFCONF_512BYTE     (1<<14)
#define S3C2410_NFCONF_4STEP       (1<<13)
#define S3C2410_NFCONF_INITECC     (1<<12)
#define S3C2410_NFCONF_nFCE        (1<<11)
#define S3C2410_NFCONF_TACLS(x)    ((x)<<8)
#define S3C2410_NFCONF_TWRPH0(x)   ((x)<<4)
#define S3C2410_NFCONF_TWRPH1(x)   ((x)<<0)
#define S3C2410_ADDR_NALE 4
#define S3C2410_ADDR_NCLE 8
#endif

#if defined(CONFIG_S3C2440)
#define S3C2440_NFCONT_EN          (1<<0)
#define S3C2440_NFCONT_INITECC     (1<<4)
#define S3C2440_NFCONT_nFCE        (1<<1)
#define S3C2440_NFCONT_MAINECCLOCK (1<<5)
#define S3C2440_NFCONF_TACLS(x)    ((x)<<12)
#define S3C2440_NFCONF_TWRPH0(x)   ((x)<<8)
#define S3C2440_NFCONF_TWRPH1(x)   ((x)<<4)
#define S3C2440_ADDR_NALE 0x08	//==CMD�Ĵ���
#define S3C2440_ADDR_NCLE 0x0c	//==ADDR�Ĵ���
#endif


static void s3c2440_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
//	struct nand_chip *chip = mtd->priv;
	struct s3c2440_nand *nand = s3c2440_get_base_nand();

	debugX(7, "hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE) {
		IO_ADDR_W = (ulong)nand;

		if (!(ctrl & NAND_CLE))	//==Ҫд���ǵ�ַ
			IO_ADDR_W |= S3C2440_ADDR_NCLE;
		if (!(ctrl & NAND_ALE))	//==Ҫд��������
			IO_ADDR_W |= S3C2440_ADDR_NALE;

		//chip->IO_ADDR_W = (void *)IO_ADDR_W;
//#if defined(CONFIG_S3C2410)
//		if (ctrl & NAND_NCE)
//			writel(readl(&nand->NFCONF) & ~S3C2440_NFCONF_nFCE,
//			       &nand->NFCONF);
//		else
//			writel(readl(&nand->NFCONF) | S3C2440_NFCONF_nFCE,
//			       &nand->NFCONF);
//	}
//#endif
#if defined(CONFIG_S3C2440)
		if (ctrl & NAND_NCE)
			writel(readl(&nand->NFCONT) & ~S3C2440_NFCONT_nFCE,
			       &nand->NFCONT);
		else
			writel(readl(&nand->NFCONT) | S3C2440_NFCONT_nFCE,
			       &nand->NFCONT);
	}
#endif

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, (void *)IO_ADDR_W);
}

static int s3c2440_dev_ready(struct mtd_info *mtd)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	return readl(&nand->NFSTAT) & 0x01;
}

#ifdef CONFIG_S3C2440_NAND_HWECC
void s3c2440_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	debugX(1, "s3c2440_nand_enable_hwecc(%p, %d)\n", mtd, mode);

	writel(readl(&nand->NFCONT) | S3C2440_NFCONT_INITECC, &nand->NFCONT);
}

static int s3c2440_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
				      u_char *ecc_code)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	unsigned long ecc = nand->NFMECC0;

	ecc_code[0] = ecc;
	ecc_code[1] = ecc>>8;
	ecc_code[2] = ecc>>16;
	debugX(1, "s3c2440_nand_calculate_hwecc(%p,): 0x%02x 0x%02x 0x%02x\n",
	       mtd , ecc_code[0], ecc_code[1], ecc_code[2]);

	return 0;
}

static int s3c2440_nand_correct_data(struct mtd_info *mtd, u_char *dat,
				     u_char *read_ecc, u_char *calc_ecc)
{
	if (read_ecc[0] == calc_ecc[0] &&
	    read_ecc[1] == calc_ecc[1] &&
	    read_ecc[2] == calc_ecc[2])
		return 0;

	printf("s3c2440_nand_correct_data: not implemented\n");
	return -1;
}
#endif

int board_nand_init(struct nand_chip *nand)
{
	u_int32_t cfg;
	u_int8_t tacls, twrph0, twrph1;
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();

	debugX(1, "board_nand_init()\n");

	writel(readl(&clk_power->CLKCON) | (1 << 4), &clk_power->CLKCON);

#if defined(CONFIG_S3C2410)
	/* initialize hardware */
	twrph0 = 3;
	twrph1 = 0;
	tacls = 0;

	cfg = S3C2440_NFCONF_EN;
	cfg |= S3C2440_NFCONF_TACLS(tacls - 1);
	cfg |= S3C2440_NFCONF_TWRPH0(twrph0 - 1);
	cfg |= S3C2440_NFCONF_TWRPH1(twrph1 - 1);
	writel(cfg, &nand_reg->NFCONF);

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void *)&nand_reg->NFDATA;
#endif
#if defined(CONFIG_S3C2440)
	twrph0 = 3;
	twrph1 = 1;
	tacls = 1;

	cfg = 0;
	cfg |= S3C2440_NFCONF_TACLS(tacls);
	cfg |= S3C2440_NFCONF_TWRPH0(twrph0 - 1);
	cfg |= S3C2440_NFCONF_TWRPH1(twrph1 - 1);
	writel(cfg, &nand_reg->NFCONF);

	cfg = (1<<4)|(0<<1)|(1<<0);
	writel(cfg, &nand_reg->NFCONT);

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void *)&nand_reg->NFDATA;
#endif

	/* read_buf and write_buf are default */
	/* read_byte and write_byte are default */

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s3c2440_hwcontrol;

	nand->dev_ready = s3c2440_dev_ready;

#ifdef CONFIG_S3C2440_NAND_HWECC
	nand->ecc.hwctl = s3c2440_nand_enable_hwecc;
	nand->ecc.calculate = s3c2440_nand_calculate_ecc;
	nand->ecc.correct = s3c2440_nand_correct_data;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.size = CONFIG_SYS_NAND_ECCSIZE;
	nand->ecc.bytes = CONFIG_SYS_NAND_ECCBYTES;
#else
	nand->ecc.mode = NAND_ECC_SOFT;
#endif

#ifdef CONFIG_S3C2440_NAND_BBT
	nand->options = NAND_USE_FLASH_BBT;
#else
	nand->options = 0;
#endif

	debugX(1, "end of nand_init\n");

	return 0;
}
