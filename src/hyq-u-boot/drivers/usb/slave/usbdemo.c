/*
 *************************************************************************
 *
 * USB DEMO 描述符
 *
 *************************************************************************
 */
#include <common.h>
#include "usblib.h"
#include "usbdemo.h"
 
			
//****************************************************************************
//
// 字符串描述符
//
//****************************************************************************
//==语言ID
const unsigned char g_pLangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//==厂商字符串
const unsigned char g_pManufacturerString[] =
{
    (0x14 + 2),
    USB_DTYPE_STRING,
	'S',0x0,'y',0x0,'s',0x0,'t',0x0,'e',0x0,'m',0x0,' ',0x0,'M',0x0,
	'C',0x0,'U',0x0,
};

//==产品字符串
const unsigned char g_pProductString[] =
{
    (0x2a + 2),
    USB_DTYPE_STRING,
    'S',0x0,'E',0x0,'C',0x0,' ',0x0,'S',0x0,'3',0x0,'C',0x0,'2',0x0,
    '4',0x0,'1',0x0,'0',0x0,'X',0x0,' ',0x0,'T',0x0,'e',0x0,'s',0x0,
    't',0x0,' ',0x0,'B',0x0,'/',0x0,'D',0x0
};


//==字符串表
const unsigned char * const g_pDemoStringDescriptors[] =
{
    g_pLangDescriptor,
    g_pManufacturerString,
    g_pProductString,
};


//****************************************************************************
//
// 设备描述符
//
//****************************************************************************
unsigned char g_pDemoDeviceDescriptor[] =
{
    18,                         // Size of this structure.
    USB_DTYPE_DEVICE,           // Type of this structure.
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    0xff,//USB_CLASS_DEVICE,           // USB Device Class
    0,                          // USB Device Sub-class
    0,      					// USB Device protocol
    8,                         // Maximum packet size for default pipe.
    USBShort(0x5345),           // Vendor ID (VID).
    USBShort(0x1234),           // Product ID (PID).
    USBShort(0x100),            // Device Version BCD.
    1,                          // Manufacturer string identifier.
    2,                          // Product string identifier.
    0,                          // Product serial number.
    1                           // Number of configurations.
};


//****************************************************************************
//
// 配置描述符集合
//
//****************************************************************************
//==配置描述符
unsigned char g_pConfig[] =
{
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(0x20),               // The total size of this full structure.
    1,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    0,                          // The string identifier that describes this
                                // configuration.
    0xc0,     					// Bus Powered, Self Powered, remote wake up.
    25,                       // The maximum power in 2mA increments.
};

//==接口描述符
unsigned char g_pInterface[] =
{
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    0,                          // The index for this interface.
    0,                          // The alternate setting for this interface.
    2,                          // The number of endpoints used by this
                                // interface.
    0xff,//USB_CLASS_HID,              // The interface class
    0,                          // The interface sub-class.
    0,                          // The interface protocol for the sub-class
                                // specified above.
    0,                          // The string index for this interface.
};

//==输入端点描述符
unsigned char g_pInEndpoint[] =
{
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    0x81,
    USB_EP_ATTR_INT,            // Endpoint is an interrupt endpoint.
    USBShort(32), 				// The maximum packet size.
    0,                         // The polling interval for this endpoint.
};

//==输出端点描述符
unsigned char g_pOutEndpoint[] =
{
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    0x01,
    USB_EP_ATTR_INT,            // Endpoint is an interrupt endpoint.
    USBShort(32), 				// The maximum packet size.
    0,                         // The polling interval for this endpoint.
};

//==配置段
const tConfigSection g_sConfigSection =
{
    sizeof(g_pConfig),
    g_pConfig
};

//==接口段
const tConfigSection g_sInterfaceSection =
{
    sizeof(g_pInterface),
    g_pInterface
};

//==输入端点段
const tConfigSection g_sInEndpointSection =
{
    sizeof(g_pInEndpoint),
    g_pInEndpoint
};

//==输出端点段
const tConfigSection g_sOutEndpointSection =
{
    sizeof(g_pOutEndpoint),
    g_pOutEndpoint
};


//==段数组
const tConfigSection *g_psSections[] =
{
    &g_sConfigSection,
    &g_sInterfaceSection,
    &g_sInEndpointSection,
    &g_sOutEndpointSection
};

//==段数量
#define NUM_HID_SECTIONS (sizeof(g_psSections) /                         \
                          sizeof(tConfigSection *))

//==第一个配置
tConfigHeader g_sConfigHeader =
{
    NUM_HID_SECTIONS,
    g_psSections
};

//==配置描述符组，目前只有一个配置
const tConfigHeader * const g_pDemoConfigDescriptors[] =
{
    &g_sConfigHeader
};

