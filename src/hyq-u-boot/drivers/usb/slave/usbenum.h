/*
 *************************************************************************
 *
 * USB�豸ö��ͷ�ļ�
 *
 *************************************************************************
 */

#ifndef __USBDENUM_H__
#define __USBDENUM_H__


#define DBG_USBD_LEVEL 1
#define EP0_MAX_PACKET_SIZE     EP0_PKT_SIZE  			//==�˵�0����С��8�ֽ�
#define DEFAULT_CONFIG_ID       1						//==Ĭ������

//#define DbgPrintX(level,fmt,args...)  printf(fmt,##args);

//==��������
void USBDInit(void);

#endif
