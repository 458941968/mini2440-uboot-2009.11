/*
 *************************************************************************
 *
 * USB HID 键盘 描述符声明
 *
 *************************************************************************
 */
 #ifndef __USBDKBD_H__
#define  __USBDKBD_H__


#define USB_DTYPE_REPORT		0x22	 				//==报告描述符类型码
//==声明字符串描述符，配置描述符，设备描述符，报告描述符
extern const unsigned char * const g_pHIDKbdStringDescriptors[];
extern const tConfigHeader * const g_pHIDKbdConfigDescriptors[];
extern unsigned char g_pHIDKbdDeviceDescriptor[];
extern unsigned char g_pucHIDKbdReportDescriptor[];
extern unsigned char g_pHID[];

#endif
