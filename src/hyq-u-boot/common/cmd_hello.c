#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <u-boot/zlib.h>
#include <bzlib.h>
#include <environment.h>
#include <lmb.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>

int do_hello(cmd_tbl_t *cmdtp,int flag,int argc,char *argv[])
{
	int i;
	printf("hello word!,%d\n",argc);

	for(i=0;i<argc;i++)
	{
		printf("argv[%d]=%s\n",i,argv[i]);
	}
	return 0;
}

U_BOOT_CMD(
	hello,CONFIG_SYS_MAXARGS,1,do_hello,
	"hello - just for test\n",
	"hello,long help ........\n"
);
