/*
 *************************************************************************
 *
 * USB HID ���� ����������
 *
 *************************************************************************
 */
 #ifndef __USBDKBD_H__
#define  __USBDKBD_H__


#define USB_DTYPE_REPORT		0x22	 				//==����������������
//==�����ַ������������������������豸������������������
extern const unsigned char * const g_pHIDKbdStringDescriptors[];
extern const tConfigHeader * const g_pHIDKbdConfigDescriptors[];
extern unsigned char g_pHIDKbdDeviceDescriptor[];
extern unsigned char g_pucHIDKbdReportDescriptor[];
extern unsigned char g_pHID[];

#endif
