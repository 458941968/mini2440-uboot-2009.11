/*
 *************************************************************************
 *
 * USBת��������������
 *
 *************************************************************************
 */
#ifndef __USBUART_H__
#define  __USBUART_H__

extern const unsigned char * const g_pCDCSerStringDescriptors[];
extern const tConfigHeader * const g_pCDCSerConfigDescriptors[];
extern unsigned char g_pCDCSerDeviceDescriptor[];
//==��������
unsigned long Usb2UartSendData( unsigned char *pucBuffer, unsigned long ulCount);

#endif
