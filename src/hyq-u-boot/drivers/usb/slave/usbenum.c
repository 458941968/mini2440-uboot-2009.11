/*
 *************************************************************************
 *
 * USB�豸ö��ģ��
 *
 *************************************************************************
 */
#include <common.h>
#if defined(CONFIG_S3C2400)
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#elif defined(CONFIG_S3C2440)
#include <s3c2440.h>
#endif

#include <s3c24x0.h>

#include "usblib.h"
#include "usbhid.h"
#include "usbdhid.h"
#include "usbcdc.h"
#include "2440usb.h"
#include "usbenum.h"
#include "usb2uart.h"
#include "usbdemo.h"
#include "usbkbd.h"

static unsigned char g_sRequest[EP0_MAX_PACKET_SIZE];	//==��׼�������ͻ�����
tDeviceInstance g_psUSBDevice[1];						//==�豸ʵ��


//****************************************************************************
//
// �豸��Ϣ�ṹ��
//
//****************************************************************************
tDeviceInfo g_sDeviceInfo = 
{
#ifdef CONFIG_USB_DEMO
	g_pDemoDeviceDescriptor,  		//==�豸������
	g_pDemoConfigDescriptors,		//==����������
	g_pDemoStringDescriptors,		//==�ַ���������
#endif
#ifdef CONFIG_USB_UART
	g_pCDCSerDeviceDescriptor,  		//==�豸������
	g_pCDCSerConfigDescriptors,		//==����������
	g_pCDCSerStringDescriptors,		//==�ַ���������
#endif
#ifdef CONFIG_USB_HID_KBD
	g_pHIDKbdDeviceDescriptor,  		//==USB�����豸������
	g_pHIDKbdConfigDescriptors,			//==USB��������������
	g_pHIDKbdStringDescriptors,			//==USB�����ַ���������
#endif
	0,
	0
};

#define DbgPrintX(level,fmt,args...)  DbgPrintf(fmt,##args);

#if 1
void DbgPrintf(char *fmt,...)
{
	int len;
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
  	len = strlen(string);
    va_end(ap);

    puts(string);
#ifdef CONFIG_USB_UART
//	Usb2UartSendData((unsigned char *)string, len);	 //==��USB���⴮�ڴ�ӡ������������ΪHID����ʱ������
#endif
}
#else
void DbgPrintf(char *fmt,...)
{
}
#endif


/********************************************************************
�������ܣ�����������
��ڲ�����ulIndex:USB������һ��Ϊ0��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void USBDCDStallEP0(void)
{
    USBEndpointStall(USB_EP_0) ;
    g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
}


/********************************************************************
�������ܣ���ȡ���������������ܳ��ȡ�
��ڲ�����*psConfig:����������ָ�롣
��    �أ����������������ܳ��ȡ�
��    ע���ޡ�
********************************************************************/
unsigned long USBDCDConfigDescGetSize(const tConfigHeader *psConfig)
{
    unsigned long ulLoop;
    unsigned long ulLen;

    ulLen = 0;

    for(ulLoop = 0; ulLoop < psConfig->ucNumSections; ulLoop++)
    {
        ulLen += psConfig->psSections[ulLoop]->usSize;
    }

    return(ulLen);
}


/********************************************************************
�������ܣ����պ�����
��ڲ�������
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
static void USBDEP0StateRx(void)
{
    unsigned long ulNumBytes;
    unsigned char *pData;

    g_psUSBDevice[0].eEP0State = USB_STATE_RX;

    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

	//==����������ݱȶ˵�0��
    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
		//==ֻ���ȷ��Ͷ˵�0����С���ֽ���
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }
    
    pData = (unsigned char *)g_psUSBDevice[0].pEP0Data;
    
	//==��������
    USBEndpointDataGet(USB_EP_0, pData, ulNumBytes);

	//==���һ����
    if(g_psUSBDevice[0].ulEP0DataRemain < EP0_MAX_PACKET_SIZE)
    {
		//==����ACK��������ɱ�־
        USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX_LAST);
        g_psUSBDevice[0].eEP0State =  USB_STATE_IDLE;
    }
    else
    {
		//==����ACK������δ��ɱ�־
        USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX);
    }

    g_psUSBDevice[0].pEP0Data += ulNumBytes;
    g_psUSBDevice[0].ulEP0DataRemain -= ulNumBytes;	
}
/********************************************************************
�������ܣ����ͺ�����
��ڲ�����ulIndex:USB������һ��Ϊ0��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
static void USBDEP0StateTx(void)
{
    unsigned long ulNumBytes;
    unsigned char *pData;

    g_psUSBDevice[0].eEP0State = USB_STATE_TX;

    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

	//==����������ݱȶ˵�0��
    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
		//==ֻ���ȷ��Ͷ˵�0����С���ֽ���
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }

    pData = (unsigned char *)g_psUSBDevice[0].pEP0Data;

    g_psUSBDevice[0].ulEP0DataRemain -= ulNumBytes;
    g_psUSBDevice[0].pEP0Data += ulNumBytes;

	//==ѹ���ݽ�FIFO������
    USBEndpointDataPut(USB_EP_0, pData, ulNumBytes);

	//==����������һ�����ݰ�
    if(ulNumBytes == EP0_MAX_PACKET_SIZE)
    {
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX);
    }
	//==���һ�����ݰ�
    else
    {
        g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX_LAST);
    }
}


/********************************************************************
�������ܣ�����������������
��ڲ�����ulIndex:USB������һ��Ϊ0��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
static void USBDEP0StateTxConfig(void)
{
    unsigned long ulNumBytes;
    unsigned long ulSecBytes;
    unsigned long ulToSend;
    unsigned char *pData;
    tConfigDescriptor sConfDesc;
    const tConfigHeader *psConfig;
    const tConfigSection *psSection;

    g_psUSBDevice[0].eEP0State = USB_STATE_TX_CONFIG;

  	//==��ǰ����
    psConfig = g_psUSBDevice[0].psInfo->ppConfigDescriptors[
               g_psUSBDevice[0].ucConfigIndex];

    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }

    //==��һ�ε���
    if((g_psUSBDevice[0].ucSectionOffset == 0) &&
       (g_psUSBDevice[0].ucConfigSection == 0))
    {
     	//==����������
        sConfDesc = *(tConfigDescriptor *)g_psUSBDevice[0].pEP0Data;

        sConfDesc.wTotalLength = (unsigned short)USBDCDConfigDescGetSize(
                                                                   psConfig);

        ulToSend = (ulNumBytes < sizeof(tConfigDescriptor)) ? ulNumBytes :
                        sizeof(tConfigDescriptor);
        USBEndpointDataPut(USB_EP_0, (unsigned char *)&sConfDesc,
                           ulToSend);

        //==��һ����
        if(psConfig->psSections[0]->usSize == ulToSend)
        {
            g_psUSBDevice[0].ucSectionOffset = 0;
            g_psUSBDevice[0].ucConfigSection = 1;
        }
        else
        {
            g_psUSBDevice[0].ucSectionOffset = (unsigned char)ulToSend;
        }

        ulToSend = ulNumBytes - ulToSend;
    }
    else
    {
        ulToSend = ulNumBytes;
    }

    while(ulToSend)
    {
 		//==��ǰ���õĶ�
        psSection = psConfig->psSections[g_psUSBDevice[0].ucConfigSection];

       //==���㵱ǰ��ʣ���ֽ�
        ulSecBytes = (unsigned long)(psSection->usSize -
                     g_psUSBDevice[0].ucSectionOffset);

        //==ָ��Ҫ���͵�����λ��
        pData = (unsigned char *)psSection->pucData +
                g_psUSBDevice[0].ucSectionOffset;

        if(ulSecBytes > ulToSend)
        {
            ulSecBytes = ulToSend;
        }

        USBEndpointDataPut(USB_EP_0, pData, ulSecBytes);

        ulToSend -= ulSecBytes;
        g_psUSBDevice[0].ucSectionOffset += (unsigned char)ulSecBytes;

		//==�ν���
        if(g_psUSBDevice[0].ucSectionOffset == psSection->usSize)
        {
            g_psUSBDevice[0].ucConfigSection++;
            g_psUSBDevice[0].ucSectionOffset = 0;
        }
    }

    g_psUSBDevice[0].ulEP0DataRemain -= ulNumBytes;

    if(psConfig->ucNumSections <= g_psUSBDevice[0].ucConfigSection)
    {
        g_psUSBDevice[0].ulEP0DataRemain = 0;
    }

    if(g_psUSBDevice[0].ulEP0DataRemain != 0)
    {
        pData =(unsigned char *)
            psConfig->psSections[g_psUSBDevice[0].ucConfigSection]->pucData;
        ulToSend = g_psUSBDevice[0].ucSectionOffset;
        g_psUSBDevice[0].pEP0Data = (pData + ulToSend);
    }

    //==�������һ����
    if(ulNumBytes == EP0_MAX_PACKET_SIZE)
    {
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX);
    }
	//==���һ����
    else
    {
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX_LAST);
        g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
    }
}


//*****************************************************************************
//
// 11�ֱ�׼��������ö�ٽ׶�ֻ��ʵ��3����
//
//*****************************************************************************
//==��ȡ״̬
static void USBDGetStatus(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rGetStatus\n");
}

//==�������
static void USBDClearFeature(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rClearFeature\n");
}

//==��������
static void USBDSetFeature(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSetFeature\n");
}

//==���õ�ַ
static void USBDSetAddress(void *pvInstance, tUSBRequest *pUSBRequest)
{
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device(); 	
    tDeviceInstance *psUSBControl;

	DbgPrintX(DBG_USBD_LEVEL,"\rSetAddress\n");

    psUSBControl = (tDeviceInstance *)pvInstance;

	//==����ACK��������ɱ�־
    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX_LAST);   
	usbdevregs->FUNC_ADDR_REG = pUSBRequest->wValue | 0x80;	
	psUSBControl->eEP0State = USB_STATE_IDLE;
}

//==��ȡ������
static void USBDGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned char bConfig;
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;

	DbgPrintX(DBG_USBD_LEVEL,"\rGetDscriptor\n");

    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->psInfo;

	//==����ACK������δ��ɱ�־
    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX);

    bConfig = 0;

    switch(pUSBRequest->wValue >> 8)
    {
		//==�����豸������
        case USB_DTYPE_DEVICE:
        {
			DbgPrintX(DBG_USBD_LEVEL,"\r--Device\n");

            psUSBControl->pEP0Data = (unsigned char *)psDevice->pDeviceDescriptor;
            psUSBControl->ulEP0DataRemain = psDevice->pDeviceDescriptor[0];
            break;
        }

		//==��������������
        case USB_DTYPE_CONFIGURATION:
       	{
            const tConfigHeader *psConfig;
            const tDeviceDescriptor *psDeviceDesc;
            unsigned char ucIndex;

			DbgPrintX(DBG_USBD_LEVEL,"\r--Configuration\n");

            ucIndex = (unsigned char)(pUSBRequest->wValue & 0xFF);

            psDeviceDesc =
                (const tDeviceDescriptor *)psDevice->pDeviceDescriptor;

			//==���󲻺Ϸ�����
            if(ucIndex >= psDeviceDesc->bNumConfigurations)
            {
                USBDCDStallEP0();
                psUSBControl->pEP0Data = 0;
                psUSBControl->ulEP0DataRemain = 0;
            }
            else
            {
                psConfig = psDevice->ppConfigDescriptors[ucIndex];

                psUSBControl->ucConfigSection = 0;			  //==���ö�
                psUSBControl->ucSectionOffset = 0;			  //==����ƫ��
                psUSBControl->pEP0Data = (unsigned char *)
                                          psConfig->psSections[0]->pucData;
                psUSBControl->ulEP0DataRemain =
                                            USBDCDConfigDescGetSize(psConfig);
                psUSBControl->ucConfigIndex = ucIndex;		   //==����ֵ

                bConfig = 1;
            }
            break;
        }

		//==�����ַ���������
        case USB_DTYPE_STRING:
        {
            long lIndex;

			lIndex = pUSBRequest->wValue & 0xFF;

			DbgPrintX(DBG_USBD_LEVEL,"\r--String%d\n",lIndex);

            psUSBControl->pEP0Data =
                (unsigned char *)psDevice->ppStringDescriptors[lIndex];

            psUSBControl->ulEP0DataRemain =
                psDevice->ppStringDescriptors[lIndex][0];

            break;
        }
#ifdef CONFIG_USB_HID_KBD
		//==���󱨸�������
		case USB_DTYPE_REPORT:
		{
			DbgPrintX(DBG_USBD_LEVEL,"\r--Report\n");
            psUSBControl->pEP0Data = (unsigned char *)g_pucHIDKbdReportDescriptor;
            psUSBControl->ulEP0DataRemain =  (g_pHID[8]<<8)+ g_pHID[7];
            break;	
		}
#endif
        default:
        {  
        	USBDCDStallEP0();
            return;
        }
    }

	//==���������Ҫ����
    if(psUSBControl->pEP0Data)
    {
        if(psUSBControl->ulEP0DataRemain > pUSBRequest->wLength)
        {
            psUSBControl->ulEP0DataRemain = pUSBRequest->wLength;
        }

        if(!bConfig)
        {
            USBDEP0StateTx();			//==һ�㷢��
        }
        else
        {
            USBDEP0StateTxConfig();		//==��������������
        }
    }
}

//==����������
static void USBDSetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSetDescriptor\n");
}

//==��ȡ����
static void USBDGetConfiguration(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rGetConfig\n");
}

//==��������
static void USBDSetConfiguration(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tDeviceInstance *psUSBControl;
    psUSBControl = (tDeviceInstance *)pvInstance;

	DbgPrintX(DBG_USBD_LEVEL,"\rSetConfig\n");
	
	//==����ACK��������ɱ�־
    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX_LAST);

    if(pUSBRequest->wValue > psUSBControl->psInfo->pDeviceDescriptor[17])
    {
        USBDCDStallEP0();
    }
    else
    {
		//==��������ֵ
        psUSBControl->ulConfiguration = pUSBRequest->wValue;
   		psUSBControl->eEP0State = USB_STATE_IDLE;
    }
}

//==��ȡ�ӿ�
static void USBDGetInterface(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rGetInterface\n");
}

//==���ýӿ�
static void USBDSetInterface(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSetInterface\n");
}

//==ͬ��֡
static void USBDSyncFrame(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSyncFrame\n");
}


//*****************************************************************************
//
// ��׼��������
//
//*****************************************************************************
static const tStdRequest g_psUSBDStdRequests[] =
{
    USBDGetStatus,
    USBDClearFeature,
    0,
    USBDSetFeature,
    0,
    USBDSetAddress,
    USBDGetDescriptor,
    USBDSetDescriptor,
    USBDGetConfiguration,
    USBDSetConfiguration,
    USBDGetInterface,
    USBDSetInterface,
    USBDSyncFrame
};


/********************************************************************
�������ܣ����������Ͳ��Ҵ�������
��ڲ�����ulIndex:USB������һ��Ϊ0��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
static void USBDReadAndDispatchRequest(void)
{
    unsigned long ulSize;
    tUSBRequest *pRequest;
#ifdef CONFIG_USB_UART
	tLineCoding sLineCoding;	//==��������
#endif

    pRequest = (tUSBRequest *)g_sRequest;
    ulSize = EP0_MAX_PACKET_SIZE;

	//==����������
    USBEndpointDataGet(USB_EP_0, g_sRequest, ulSize);

	//==���Ǳ�׼����
    if((pRequest->bmRequestType & USB_RTYPE_TYPE_M) != USB_RTYPE_STANDARD)
    {
        switch(pRequest->bRequest)
        {
#ifdef CONFIG_USB_UART
			//==���ô������ԣ�CDC������
			case USB_CDC_SET_LINE_CODING:
			{
				DbgPrintX(DBG_USBD_LEVEL,"\rSET_LINE_CODING\n");

				g_psUSBDevice[0].eEP0State = USB_STATE_RX;
				g_psUSBDevice[0].ulOUTDataSize = pRequest->wLength;
				g_psUSBDevice[0].ulEP0DataRemain = pRequest->wLength;

				USBEndpointDataAck(USB_EP_0, USB_FLAG_RX);
			    //USBDEP0StateRx();
            	break;
			}

			//==��ȡ�������ԣ�CDC������
            case USB_CDC_GET_LINE_CODING:
			{
				DbgPrintX(DBG_USBD_LEVEL,"\rGET_LINE_CODING\n");

				sLineCoding.ulRate = 115200;
				sLineCoding.ucStop = 0;
				sLineCoding.ucParity = 0;
				sLineCoding.ucDatabits = 8;

			    g_psUSBDevice[0].pEP0Data = (unsigned char *)&sLineCoding;
			    g_psUSBDevice[0].ulEP0DataRemain = 7;
			    g_psUSBDevice[0].ulOUTDataSize = 7;

				USBEndpointDataAck(USB_EP_0, USB_FLAG_RX);
			    USBDEP0StateTx();
            	break;
			}
			
			//==���ÿ���״̬��CDC������
			case USB_CDC_SET_CONTROL_LINE_STATE:
			{	
				DbgPrintX(DBG_USBD_LEVEL,"\rSET_CONTROL_LINE_STATE\n");

				g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
				USBEndpointDataAck(USB_EP_0, USB_FLAG_RX_LAST);
            	break;
			}  
#endif
			//==HID ������
#ifdef CONFIG_USB_HID_KBD
			//==���ÿ��У�HID������
            case USBREQ_SET_IDLE:
			{
				DbgPrintX(DBG_USBD_LEVEL,"\rSetIdle\n");

				g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
				USBEndpointDataAck(USB_EP_0, USB_FLAG_RX_LAST);
            	break;
			}
			
			//==���ñ��棨HID������
			case USBREQ_SET_REPORT:
			{	
				DbgPrintX(DBG_USBD_LEVEL,"\rSetReport\n");
					 
				g_psUSBDevice[0].eEP0State = USB_STATE_RX;
				g_psUSBDevice[0].ulOUTDataSize = pRequest->wLength;
				g_psUSBDevice[0].ulEP0DataRemain = pRequest->wLength;
				USBEndpointDataAck(USB_EP_0,USB_FLAG_RX);
            	break;
			}  
#endif
			default:
				USBDCDStallEP0();
        }
    }
    else //==��׼����
    {
        if((pRequest->bRequest <
           (sizeof(g_psUSBDStdRequests) / sizeof(tStdRequest))) &&
           (g_psUSBDStdRequests[pRequest->bRequest] != 0))
        {
			//==���ñ�׼��������
            g_psUSBDStdRequests[pRequest->bRequest](&g_psUSBDevice[0],
                                                    pRequest);
        }
        else
        {
            USBDCDStallEP0();
        }
    }
}


/********************************************************************
�������ܣ��˵�0ö�ٺ�����
��ڲ�����*pDevInstance���豸ʵ����
��    �أ��ޡ�
��    ע���˵�0״̬�ı�ͼ���¡�
********************************************************************/
void Ep0Handler(tDeviceInstance *pDevInstance)
{
    unsigned long ulEPStatus;
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();

    usbdevregs->INDEX_REG=0;

	//==��ȡ�˵�״̬
    ulEPStatus = usbdevregs->EP0_CSR_IN_CSR1_REG;

//	DbgPrintX(DBG_USBD_LEVEL,"\rEP0-ST=%d\n", ulEPStatus);

    if(ulEPStatus & EP0_SETUP_END)
    {   
		USBEndpointSetup(USB_EP_0);
		if(ulEPStatus & EP0_OUT_PKT_READY) 
		{  
		    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX);
		}
		
		pDevInstance->eEP0State = USB_STATE_IDLE;
		return;
    }	

    //I think that EP0_SENT_STALL will not be set to 1.
    if(ulEPStatus & EP0_SENT_STALL)
    {   
	   	USBEndpointStall(USB_EP_0);
		if(ulEPStatus & EP0_OUT_PKT_READY) 
		{
		    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX);
		}
		
		pDevInstance->eEP0State = USB_STATE_IDLE;
		return;
    }	

    switch(pDevInstance->eEP0State)
    {
		//==����״̬
        case USB_STATE_IDLE:
        {
            if(ulEPStatus & EP0_OUT_PKT_READY)
            {
                USBDReadAndDispatchRequest();
            }
            break;
        }

		//==����״̬
        case USB_STATE_TX:
        {
            USBDEP0StateTx();
            break;
        }

		//==��������������
        case USB_STATE_TX_CONFIG:
        {
            USBDEP0StateTxConfig();
            break;
        }

		//==����״̬
        case USB_STATE_RX:
        {
			USBDEP0StateRx();
            break;
        }
  
		//==����״̬
        case USB_STATE_STALL:
        {
            if(ulEPStatus & EP0_SENT_STALL)
            {
				//==��������־
                USBDCDStallEP0();
            }
            break;
        }
  
        default:
        {
        }
    }
}


#ifdef CONFIG_USB_UART
/********************************************************************
�������ܣ�����˵�3����������USB�����⴮�ڣ��������ݵ����ջ�������
��ڲ�����*pDevInstance���豸ʵ����
��    �أ��ޡ�
��    ע������˵�1Ϊ�������䡣
********************************************************************/
void Ep3Handler(tDeviceInstance *pDevInstance)
{
    int i;
	unsigned long ulEPStatus;
	unsigned long ulCount;
	unsigned char pucData[EP3_PKT_SIZE];
	struct s3c24x0_usb_device * const usbdevregs = s3c24x0_get_base_usb_device();

    usbdevregs->INDEX_REG=3;

	//==��ȡ�˵�״̬
    ulEPStatus = usbdevregs->OUT_CSR1_REG;

	//DbgPrintX(USB_DBG_LEVEL,"\rEP3 Status:%d\n",ulEPStatus);

	//==���յ�������
	if(ulEPStatus & EPO_OUT_PKT_READY)
	{	
		ulCount = usbdevregs->OUT_FIFO_CNT1_REG; 

		//==��������
		USBEndpointDataGet(USB_EP_3, pucData, ulCount);
        for(i=0; i<ulCount; i++)
            DbgPrintX(DBG_USBD_LEVEL,"\rReceive EP3 data=%-2c", pucData[i]);
        printf("\n");

	    //==����Ӧ��ACK
		USBEndpointDataAck(USB_EP_3, USB_FLAG_RX);
 
		//==�ѽ��յ������ݷ��ͻ�ȥ
		Usb2UartSendData((unsigned char*)pucData, ulCount);
		
		
	}
}
#endif


/********************************************************************
�������ܣ�USB0�жϴ�������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void USBIntHandler(void)
{
	struct s3c24x0_usb_device * const usbdevregs = s3c24x0_get_base_usb_device();
	struct s3c24x0_interrupt * const intregs = s3c24x0_get_base_interrupt();
    unsigned char usbdIntpnd, epIntpnd;
    unsigned char saveIndexReg = usbdevregs->INDEX_REG;
    usbdIntpnd = usbdevregs->USB_INT_REG;
    epIntpnd = usbdevregs->EP_INT_REG;
	
    if ( usbdIntpnd & SUSPEND_INT )
    {
    	usbdevregs->USB_INT_REG = SUSPEND_INT;
    	DbgPrintX(DBG_USBD_LEVEL,"\rsuspend!\n");
    }

    if ( usbdIntpnd & RESUME_INT )
    {
    	usbdevregs->USB_INT_REG = RESUME_INT;
    	DbgPrintX(DBG_USBD_LEVEL,"\rresume!\n");
    }

    if ( usbdIntpnd & RESET_INT )
    {
    	ConfigUsbd();
    	usbdevregs->USB_INT_REG = RESET_INT;  //RESET_INT should be cleared after ResetUsbd().   	
    	DbgPrintX(DBG_USBD_LEVEL, "\rreset!\n");  
    }

    if ( epIntpnd & EP0_INT )
    {
	    usbdevregs->EP_INT_REG = EP0_INT;  
    	Ep0Handler(&g_psUSBDevice[0]);
    }

    if( epIntpnd & EP1_INT )
    {
    	usbdevregs->EP_INT_REG=EP1_INT;  
	    DbgPrintX(DBG_USBD_LEVEL, "\rEp1\n");  
    	//Ep1Handler(&g_psUSBDevice[0]);
    }

    if( epIntpnd & EP2_INT )
    {
    	usbdevregs->EP_INT_REG=EP2_INT;  
	    DbgPrintX(DBG_USBD_LEVEL, "\rEp2\n");  
    	//Ep2Handler(&g_psUSBDevice[0]);
    }

    if( epIntpnd & EP3_INT )
    {
    	usbdevregs->EP_INT_REG=EP3_INT;  
	    DbgPrintX(DBG_USBD_LEVEL, "\rEp3\n");  
    	Ep3Handler(&g_psUSBDevice[0]);
    }

    intregs->SRCPND = BIT_USBD;
	intregs->INTPND = BIT_USBD;

    usbdevregs->INDEX_REG = saveIndexReg;
}


/********************************************************************
�������ܣ�USB��ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void USBDInit(void)
{
	struct s3c24x0_gpio * const gpioregs = s3c24x0_get_base_gpio();
	struct s3c24x0_interrupt * intregs = s3c24x0_get_base_interrupt();

	udelay(100000);

	//==����豸ʵ���ṹ��
    g_psUSBDevice[0].ulConfiguration = DEFAULT_CONFIG_ID;
    g_psUSBDevice[0].ulDefaultConfiguration = DEFAULT_CONFIG_ID;
    g_psUSBDevice[0].psInfo = &g_sDeviceInfo;
    g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
	g_psUSBDevice[0].ucStatus = 0;

	gpioregs->MISCCR=gpioregs->MISCCR&~(1<<3); // USBD is selected instead of USBH1 
	gpioregs->MISCCR=gpioregs->MISCCR&~(1<<13); // USB port 1 is enabled.
    
    ConfigUsbd(); 

	udelay(100000);
	intregs->INTMSK &= ~(BIT_USBD);  
	gpioregs->GPCDAT |= (1<<5);   /* enable USB Device, thisway.diy */
}
