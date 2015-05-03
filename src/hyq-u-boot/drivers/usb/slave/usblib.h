//*****************************************************************************
//
//*****************************************************************************

#ifndef __USBLIB_H__
#define __USBLIB_H__


#ifdef __cplusplus
extern "C"
{
#endif


#define USBLIB_NUM_EP           16   // Number of supported endpoints.

#define gcc

#if defined(ccs) ||             \
    defined(codered) ||         \
    defined(gcc) ||             \
    defined(rvmdk) ||           \
    defined(__ARMCC_VERSION) || \
    defined(sourcerygxx)
#define PACKED __attribute__ ((packed))
#elif defined(ewarm)
#define PACKED
#else
#error Unrecognized COMPILER!
#endif
//*****************************************************************************
//
// Assorted language IDs from the document "USB_LANGIDs.pdf" provided by the
// USB Implementers' Forum (Version 1.0).
//
//*****************************************************************************
#define USB_LANG_CHINESE_PRC    0x0804      // Chinese (PRC)
#define USB_LANG_CHINESE_TAIWAN 0x0404      // Chinese (Taiwan)
#define USB_LANG_EN_US          0x0409      // English (United States)
#define USB_LANG_EN_UK          0x0809      // English (United Kingdom)
#define USB_LANG_EN_AUS         0x0C09      // English (Australia)
#define USB_LANG_EN_CA          0x1009      // English (Canada)
#define USB_LANG_EN_NZ          0x1409      // English (New Zealand)
#define USB_LANG_FRENCH         0x040C      // French (Standard)
#define USB_LANG_GERMAN         0x0407      // German (Standard)
#define USB_LANG_HINDI          0x0439      // Hindi
#define USB_LANG_ITALIAN        0x0410      // Italian (Standard)
#define USB_LANG_JAPANESE       0x0411      // Japanese
#define USB_LANG_KOREAN         0x0412      // Korean
#define USB_LANG_ES_TRAD        0x040A      // Spanish (Traditional)
#define USB_LANG_ES_MODERN      0x0C0A      // Spanish (Modern)
#define USB_LANG_SWAHILI        0x0441      // Swahili (Kenya)
#define USB_LANG_URDU_IN        0x0820      // Urdu (India)
#define USB_LANG_URDU_PK        0x0420      // Urdu (Pakistan)


//*****************************************************************************
//
//! The standard USB request header as defined in section 9.3 of the USB 2.0
//! specification.
//
//*****************************************************************************
typedef struct
{
    unsigned char bmRequestType;
    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
}
PACKED tUSBRequest;

//*****************************************************************************
//
// The following defines are used with the bmRequestType member of tUSBRequest.
//
// Request types have 3 bit fields:
// 4:0 - Is the recipient type.
// 6:5 - Is the request type.
// 7 - Is the direction of the request.
//
//*****************************************************************************
#define USB_RTYPE_DIR_IN        0x80
#define USB_RTYPE_DIR_OUT       0x00

#define USB_RTYPE_TYPE_M        0x60
#define USB_RTYPE_VENDOR        0x40
#define USB_RTYPE_CLASS         0x20
#define USB_RTYPE_STANDARD      0x00

#define USB_RTYPE_RECIPIENT_M   0x1f
#define USB_RTYPE_OTHER         0x03
#define USB_RTYPE_ENDPOINT      0x02
#define USB_RTYPE_INTERFACE     0x01
#define USB_RTYPE_DEVICE        0x00

//*****************************************************************************
//
// Standard USB requests IDs used in the bRequest field of tUSBRequest.
//
//*****************************************************************************
#define USBREQ_GET_STATUS       0x00
#define USBREQ_CLEAR_FEATURE    0x01
#define USBREQ_SET_FEATURE      0x03
#define USBREQ_SET_ADDRESS      0x05
#define USBREQ_GET_DESCRIPTOR   0x06
#define USBREQ_SET_DESCRIPTOR   0x07
#define USBREQ_GET_CONFIG       0x08
#define USBREQ_SET_CONFIG       0x09
#define USBREQ_GET_INTERFACE    0x0a
#define USBREQ_SET_INTERFACE    0x0b
#define USBREQ_SYNC_FRAME       0x0c

//*****************************************************************************
//
// Data returned from a USBREQ_GET_STATUS request to a device.
//
//*****************************************************************************
#define USB_STATUS_SELF_PWR     0x0001  // Currently self powered.
#define USB_STATUS_BUS_PWR      0x0000  // Currently bus-powered.
#define USB_STATUS_PWR_M        0x0001  // Mask for power mode.
#define USB_STATUS_REMOTE_WAKE  0x0002  // Remote wake-up is currently enabled.

//*****************************************************************************
//
// Feature Selectors (tUSBRequest.wValue) passed on USBREQ_CLEAR_FEATURE and
// USBREQ_SET_FEATURE.
//
//*****************************************************************************
#define USB_FEATURE_EP_HALT     0x0000  // Endpoint halt feature.
#define USB_FEATURE_REMOTE_WAKE 0x0001  // Remote wake feature, device only.
#define USB_FEATURE_TEST_MODE   0x0002  // Test mode

//*****************************************************************************
//
// Endpoint Selectors (tUSBRequest.wIndex) passed on USBREQ_CLEAR_FEATURE,
// USBREQ_SET_FEATURE and USBREQ_GET_STATUS.
//
//*****************************************************************************
#define USB_REQ_EP_NUM_M   0x007F
#define USB_REQ_EP_DIR_M   0x0080
#define USB_REQ_EP_DIR_IN  0x0080
#define USB_REQ_EP_DIR_OUT 0x0000

//*****************************************************************************
//
// Standard USB descriptor types.  These values are passed in the upper bytes
// of tUSBRequest.wValue on USBREQ_GET_DESCRIPTOR and also appear in the
// bDescriptorType field of standard USB descriptors.
//
//*****************************************************************************
#define USB_DTYPE_DEVICE        1
#define USB_DTYPE_CONFIGURATION 2
#define USB_DTYPE_STRING        3
#define USB_DTYPE_INTERFACE     4
#define USB_DTYPE_ENDPOINT      5
#define USB_DTYPE_DEVICE_QUAL   6
#define USB_DTYPE_OSPEED_CONF   7
#define USB_DTYPE_INTERFACE_PWR 8
#define USB_DTYPE_OTG           9
#define USB_DTYPE_INTERFACE_ASC 11
#define USB_DTYPE_CS_INTERFACE  36

//*****************************************************************************
// ÃèÊö·ûÍ·
//
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
}
PACKED tDescriptorHeader;

//*****************************************************************************
// Éè±¸ÃèÊö·û
//
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdUSB;
    unsigned char bDeviceClass;
    unsigned char bDeviceSubClass;
    unsigned char bDeviceProtocol;
    unsigned char bMaxPacketSize0;
    unsigned short idVendor;
    unsigned short idProduct;
    unsigned short bcdDevice;
    unsigned char iManufacturer;
    unsigned char iProduct;
    unsigned char iSerialNumber;
    unsigned char bNumConfigurations;
}
PACKED tDeviceDescriptor;

//*****************************************************************************
//
// USB Device Class codes used in the tDeviceDescriptor.bDeviceClass field.
// Definitions for the bDeviceSubClass and bDeviceProtocol fields are device
// specific and can be found in the appropriate device class header files.
//
//*****************************************************************************
#define USB_CLASS_DEVICE        0x00
#define USB_CLASS_AUDIO         0x01
#define USB_CLASS_CDC           0x02
#define USB_CLASS_HID           0x03
#define USB_CLASS_PHYSICAL      0x05
#define USB_CLASS_IMAGE         0x06
#define USB_CLASS_PRINTER       0x07
#define USB_CLASS_MASS_STORAGE  0x08
#define USB_CLASS_HUB           0x09
#define USB_CLASS_CDC_DATA      0x0a
#define USB_CLASS_SMART_CARD    0x0b
#define USB_CLASS_SECURITY      0x0d
#define USB_CLASS_VIDEO         0x0e
#define USB_CLASS_HEALTHCARE    0x0f
#define USB_CLASS_DIAG_DEVICE   0xdc
#define USB_CLASS_WIRELESS      0xe0
#define USB_CLASS_MISC          0xef
#define USB_CLASS_APP_SPECIFIC  0xfe
#define USB_CLASS_VEND_SPECIFIC 0xff
#define USB_CLASS_EVENTS        0xffffffff

//*****************************************************************************
//
// Generic values for undefined subclass and protocol.
//
//*****************************************************************************
#define USB_SUBCLASS_UNDEFINED  0x00
#define USB_PROTOCOL_UNDEFINED  0x00

//*****************************************************************************
//
// The following are the miscellaneous subclass values.
//
//*****************************************************************************
#define USB_MISC_SUBCLASS_SYNC      0x01
#define USB_MISC_SUBCLASS_COMMON    0x02

//*****************************************************************************
//
// These following are miscellaneous protocol values.
//
//*****************************************************************************
#define USB_MISC_PROTOCOL_IAD   0x01

//*****************************************************************************
//
//! This structure describes the USB device qualifier descriptor as defined in
//! the USB 2.0 specification, section 9.6.2.
//
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdUSB;
    unsigned char bDeviceClass;
    unsigned char bDeviceSubClass;
    unsigned char bDeviceProtocol;
    unsigned char bMaxPacketSize0;
    unsigned char bNumConfigurations;
    unsigned char bReserved;
}
PACKED tDeviceQualifierDescriptor;

//*****************************************************************************
// ÅäÖÃÃèÊö·û
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short wTotalLength;
    unsigned char bNumInterfaces;
    unsigned char bConfigurationValue;
    unsigned char iConfiguration;
    unsigned char bmAttributes;
    unsigned char bMaxPower;
}
PACKED tConfigDescriptor;

//*****************************************************************************
//
// Flags used in constructing the value assigned to the field
// tConfigDescriptor.bmAttributes.  Note that bit 7 is reserved and must be set
// to 1.
//
//*****************************************************************************
#define USB_CONF_ATTR_PWR_M     0xC0

#define USB_CONF_ATTR_SELF_PWR  0xC0
#define USB_CONF_ATTR_BUS_PWR   0x80
#define USB_CONF_ATTR_RWAKE     0xA0

//*****************************************************************************
// ½Ó¿ÚÃèÊö·û
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    unsigned char bInterfaceClass;
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char iInterface;
}
PACKED tInterfaceDescriptor;

//*****************************************************************************
// ¶ËµãÃèÊö·û
//
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bEndpointAddress;
    unsigned char bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char bInterval;
}
PACKED tEndpointDescriptor;

//*****************************************************************************
//
// Flags used in constructing the value assigned to the field
// tEndpointDescriptor.bEndpointAddress.
//
//*****************************************************************************
#define USB_EP_DESC_OUT                 0x00
#define USB_EP_DESC_IN                  0x80
#define USB_EP_DESC_NUM_M               0x0f

//*****************************************************************************
//
// Mask used to extract the maximum packet size (in bytes) from the
// wMaxPacketSize field of the endpoint descriptor.
//
//*****************************************************************************
#define USB_EP_MAX_PACKET_COUNT_M       0x07FF

//*****************************************************************************
//
// Endpoint attributes used in tEndpointDescriptor.bmAttributes.
//
//*****************************************************************************
#define USB_EP_ATTR_CONTROL             0x00
#define USB_EP_ATTR_ISOC                0x01
#define USB_EP_ATTR_BULK                0x02
#define USB_EP_ATTR_INT                 0x03
#define USB_EP_ATTR_TYPE_M              0x03

#define USB_EP_ATTR_ISOC_M              0x0c
#define USB_EP_ATTR_ISOC_NOSYNC         0x00
#define USB_EP_ATTR_ISOC_ASYNC          0x04
#define USB_EP_ATTR_ISOC_ADAPT          0x08
#define USB_EP_ATTR_ISOC_SYNC           0x0c
#define USB_EP_ATTR_USAGE_M             0x30
#define USB_EP_ATTR_USAGE_DATA          0x00
#define USB_EP_ATTR_USAGE_FEEDBACK      0x10
#define USB_EP_ATTR_USAGE_IMPFEEDBACK   0x20

//*****************************************************************************
//
//! This structure describes the USB string descriptor for index 0 as defined
//! in USB 2.0 specification section 9.6.7.  Note that the number of language
//! IDs is variable and can be determined by examining bLength.  The number of
//! language IDs present in the descriptor is given by ((bLength - 2) / 2).
//
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short wLANGID[1];
}
PACKED tString0Descriptor;

//*****************************************************************************
//
//! This structure describes the USB string descriptor for all string indexes
//! other than 0 as defined in USB 2.0 specification section 9.6.7.
//
//*****************************************************************************
typedef struct
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bString;
}
PACKED tStringDescriptor;


#define USBShort(usValue)       (usValue & 0xff), (usValue >> 8)


//*****************************************************************************
//
// Function prototype for any standard USB request.
//
//*****************************************************************************
typedef void (* tStdRequest)(void *pvInstance, tUSBRequest *pUSBRequest);



//*****************************************************************************
//
//! This structure defines a contiguous block of data which contains a group
//! of descriptors that form part of a configuration descriptor for a device.
//! It is assumed that a config section contains only whole descriptors.  It is
//! not valid to split a single descriptor across multiple sections.
//!
//*****************************************************************************
typedef struct
{
    unsigned short usSize;
    const unsigned char *pucData;
}
tConfigSection;

//*****************************************************************************
//
//! This is the top level structure defining a USB device configuration
//! descriptor.  A configuration descriptor contains a collection of device-
//! specific descriptors in addition to the basic config, interface and
//! endpoint descriptors.  To allow flexibility in constructing the
//! configuration, the descriptor is described in terms of a list of data
//! blocks.  The first block must contain the configuration descriptor itself
//! and the following blocks are appended to this in order to produce the
//! full descriptor sent to the host in response to a GetDescriptor request
//! for the configuration descriptor.
//!
//*****************************************************************************
typedef struct
{
    unsigned char ucNumSections;
    const tConfigSection * const *psSections;
}
tConfigHeader;


//*****************************************************************************
// Éè±¸ÐÅÏ¢
//*****************************************************************************
typedef struct
{
    const unsigned char *pDeviceDescriptor;
    const tConfigHeader * const *ppConfigDescriptors;
    const unsigned char * const *ppStringDescriptors;
    unsigned long ulNumStringDescriptors;
    void *pvInstance;
}
tDeviceInfo;


//*****************************************************************************
// ¶Ëµã0×´Ì¬
//*****************************************************************************
typedef enum
{
    USB_STATE_IDLE,
    USB_STATE_TX,
    USB_STATE_TX_CONFIG,
    USB_STATE_RX,
    USB_STATE_STATUS,
    USB_STATE_STALL
}
tEP0State;

//*****************************************************************************
// Éè±¸ÊµÀý
//*****************************************************************************
typedef struct
{
    tDeviceInfo *psInfo;
    void *pvInstance;
    volatile tEP0State eEP0State;
    volatile unsigned long ulDevAddress;
    unsigned long ulConfiguration;
    unsigned long ulDefaultConfiguration;
    unsigned char *pEP0Data;
    volatile unsigned long ulEP0DataRemain;
    unsigned long ulOUTDataSize;
    unsigned char ucStatus;
    unsigned char ucConfigSection;
    unsigned char ucSectionOffset;
    unsigned char ucConfigIndex;
}
tDeviceInstance;


#ifdef __cplusplus
}
#endif

#endif // __USBLIB_H__
