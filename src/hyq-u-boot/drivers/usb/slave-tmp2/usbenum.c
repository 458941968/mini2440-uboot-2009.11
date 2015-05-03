/**************************************************************
 NAME: usbsetup.c
 DESC: process the USB setup stage operations.
 HISTORY:
 MAR.25.2002:purnnamu: S3C2400X usbsetup.c is ported for S3C2410X.
 AUG.20.2002:purnnamu: rEP0_CSR should be used instead of rOUT_CSR1_REG for EP0 macros.
 **************************************************************/
#include <common.h>
#if defined(CONFIG_S3C2400)
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#elif defined(CONFIG_S3C2440)
#include <s3c2440.h>
#endif

#include <s3c24x0.h>

#include "usb.h"
#include "2440usb.h"
#include "usbenum.h"

// *** End point information ***
//   EP0: control
//   EP1: bulk in end point
//   EP2: not used
//   EP3: bulk out end point
//   EP4: not used

// *** VERY IMPORTANT NOTE ***
// Every descriptor size of EP0 should be 8n+m(m=1~7).
// Otherwise, USB will not operate normally because the program
// doesn't prepare the case that the descriptor size is 8n+0.
// If the size of a descriptor is 8n, the 0 length packit should be sent. 
// Special thanks to E.S.Choi for reminding me of this USB specification.


// ===================================================================
// All following commands will operate only in case 
// - ep0_csr is valid.
// ===================================================================
#define CLR_EP0_OUT_PKT_RDY() 		usbdevregs->EP0_CSR_IN_CSR1_REG=((ep0_csr & (~EP0_WR_BITS)) | EP0_SERVICED_OUT_PKT_RDY )	 
#define CLR_EP0_OUTPKTRDY_DATAEND() 	usbdevregs->EP0_CSR_IN_CSR1_REG=((ep0_csr & (~EP0_WR_BITS)) | (EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END))
#define SET_EP0_IN_PKT_RDY() 		usbdevregs->EP0_CSR_IN_CSR1_REG=((ep0_csr & (~EP0_WR_BITS)) | (EP0_IN_PKT_READY))	 
#define SET_EP0_INPKTRDY_DATAEND() 	usbdevregs->EP0_CSR_IN_CSR1_REG=((ep0_csr & (~EP0_WR_BITS)) | (EP0_IN_PKT_READY|EP0_DATA_END))			
#define CLR_EP0_SETUP_END() 		usbdevregs->EP0_CSR_IN_CSR1_REG=((ep0_csr & (~EP0_WR_BITS)) | (EP0_SERVICED_SETUP_END))
#define CLR_EP0_SENT_STALL() 		usbdevregs->EP0_CSR_IN_CSR1_REG=((ep0_csr & (~EP0_WR_BITS)) & (~EP0_SENT_STALL))
#define FLUSH_EP0_FIFO() 		{ while (usbdevregs->OUT_FIFO_CNT1_REG) { usbdevregs->fifo[0].EP_FIFO_REG; }}

unsigned int ep0State;
unsigned int ep0SubState;


struct USB_SETUP_DATA descSetup;			//==ÇëÇóÍ·

struct USB_DEVICE_DESCRIPTOR descDev;			//==Éè±¸ÃèÊö·û
struct USB_CONFIGURATION_DESCRIPTOR descConf;	//==ÅäÖÃÃèÊö·û
struct USB_INTERFACE_DESCRIPTOR descIf;			//==½Ó¿ÚÃèÊö·û
struct USB_ENDPOINT_DESCRIPTOR descEndpt0;		//==¶Ëµã0ÃèÊö·û
struct USB_ENDPOINT_DESCRIPTOR descEndpt1;		//==¶Ëµã1ÃèÊö·û



static const unsigned char descStr0[]={
	4,STRING_TYPE,LANGID_US_L,LANGID_US_H,  //codes representing languages
    };

static const unsigned char descStr1[]={  //Manufacturer  
	(0x14+2),STRING_TYPE, 
	'S',0x0,'y',0x0,'s',0x0,'t',0x0,'e',0x0,'m',0x0,' ',0x0,'M',0x0,
	'C',0x0,'U',0x0,
    };
    
static const unsigned char descStr2[]={  //Product  
    (0x2a+2),STRING_TYPE, 
    'S',0x0,'E',0x0,'C',0x0,' ',0x0,'S',0x0,'3',0x0,'C',0x0,'2',0x0,
    '4',0x0,'1',0x0,'0',0x0,'X',0x0,' ',0x0,'T',0x0,'e',0x0,'s',0x0,
    't',0x0,' ',0x0,'B',0x0,'/',0x0,'D',0x0
    };



#if 1
void DbgPrintf(char *fmt,...)
{
    int i,slen;
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    
//    slen=strlen(string);
    
//    for(i=0;i<slen;i++)
//    	_WrDbgStrFifo(string[i]);
    
    va_end(ap);
    puts(string);
}
#else
void DbgPrintf(char *fmt,...)
{
}
#endif


void Ep0Handler(void)
{
    static int ep0SubState;
    unsigned char ep0_csr;
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();

    usbdevregs->INDEX_REG=0;
    ep0_csr=usbdevregs->EP0_CSR_IN_CSR1_REG;
    
//    DbgPrintf("<0:%x]\n",ep0_csr);

    //DATAEND interrupt(ep0_csr==0x0) will be ignored 
    //because ep0State==EP0_STATE_INIT when the DATAEND interrupt is issued.
    if(ep0_csr & EP0_SETUP_END)
    {   
		CLR_EP0_SETUP_END();	
		if(ep0_csr & EP0_OUT_PKT_READY) 
		{ 
		    CLR_EP0_OUT_PKT_RDY();
		}
		ep0State=EP0_STATE_INIT;
		return;
    }	

    //I think that EP0_SENT_STALL will not be set to 1.
    if(ep0_csr & EP0_SENT_STALL)
    {   
	   	DbgPrintf("[STALL]");
	   	CLR_EP0_SENT_STALL();
		if(ep0_csr & EP0_OUT_PKT_READY) 
		{
		    CLR_EP0_OUT_PKT_RDY();
		}
		ep0State=EP0_STATE_INIT;
		return;
    }	

    if((ep0_csr & EP0_OUT_PKT_READY)) // && (ep0State==EP0_STATE_INIT))
    {	
		RdPktEp0((unsigned char *)&descSetup, EP0_PKT_SIZE);
    
		switch(descSetup.bRequest)
    	{
    	case GET_DESCRIPTOR:
            switch(descSetup.bValueH)        
            {
            case DEVICE_TYPE:
	 	    //	DbgPrintf("[GDD]");
	 	    	CLR_EP0_OUT_PKT_RDY();
		    	ep0State=EP0_STATE_GD_DEV_0;	        
		    	break;	
		    	
		    case CONFIGURATION_TYPE:
	 	    	DbgPrintf("[GDC]");
	 	    	CLR_EP0_OUT_PKT_RDY();
	 	    	if((descSetup.bLengthL+(descSetup.bLengthH<<8))>0x9)	
		    	   ep0State=EP0_STATE_GD_CFG_0; //for WIN98,WIN2K
	            else	    	    
	  		    	ep0State=EP0_STATE_GD_CFG_ONLY_0; //for WIN2K
		    	break;
		    	
	   	    case STRING_TYPE:
	 	    	DbgPrintf("[GDS]");
	 	    	CLR_EP0_OUT_PKT_RDY();
		    	switch(descSetup.bValueL)
		    	{
		    	    case 0:
		    	    	ep0State=EP0_STATE_GD_STR_I0;
		    	    	break;
		    	    case 1:
	       	    	    ep0State=EP0_STATE_GD_STR_I1;
		    	    	break;
		    	    case 2:	
		    	    	ep0State=EP0_STATE_GD_STR_I2;
		    	    	break;
		    	    default:
			    		DbgPrintf("[UE:STRI?]");
			    		break;
		    	}
		    	ep0SubState=0;
		    	break;
    	
		    default:
		    	DbgPrintf("[UE:GD?]");
		    	break;
	    	}	
    		break;/*device*/

    	case SET_ADDRESS:
            DbgPrintf("[SA:%d]",descSetup.bValueL);
            usbdevregs->FUNC_ADDR_REG=descSetup.bValueL | 0x80;
	    	CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
            ep0State=EP0_STATE_INIT;
            break;
    	
		case SET_CONFIGURATION:
            DbgPrintf("[SC]");
            CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
            ep0State=EP0_STATE_INIT;
            break;

    	    //////////////////////// For chapter 9 test ////////////////////
		case CLEAR_FEATURE:
			break;
		case GET_CONFIGURATION:
		  	break;
		case GET_INTERFACE:
		  	break;
		case GET_STATUS:	  	      
			break;
		case SET_DESCRIPTOR:
			break;
		case SET_FEATURE:  
			break;
		case SET_INTERFACE:
			break;
		case SYNCH_FRAME:
			break;

    	  //////////////////////////////////////////////////////////////
  	default:
	    DbgPrintf("[UE:SETUP=%x]",descSetup.bRequest);
    	CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
	    ep0State=EP0_STATE_INIT;
	    break;
        }
    }
    
    switch(ep0State)
    {	
		case EP0_STATE_INIT:
		    break; 

	//=== GET_DESCRIPTOR:DEVICE ===
    	case EP0_STATE_GD_DEV_0:
      //      DbgPrintf("[GDD0]");
            WrPktEp0((unsigned char *)&descDev+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_DEV_1;
            break;
            
    	case EP0_STATE_GD_DEV_1:
            DbgPrintf("[GDD1]");
            WrPktEp0((unsigned char *)&descDev+0x8,8); 
            SET_EP0_IN_PKT_RDY();
            
            ep0State=EP0_STATE_GD_DEV_2;
            break;

    	case EP0_STATE_GD_DEV_2:
            DbgPrintf("[GDD2]");
            WrPktEp0((unsigned char *)&descDev+0x10,2);   //8+8+2=0x12
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;
            break;
    

        //=== GET_DESCRIPTOR:CONFIGURATION+INTERFACE+ENDPOINT0+ENDPOINT1 ===
        //Windows98 gets these 4 descriptors all together by issuing only a request.
        //Windows2000 gets each descriptor seperately.
    	case EP0_STATE_GD_CFG_0:
            DbgPrintf("[GDC0]");
            WrPktEp0((unsigned char *)&descConf+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_1;
            break;
    
    	case EP0_STATE_GD_CFG_1:
            DbgPrintf("[GDC1]");
            WrPktEp0((unsigned char *)&descConf+8,1); 
            WrPktEp0((unsigned char *)&descIf+0,7); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_2;
            break;

    	case EP0_STATE_GD_CFG_2:
            DbgPrintf("[GDC2]");
            WrPktEp0((unsigned char *)&descIf+7,2); 
            WrPktEp0((unsigned char *)&descEndpt0+0,6); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_3;
            break;

    	case EP0_STATE_GD_CFG_3:
            DbgPrintf("[GDC3]");
            WrPktEp0((unsigned char *)&descEndpt0+6,1); 
            WrPktEp0((unsigned char *)&descEndpt1+0,7); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_4;            
            break;

    	case EP0_STATE_GD_CFG_4:
            DbgPrintf("[GDC4]");
             //zero length data packit 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;
            
        //=== GET_DESCRIPTOR:CONFIGURATION ONLY===
    	case EP0_STATE_GD_CFG_ONLY_0:
            DbgPrintf("[GDCO0]");
            WrPktEp0((unsigned char *)&descConf+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_ONLY_1;
            break;
    
    	case EP0_STATE_GD_CFG_ONLY_1:
            DbgPrintf("[GDCO1]");
            WrPktEp0((unsigned char *)&descConf+8,1); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;
          
            
        //=== GET_DESCRIPTOR:STRING ===
    	case EP0_STATE_GD_STR_I0:
            DbgPrintf("[GDS0_0]");
	        WrPktEp0((unsigned char *)descStr0, 4 );  
		    SET_EP0_INPKTRDY_DATAEND();
		    ep0State=EP0_STATE_INIT;     
		    ep0SubState=0;
		    break;

		case EP0_STATE_GD_STR_I1:
	        DbgPrintf("[GDS1_%d]",ep0SubState);
	        if( (ep0SubState*EP0_PKT_SIZE+EP0_PKT_SIZE)<sizeof(descStr1) )
	        {
	        	WrPktEp0((unsigned char *)descStr1+(ep0SubState*EP0_PKT_SIZE),EP0_PKT_SIZE); 
	        	SET_EP0_IN_PKT_RDY();
	        	ep0State=EP0_STATE_GD_STR_I1;
	        	ep0SubState++;
	        }
		    else
		    {
		    	WrPktEp0((unsigned char *)descStr1+(ep0SubState*EP0_PKT_SIZE),
		    		 sizeof(descStr1)-(ep0SubState*EP0_PKT_SIZE)); 
				SET_EP0_INPKTRDY_DATAEND();
				ep0State=EP0_STATE_INIT;     
				ep0SubState=0;
		    }
		    break;
	
		case EP0_STATE_GD_STR_I2:
	    	DbgPrintf("[GDS2_%d]",ep0SubState);
	        if( (ep0SubState*EP0_PKT_SIZE+EP0_PKT_SIZE)<sizeof(descStr2) )
	        {
	        	WrPktEp0((unsigned char *)descStr2+(ep0SubState*EP0_PKT_SIZE),EP0_PKT_SIZE); 
	        	SET_EP0_IN_PKT_RDY();
	        	ep0State=EP0_STATE_GD_STR_I2;
	        	ep0SubState++;
	        }
		    else
		    {
	            DbgPrintf("[E]");
		    	WrPktEp0((unsigned char *)descStr2+(ep0SubState*EP0_PKT_SIZE),
		    		 sizeof(descStr2)-(ep0SubState*EP0_PKT_SIZE)); 
				SET_EP0_INPKTRDY_DATAEND();
				ep0State=EP0_STATE_INIT;     
				ep0SubState=0;
		    }
		    break;
		 	
	 	default:
	    	DbgPrintf("UE:G?D");
	 	    break;
    }
}


void InitDescriptorTable(void)
{	
    //Standard device descriptor
    descDev.bLength=0x12;	//EP0_DEV_DESC_SIZE=0x12 bytes    
    descDev.bDescriptorType=DEVICE_TYPE;         
    descDev.bcdUSBL=0x10;
    descDev.bcdUSBH=0x01; 	//Ver 1.10
    descDev.bDeviceClass=0xFF; //0x0          
    descDev.bDeviceSubClass=0x0;          
    descDev.bDeviceProtocol=0x0;          
    descDev.bMaxPacketSize0=0x8;         
    descDev.idVendorL=0x45;
    descDev.idVendorH=0x53;
    descDev.idProductL=0x34;
    descDev.idProductH=0x12;
    descDev.bcdDeviceL=0x00;
    descDev.bcdDeviceH=0x01;
    descDev.iManufacturer=0x1;  //index of string descriptor
    descDev.iProduct=0x2;	//index of string descriptor 
    descDev.iSerialNumber=0x0;
    descDev.bNumConfigurations=0x1;

    //Standard configuration descriptor
    descConf.bLength=0x9;    
    descConf.bDescriptorType=CONFIGURATION_TYPE;         
    descConf.wTotalLengthL=0x20; //<cfg desc>+<if desc>+<endp0 desc>+<endp1 desc>
    descConf.wTotalLengthH=0;
    descConf.bNumInterfaces=1;
//dbg    descConf.bConfigurationValue=2;  //why 2? There's no reason.
    descConf.bConfigurationValue=1;  
    descConf.iConfiguration=0;
    descConf.bmAttributes=CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED;  //bus powered only.
    descConf.maxPower=25; //draws 50mA current from the USB bus.          

    //Standard interface descriptor
    descIf.bLength=0x9;    
    descIf.bDescriptorType=INTERFACE_TYPE;         
    descIf.bInterfaceNumber=0x0;
    descIf.bAlternateSetting=0x0; //?
    descIf.bNumEndpoints=2;	//# of endpoints except EP0
    descIf.bInterfaceClass=0xff; //0x0 ?
    descIf.bInterfaceSubClass=0x0;  
    descIf.bInterfaceProtocol=0x0;
    descIf.iInterface=0x0;

    //Standard endpoint0 descriptor
    descEndpt0.bLength=0x7;    
    descEndpt0.bDescriptorType=ENDPOINT_TYPE;         
    descEndpt0.bEndpointAddress=1|EP_ADDR_IN;   // 2400Xendpoint 1 is IN endpoint.
    descEndpt0.bmAttributes=EP_ATTR_BULK;
    descEndpt0.wMaxPacketSizeL=EP1_PKT_SIZE; //64
    descEndpt0.wMaxPacketSizeH=0x0;
    descEndpt0.bInterval=0x0; //not used

    //Standard endpoint1 descriptor
    descEndpt1.bLength=0x7;    
    descEndpt1.bDescriptorType=ENDPOINT_TYPE;         
    descEndpt1.bEndpointAddress=3|EP_ADDR_OUT;   // 2400X endpoint 3 is OUT endpoint.
    descEndpt1.bmAttributes=EP_ATTR_BULK;
    descEndpt1.wMaxPacketSizeL=EP3_PKT_SIZE; //64
    descEndpt1.wMaxPacketSizeH=0x0;
    descEndpt1.bInterval=0x0; //not used 
}

