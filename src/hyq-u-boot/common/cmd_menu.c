/*
 * thisway.diy@163.com, www.100ask.net
 *
 */

#include <common.h>
#include <command.h>
#include <nand.h>

extern char console_buffer[];
extern int readline (const char *const prompt);
/*==hyq
extern char console_buffer[];
extern int readline (const char *const prompt);
extern char awaitkey(unsigned long delay, int* error_p);
extern void download_nkbin_to_flash(void);
*/

/**
 * Parses a string into a number.  The number stored at ptr is
 * potentially suffixed with K (for kilobytes, or 1024 bytes),
 * M (for megabytes, or 1048576 bytes), or G (for gigabytes, or
 * 1073741824).  If the number is suffixed with K, M, or G, then
 * the return value is the number multiplied by one kilobyte, one
 * megabyte, or one gigabyte, respectively.
 *
 * @param ptr where parse begins
 * @param retptr output pointer to next char after parse completes (output)
 * @return resulting unsigned int
 */

 /*==hyq
static unsigned long memsize_parse2 (const char *const ptr, const char **retptr)
{
	unsigned long ret = simple_strtoul(ptr, (char **)retptr, 0);
    int sixteen = 1;

	switch (**retptr) {
		case 'G':
		case 'g':
			ret <<= 10;
		case 'M':
		case 'm':
			ret <<= 10;
		case 'K':
		case 'k':
			ret <<= 10;
			(*retptr)++;
            sixteen = 0;
		default:
			break;
	}

    if (sixteen)
        return simple_strtoul(ptr, NULL, 16);
    
	return ret;
}

*/

void param_menu_usage(void)
{
    printf("\r\n##### Parameter Menu #####\r\n");
    printf("[v] View the parameters\r\n");
    printf("[s] Set parameter \r\n");
    printf("[n] Set bootargs to mount nfs\n");
    printf("[f] Set bootargs to mount mtdblock3\n");
    printf("[d] Delete parameter \r\n");
    printf("[w] Write the parameters to flash memeory \r\n");
    printf("[q] Quit \r\n");
    printf("Enter your selection: ");
}


void param_menu_shell(void)
{
    char c;
    char cmd_buf[256];
//    char name_buf[20];
//    char val_buf[256];
    
    while (1)
    {
        param_menu_usage();
 	    c = getc();
        printf("%c\n", c);

        switch (c)
        {
            case 'v':
            {
                strcpy(cmd_buf, "printenv ");
                printf("Name(enter to view all paramters): ");
                readline(NULL);
                strcat(cmd_buf, console_buffer);
                run_command(cmd_buf, 0);
                break;
            }
            
            case 's':
            {
                sprintf(cmd_buf, "setenv ");

                printf("Name: ");
                readline(NULL);
                strcat(cmd_buf, console_buffer);

                printf("Value: ");
                readline(NULL);
                strcat(cmd_buf, " ");
                strcat(cmd_buf, console_buffer);

                run_command(cmd_buf, 0);
                break;
            }
            
			case 'n':
            {
                sprintf(cmd_buf, "setenv bootargs noinitrd root=/dev/nfs rw nfsroot=192.168.1.10:/home/rootfs ip=192.168.1.19:192.168.1.10::255.255.255.0 console=ttySAC0,115200 init=/linuxrc mem=64M");

                run_command(cmd_buf, 0);
                break;
            }

			case 'f':
            {
                sprintf(cmd_buf, "setenv bootargs noinitrd root=/dev/mtdblock4 init=/linuxrc console=ttySAC0");

                run_command(cmd_buf, 0);
                break;
            }

            case 'd':
            {
                sprintf(cmd_buf, "setenv ");

                printf("Name: ");
                readline(NULL);
                strcat(cmd_buf, console_buffer);

                run_command(cmd_buf, 0);
                break;
            }
            
            case 'w':
            {
                sprintf(cmd_buf, "saveenv");
                run_command(cmd_buf, 0);
                break;
            }
            
            case 'q':
            {
                return;
                break;
            }
        }
    }
}


void main_menu_usage(void)
{
    printf("\n############ u-boot移植版本 #############\n");
    printf("姓名：黄银青\n");
    printf("专业：微电子学\n");
    printf("学号：110800836\n\n");

//==成品功能
	printf("[o] Download Boot Loader \"u-boot.bin\" to Nor Flash\n");
    printf("[u] Download Boot Loader \"u-boot.bin\" to Nand Flash\n");
    printf("[l] Download Logo \"logo.bmp\" to Nand Flash\n");
    printf("[k] Download Linux kernel \"uImage\" to Nand Flash\n");
    printf("[y] Download Rootfs \"rootfs.yaffs2\" to Nand Flash\n");


//==开发测试功能
	printf("[d] Download \"test.bin\"to SDRAM & Run\r\n");
	printf("[b] Download \"u-boot.bin\"to SDRAM & Run\r\n");
    printf("[t] Download \"uImage\" to SDRAM & Boot\r\n");

    printf("[f] Format the Nand Flash\r\n");
    printf("[s] Set the boot parameters\r\n");
    printf("[r] Reboot u-boot\r\n");
    printf("[q] Quit from menu\r\n");
    printf("Enter your selection: ");
}


void menu_shell(void)
{
    char c;
    char cmd_buf[200];

    while (1)
    {
        main_menu_usage();

		c = getc();

        printf("%c\n", c);

        switch (c)
        {	
            case 'o':
            {
                strcpy(cmd_buf, "tftp 0x30000000 u-boot.bin; protect off 1:0-8; erase 1:0-8; cp.b 0x30000000 0 40000");
                run_command(cmd_buf, 0);
                break;
            }

			case 'u':
			{
                strcpy(cmd_buf, "tftp 0x30000000 u-boot.bin; nand erase 0 0x60000; nand write 0x30000000 0 0x60000");
                run_command(cmd_buf, 0);
                break;
			}

			case 'l':
			{
                strcpy(cmd_buf, "tftp 0x30000000 logo.bmp; nand erase 0x80000 0x80000; nand write 0x30000000 0x80000 0x80000");
                run_command(cmd_buf, 0);
                break;
			} 
            
            case 'k':
            {
                strcpy(cmd_buf, "tftp 0x30000000 uImage; nand erase 0x100000 0x500000; nand write 0x30000000 0x100000 0x500000");
                run_command(cmd_buf, 0);
                break;
            }
/*
            case 'j':
            {
                strcpy(cmd_buf, "usbslave 1 0x30000000; nand erase root; nand write.jffs2 0x30000000 root $(filesize)");
                run_command(cmd_buf, 0);
                break;
            }
#if 0
            case 'c':
            {
                strcpy(cmd_buf, "usbslave 1 0x30000000; nand erase root; nand write.jffs2 0x30000000 root $(filesize)");
                run_command(cmd_buf, 0);
                break;
            }
#endif
*/
#ifdef CONFIG_SYS_DIRECT_NAND_TFTP
            case 'y':
            {
                strcpy(cmd_buf, "nand erase 0x600000 0x25000000; tftp 0x600000 rootfs.yaffs2;");
                run_command(cmd_buf, 0);
                break;
            }
#else 
            case 'y':
            {
                strcpy(cmd_buf, "tftp 0x30000000 rootfs.yaffs2; nand erase 0x600000 $(filesize); nand write.yaffs2 0x30000000 0x600000 $(filesize)");
                run_command(cmd_buf, 0);
                break;
            }
#endif
			case 'd':
			{
                strcpy(cmd_buf, "tftp 0x30000000 test.bin;go 0x30000000");
                run_command(cmd_buf, 0);
                break;
			}

			case 'b':
			{
                strcpy(cmd_buf, "tftp 0x30000000 u-boot.bin;go 0x30000000");
                run_command(cmd_buf, 0);
                break;
			}

			case 't':
			{
                strcpy(cmd_buf, "tftp 0x30007fc0 uImage; bootm 0x30007FC0");
                run_command(cmd_buf, 0);
                break;
			}

			case 'f':
			{
                printf("Are you really want to format nand flash?(y/n)\n");

                if(getc()=='y')
                {
                    strcpy(cmd_buf, "nand erase 0 0x25600000");
                    run_command(cmd_buf, 0);
                }else
                {
                    printf("Not format nand flash!\n");
                }
				break;
			}

			case 's':
			{
				param_menu_shell();
				break;
			}

            case 'r':
            {
				strcpy(cmd_buf, "reset");
				run_command(cmd_buf, 0);
                break;
            }
            
            case 'q':
            {
                return;    
                break;
            }

        }
         
    }
}

int do_menu (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    menu_shell();
    return 0;
}

U_BOOT_CMD(
	menu,	3,	0,	do_menu,
	"menu - display a menu, to select the items to do something\n",
	" - display a menu, to select the items to do something"
);

