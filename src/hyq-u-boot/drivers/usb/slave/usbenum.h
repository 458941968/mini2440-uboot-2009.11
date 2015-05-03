/*
 *************************************************************************
 *
 * USB设备枚举头文件
 *
 *************************************************************************
 */

#ifndef __USBDENUM_H__
#define __USBDENUM_H__


#define DBG_USBD_LEVEL 1
#define EP0_MAX_PACKET_SIZE     EP0_PKT_SIZE  			//==端点0包大小，8字节
#define DEFAULT_CONFIG_ID       1						//==默认配置

//#define DbgPrintX(level,fmt,args...)  printf(fmt,##args);

//==函数声明
void USBDInit(void);

#endif
