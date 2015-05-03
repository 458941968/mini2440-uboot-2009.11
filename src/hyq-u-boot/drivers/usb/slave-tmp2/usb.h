/****************************************************************
 NAME: usb.h
 DESC: definitions(USB data structure) for USB setup operation.
       Because It's h/w independent file, it may be used without any change in future.
       Reuse the source of S3C2400X u24xmon 
 HISTORY:
 Apr.07.2000:purnnamu: first release. 
 ****************************************************************/
#ifndef __USB_H__
#define __USB_H__

//************************
//       Endpoint 0      
//************************

// Standard bmRequestTyje (Direction) 
#define HOST_TO_DEVICE              (0x00)
#define DEVICE_TO_HOST              (0x80)    

// Standard bmRequestType (Type) 
#define STANDARD_TYPE               (0x00)
#define CLASS_TYPE                  (0x20)
#define VENDOR_TYPE                 (0x40)
#define RESERVED_TYPE               (0x60)

// Standard bmRequestType (Recipient) 
#define DEVICE_RECIPIENT            (0)
#define INTERFACE_RECIPIENT         (1)
#define ENDPOINT_RECIPIENT          (2)
#define OTHER_RECIPIENT             (3)

// Feature Selectors 
#define DEVICE_REMOTE_WAKEUP        (1)
#define EP_STALL                    (0)

// Standard Request Codes 
#define GET_STATUS                  (0)
#define CLEAR_FEATURE               (1)
#define SET_FEATURE                 (3)
#define SET_ADDRESS                 (5)
#define GET_DESCRIPTOR              (6)
#define SET_DESCRIPTOR              (7)
#define GET_CONFIGURATION           (8)
#define SET_CONFIGURATION           (9)
#define GET_INTERFACE               (10)
#define SET_INTERFACE               (11)
#define SYNCH_FRAME                 (12)

// Class-specific Request Codes 
#define GET_DEVICE_ID               (0)
#define GET_PORT_STATUS             (1)
#define SOFT_RESET                  (2)

// Descriptor Types
#define DEVICE_TYPE                 (1)
#define CONFIGURATION_TYPE          (2)
#define STRING_TYPE                 (3)
#define INTERFACE_TYPE              (4)
#define ENDPOINT_TYPE               (5)

//configuration descriptor: bmAttributes 
#define CONF_ATTR_DEFAULT	    (0x80) //Spec 1.0 it was BUSPOWERED bit.
#define CONF_ATTR_REMOTE_WAKEUP     (0x20)
#define CONF_ATTR_SELFPOWERED       (0x40)

//endpoint descriptor
#define EP_ADDR_IN		    (0x80)	
#define EP_ADDR_OUT		    (0x00)

#define EP_ATTR_CONTROL		    (0x0)	
#define EP_ATTR_ISOCHRONOUS	    (0x1)
#define EP_ATTR_BULK		    (0x2)
#define EP_ATTR_INTERRUPT	    (0x3)	


//string descriptor
#define LANGID_US_L 		    (0x09)  
#define LANGID_US_H 		    (0x04)


struct USB_SETUP_DATA{
    unsigned char bmRequestType;    
    unsigned char bRequest;         
    unsigned char bValueL;          
    unsigned char bValueH;          
    unsigned char bIndexL;          
    unsigned char bIndexH;          
    unsigned char bLengthL;         
    unsigned char bLengthH;         
};


struct USB_DEVICE_DESCRIPTOR{
    unsigned char bLength;    
    unsigned char bDescriptorType;         
    unsigned char bcdUSBL;
    unsigned char bcdUSBH;
    unsigned char bDeviceClass;          
    unsigned char bDeviceSubClass;          
    unsigned char bDeviceProtocol;          
    unsigned char bMaxPacketSize0;         
    unsigned char idVendorL;
    unsigned char idVendorH;
    unsigned char idProductL;
    unsigned char idProductH;
    unsigned char bcdDeviceL;
    unsigned char bcdDeviceH;
    unsigned char iManufacturer;
    unsigned char iProduct;
    unsigned char iSerialNumber;
    unsigned char bNumConfigurations;
};


struct USB_CONFIGURATION_DESCRIPTOR{
    unsigned char bLength;    
    unsigned char bDescriptorType;         
    unsigned char wTotalLengthL;
    unsigned char wTotalLengthH;
    unsigned char bNumInterfaces;
    unsigned char bConfigurationValue;
    unsigned char iConfiguration;
    unsigned char bmAttributes;
    unsigned char maxPower;          
};
    

struct USB_INTERFACE_DESCRIPTOR{
    unsigned char bLength;    
    unsigned char bDescriptorType;         
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    unsigned char bInterfaceClass;
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char iInterface;
};


struct USB_ENDPOINT_DESCRIPTOR{
    unsigned char bLength;    
    unsigned char bDescriptorType;         
    unsigned char bEndpointAddress;
    unsigned char bmAttributes;
    unsigned char wMaxPacketSizeL;
    unsigned char wMaxPacketSizeH;
    unsigned char bInterval;
};

 struct USB_CONFIGURATION_SET{
     unsigned char ConfigurationValue;
 };

 struct USB_GET_STATUS{
     unsigned char Device;
     unsigned char Interface;
     unsigned char Endpoint0;
     unsigned char Endpoint1;
     unsigned char Endpoint3;
 };

 struct USB_INTERFACE_GET{
     unsigned char AlternateSetting;
 }; 
#endif /*__USB_H__*/

