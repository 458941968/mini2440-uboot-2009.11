/*
 *************************************************************************
 *
 * USBת��������������
 *
 *************************************************************************
 */
#include <common.h>
#include "usblib.h"
#include "usbcdc.h"
#include "usb2uart.h"
#include "2440usb.h"


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
    2 + (16 * 2),
    USB_DTYPE_STRING,
    'V', 0, 'i', 0, 'r', 0, 't', 0, 'u', 0, 'a', 0, 'l', 0, ' ', 0,
    'C', 0, 'O', 0, 'M', 0, ' ', 0, 'P', 0, 'o', 0, 'r', 0, 't', 0
};

//==��Ʒ���к��ַ���
const unsigned char g_pSerialNumberString[] =
{
    (4 + 1) * 2,
    USB_DTYPE_STRING,
    '0', 0, '0', 0, '0', 0, '3', 0
};


//==���ƽӿ��ַ���
const unsigned char g_pControlInterfaceString[] =
{
    2 + (21 * 2),
    USB_DTYPE_STRING,
    'A', 0, 'C', 0, 'M', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 't', 0,
    'r', 0, 'o', 0, 'l', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0,
    'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

//==�����ַ���
const unsigned char g_pConfigString[] =
{
    2 + (26 * 2),
    USB_DTYPE_STRING,
    'S', 0, 'e', 0, 'l', 0, 'f', 0, ' ', 0, 'P', 0, 'o', 0, 'w', 0,
    'e', 0, 'r', 0, 'e', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0,
    'f', 0, 'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0,
    'o', 0, 'n', 0
};
 
//==�ַ�����
const unsigned char * const g_pCDCSerStringDescriptors[] =
{
    g_pLangDescriptor,
    g_pManufacturerString,
    g_pProductString,
    g_pSerialNumberString,
    g_pControlInterfaceString,
    g_pConfigString
};


//****************************************************************************
//
// �豸������
//
//****************************************************************************
unsigned char g_pCDCSerDeviceDescriptor[] =
{
    18,                         //����������
    USB_DTYPE_DEVICE,           // ����������
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_CDC,           // USB Device Class
    0,                          // USB Device Sub-class
    0,      					// USB Device protocol
    8,                         // Maximum packet size for default pipe.
    USBShort(0x8888),           // Vendor ID (VID).
    USBShort(0x0003),           // Product ID (PID).
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
unsigned char g_pCDCSerDescriptor[] =
{
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(0x43),               // The total size of this full structure.
    2,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    5,                          // The string identifier that describes this
                                // configuration.
    0xc0,     					// Bus Powered, Self Powered, remote wake up.
    0x25,                       // The maximum power in 2mA increments.
};

//==���ö�
const tConfigSection g_sCDCSerConfigSection =
{
    sizeof(g_pCDCSerDescriptor),
    g_pCDCSerDescriptor
};

//==CDC��ӿ�������9+5+4+5+5+7 = 35
const unsigned char g_pCDCSerCommInterface[] =
{
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    0,   						// The index for this interface.
    0,                          // The alternate setting for this interface.
    1,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_CDC,              // The interface class constant defined by
                                // USB-IF (spec 5.1.3).
    USB_CDC_SUBCLASS_ABSTRACT_MODEL,    // The interface sub-class constant
                                        // defined by USB-IF (spec 5.1.3).
    USB_CDC_PROTOCOL_V25TER,    // The interface protocol for the sub-class
                                // specified above.
    4,                          // The string index for this interface.

    //
    // Communication Class Interface Functional Descriptor - Header
    //
    5,                          // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,       // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_HEADER,  // Header functional descriptor
    USBShort(0x110),            // Complies with CDC version 1.1
    
    //
    // Communication Class Interface Functional Descriptor - ACM
    //
    4,                          // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,       // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_ABSTRACT_CTL_MGMT,
    USB_CDC_ACM_SUPPORTS_LINE_PARAMS | USB_CDC_ACM_SUPPORTS_SEND_BREAK,

    //
    // Communication Class Interface Functional Descriptor - Unions
    //
    5,                          // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,       // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_UNION,
    0,
    1,      					// Data interface number
    
    //
    // Communication Class Interface Functional Descriptor - Call Management
    //
    5,                          // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,       // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_CALL_MGMT,
    USB_CDC_CALL_MGMT_HANDLED,
    1,      					// Data interface number

    //
    // Endpoint Descriptor (interrupt, IN)
    //
    7,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
    USB_EP_DESC_IN | (0x1),
    USB_EP_ATTR_INT,                // Endpoint is an interrupt endpoint.
    USBShort(32),   				// The maximum packet size.
    0x0a                               // The polling interval for this endpoint.
};

//==CDC��ӿڶ�
const tConfigSection g_sCDCSerCommInterfaceSection =
{
    sizeof(g_pCDCSerCommInterface),
    g_pCDCSerCommInterface
};

//==������ӿ�������9+7+7 = 23
const unsigned char g_pCDCSerDataInterface[] =
{
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    1,      					// The index for this interface.
    0,                          // The alternate setting for this interface.
    2,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_CDC_DATA,         // The interface class constant defined by
                                // USB-IF (spec 5.1.3).
    0,                          // The interface sub-class constant
                                // defined by USB-IF (spec 5.1.3).
    USB_CDC_PROTOCOL_NONE,      // The interface protocol for the sub-class
                                // specified above.
    0,                          // The string index for this interface.

  	//==��������˵�2
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    USB_EP_DESC_IN | (0x2),
    USB_EP_ATTR_BULK,           // Endpoint is a bulk endpoint.
    USBShort(32),  				// The maximum packet size.
    0,                          // The polling interval for this endpoint.

	//==��������˵�3
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    USB_EP_DESC_OUT | (0x3),
    USB_EP_ATTR_BULK,           // Endpoint is a bulk endpoint.
    USBShort(32), 				// The maximum packet size.
    0,                          // The polling interval for this endpoint.
};

//==������ӿڶ�
const tConfigSection g_sCDCSerDataInterfaceSection =
{
    sizeof(g_pCDCSerDataInterface),
    g_pCDCSerDataInterface
};


//==�����飬�������Σ����öΣ�CDC��ӿڶΣ�������ӿڶ�
const tConfigSection *g_psCDCSerSections[] =
{
    &g_sCDCSerConfigSection,
    &g_sCDCSerCommInterfaceSection,
    &g_sCDCSerDataInterfaceSection
};

#define NUM_CDCSER_SECTIONS (sizeof(g_psCDCSerSections) /                     \
                             sizeof(tConfigSection *))

//==��һ������
const tConfigHeader g_sCDCSerConfigHeader =
{
    NUM_CDCSER_SECTIONS,
    g_psCDCSerSections
};

//==�����������飬Ŀǰֻ��һ������
const tConfigHeader * const g_pCDCSerConfigDescriptors[] =
{
    &g_sCDCSerConfigHeader
};



/********************************************************************
�������ܣ������ַ��������⴮�ڡ�
��ڲ�����pucBuffer:�ַ�����������ulCount:�ַ�������
��    �أ����͵��ַ�������
��    ע���ޡ�
********************************************************************/
unsigned long Usb2UartSendData(unsigned char *pucBuffer, unsigned long ulCount)
{
	unsigned long ulSendLength;

	ulSendLength = ulCount;

	//==�������һ����
    while(ulSendLength>EP2_PKT_SIZE)
    {
		USBEndpointDataPut (USB_EP_2, pucBuffer, EP2_PKT_SIZE);
		USBEndpointDataSend (USB_EP_2,USB_FLAG_TX);
		ulSendLength -= EP2_PKT_SIZE;
		pucBuffer += EP2_PKT_SIZE;
    }
	
	//==���һ����	
	USBEndpointDataPut(USB_EP_2, pucBuffer, ulSendLength);
	USBEndpointDataSend(USB_EP_2,USB_FLAG_TX);

	return ulCount;
}

