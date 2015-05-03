/*
 *************************************************************************
 *
 * USB HID ���� ����������
 *
 *************************************************************************
 */
#include <common.h>
#include "usblib.h"
#include "usbdhid.h"
#include "usbhid.h"
#include "usbkbd.h"


//****************************************************************************
//
// ����������
//
//****************************************************************************
unsigned char g_pucHIDKbdReportDescriptor[]=
{
    UsagePage(USB_HID_GENERIC_DESKTOP),
    Usage(USB_HID_KEYBOARD),
    Collection(USB_HID_APPLICATION),

        //
        // Modifier keys.
        // 8 - 1 bit values indicating the modifier keys (ctrl, shift...)
        //
        ReportSize(1),
        ReportCount(8),
        UsagePage(USB_HID_USAGE_KEYCODES),
        UsageMinimum(224),
        UsageMaximum(231),
        LogicalMinimum(0),
        LogicalMaximum(1),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),

        //
        // One byte of rsvd data required by HID spec.
        //
        ReportCount(1),
        ReportSize(8),
        Input(USB_HID_INPUT_CONSTANT),

        //
        // Keyboard LEDs.
        // 5 - 1 bit values.
        //
        ReportCount(5),
        ReportSize(1),
        UsagePage(USB_HID_USAGE_LEDS),
        UsageMinimum(1),
        UsageMaximum(5),
        Output(USB_HID_OUTPUT_DATA | USB_HID_OUTPUT_VARIABLE |
               USB_HID_OUTPUT_ABS),
        //
        // 1 - 3 bit value to pad out to a full byte.
        //
        ReportCount(1),
        ReportSize(3),
        Output(USB_HID_OUTPUT_CONSTANT), //LED report padding

        //
        // The Key buffer.
        // 6 - 8 bit values to store the current key state.
        //
        ReportCount(6),
        ReportSize(8),
        LogicalMinimum(0),
        LogicalMaximum(101),
        UsagePage(USB_HID_USAGE_KEYCODES),
        UsageMinimum (0),
        UsageMaximum (101),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_ARRAY),
    EndCollection
};
			
//****************************************************************************
//
// �ַ���������
//
//****************************************************************************
//==����ID
const unsigned char g_pLangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//==�����ַ���
const unsigned char g_pManufacturerString[] =
{
    (3 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'Y', 0, 'Q', 0
};

//==��Ʒ�ַ���
const unsigned char g_pProductString[] =
{
    (16 + 1) * 2,
    USB_DTYPE_STRING,
    'K', 0, 'e', 0, 'y', 0, 'b', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0,
    'E', 0, 'x', 0, 'a', 0, 'm', 0, 'p', 0, 'l', 0, 'e', 0
};

//==��Ʒ���к��ַ���
const unsigned char g_pSerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '0', 0, '0', 0, '0', 0, '2', 0
};

//==HID�ӿ��ַ���
const unsigned char g_pHIDInterfaceString[] =
{
    (22 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'K', 0, 'e', 0, 'y', 0, 'b', 0,
    'o', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0,
    'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

//==�����ַ���
const unsigned char g_pConfigString[] =
{
    (26 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'K', 0, 'e', 0, 'y', 0, 'b', 0,
    'o', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0,
    'f', 0, 'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0,
    'o', 0, 'n', 0
};
 
//==�ַ�����
const unsigned char * const g_pHIDKbdStringDescriptors[] =
{
    g_pLangDescriptor,
    g_pManufacturerString,
    g_pProductString,
    g_pSerialNumberString,
    g_pHIDInterfaceString,
    g_pConfigString
};


//****************************************************************************
//
// �豸������
//
//****************************************************************************
unsigned char g_pHIDKbdDeviceDescriptor[] =
{
    18,                         // Size of this structure.
    USB_DTYPE_DEVICE,           // Type of this structure.
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_DEVICE,           // USB Device Class
    0,                          // USB Device Sub-class
    0,      					// USB Device protocol
    8,                         // Maximum packet size for default pipe.
    USBShort(0x8888),           // Vendor ID (VID).
    USBShort(0x0001),           // Product ID (PID).
    USBShort(0x100),            // Device Version BCD.
    1,                          // Manufacturer string identifier.
    2,                          // Product string identifier.
    3,                          // Product serial number.
    1                           // Number of configurations.
};


//****************************************************************************
//
// ��������������
//
//****************************************************************************
//==����������
unsigned char g_pConfig[] =
{
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(34),               // The total size of this full structure.
    1,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    5,                          // The string identifier that describes this
                                // configuration.
    0xc0,     					// Bus Powered, Self Powered, remote wake up.
    0x25,                       // The maximum power in 2mA increments.
};

//==�ӿ�������
unsigned char g_pInterface[] =
{
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    0,                          // The index for this interface.
    0,                          // The alternate setting for this interface.
    1,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_HID,              // The interface class
    1,                          // The interface sub-class.
    1,                          // The interface protocol for the sub-class
                                // specified above.
    4,                          // The string index for this interface.
};

//==HID��������
unsigned char g_pHID[] =
{
	0x09,					    //==HID��������С
	0x21,						//==HID����������ֵ
	USBShort(0x111), 			//==HID�淶�汾
	0x21,						//==����ʶ���루������
	0x01,						//==�������������Ŀ
	0x22,						//==����������������
	sizeof(g_pucHIDKbdReportDescriptor)&0xff,			//==������������С��2�ֽڣ�
	(sizeof(g_pucHIDKbdReportDescriptor)>>8)&0xff,
};

//==����˵�������
unsigned char g_pInEndpoint[] =
{
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    USB_EP_DESC_IN | 0x01,
    USB_EP_ATTR_INT,            // Endpoint is an interrupt endpoint.
    USBShort(32), 				// The maximum packet size.
    16,                         // The polling interval for this endpoint.
};

//==���ö�
const tConfigSection g_sConfigSection =
{
    sizeof(g_pConfig),
    g_pConfig
};

//==�ӿڶ�
const tConfigSection g_sInterfaceSection =
{
    sizeof(g_pInterface),
    g_pInterface
};

//==HID��
const tConfigSection g_sHIDSection =
{
   sizeof(g_pHID),
    g_pHID
};

//==����˵��
const tConfigSection g_sInEndpointSection =
{
    sizeof(g_pInEndpoint),
    g_pInEndpoint
};


//==������
const tConfigSection *g_psSections[] =
{
    &g_sConfigSection,
    &g_sInterfaceSection,
    &g_sHIDSection,
    &g_sInEndpointSection
};

//==������
#define NUM_HID_SECTIONS (sizeof(g_psSections) /                         \
                          sizeof(tConfigSection *))

//==��һ������
tConfigHeader g_sConfigHeader =
{
    NUM_HID_SECTIONS,
    g_psSections
};

//==�����������飬Ŀǰֻ��һ������
const tConfigHeader * const g_pHIDKbdConfigDescriptors[] =
{
    &g_sConfigHeader
};


