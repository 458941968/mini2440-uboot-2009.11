/*
 *************************************************************************
 *
 * USB设备枚举模块
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

static unsigned char g_sRequest[EP0_MAX_PACKET_SIZE];	//==标准请求类型缓冲区
tDeviceInstance g_psUSBDevice[1];						//==设备实例


//****************************************************************************
//
// 设备信息结构体
//
//****************************************************************************
tDeviceInfo g_sDeviceInfo = 
{
#ifdef CONFIG_USB_DEMO
	g_pDemoDeviceDescriptor,  		//==设备描述符
	g_pDemoConfigDescriptors,		//==配置描述符
	g_pDemoStringDescriptors,		//==字符串描述符
#endif
#ifdef CONFIG_USB_UART
	g_pCDCSerDeviceDescriptor,  		//==设备描述符
	g_pCDCSerConfigDescriptors,		//==配置描述符
	g_pCDCSerStringDescriptors,		//==字符串描述符
#endif
#ifdef CONFIG_USB_HID_KBD
	g_pHIDKbdDeviceDescriptor,  		//==USB键盘设备描述符
	g_pHIDKbdConfigDescriptors,			//==USB键盘配置描述符
	g_pHIDKbdStringDescriptors,			//==USB键盘字符串描述符
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
//	Usb2UartSendData((unsigned char *)string, len);	 //==从USB虚拟串口打印函数，当配置为HID键盘时不启用
#endif
}
#else
void DbgPrintf(char *fmt,...)
{
}
#endif


/********************************************************************
函数功能：错误请求处理。
入口参数：ulIndex:USB索引，一般为0。
返    回：无。
备    注：无。
********************************************************************/
void USBDCDStallEP0(void)
{
    USBEndpointStall(USB_EP_0) ;
    g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
}


/********************************************************************
函数功能：获取配置描述符集合总长度。
入口参数：*psConfig:配置描述符指针。
返    回：配置描述符集合总长度。
备    注：无。
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
函数功能：接收函数。
入口参数：。
返    回：无。
备    注：无。
********************************************************************/
static void USBDEP0StateRx(void)
{
    unsigned long ulNumBytes;
    unsigned char *pData;

    g_psUSBDevice[0].eEP0State = USB_STATE_RX;

    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

	//==如果请求数据比端点0多
    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
		//==只能先发送端点0包大小的字节数
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }
    
    pData = (unsigned char *)g_psUSBDevice[0].pEP0Data;
    
	//==接收数据
    USBEndpointDataGet(USB_EP_0, pData, ulNumBytes);

	//==最后一个包
    if(g_psUSBDevice[0].ulEP0DataRemain < EP0_MAX_PACKET_SIZE)
    {
		//==返回ACK和数据完成标志
        USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX_LAST);
        g_psUSBDevice[0].eEP0State =  USB_STATE_IDLE;
    }
    else
    {
		//==返回ACK和数据未完成标志
        USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX);
    }

    g_psUSBDevice[0].pEP0Data += ulNumBytes;
    g_psUSBDevice[0].ulEP0DataRemain -= ulNumBytes;	
}
/********************************************************************
函数功能：发送函数。
入口参数：ulIndex:USB索引，一般为0。
返    回：无。
备    注：无。
********************************************************************/
static void USBDEP0StateTx(void)
{
    unsigned long ulNumBytes;
    unsigned char *pData;

    g_psUSBDevice[0].eEP0State = USB_STATE_TX;

    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

	//==如果请求数据比端点0多
    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
		//==只能先发送端点0包大小的字节数
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }

    pData = (unsigned char *)g_psUSBDevice[0].pEP0Data;

    g_psUSBDevice[0].ulEP0DataRemain -= ulNumBytes;
    g_psUSBDevice[0].pEP0Data += ulNumBytes;

	//==压数据进FIFO缓冲区
    USBEndpointDataPut(USB_EP_0, pData, ulNumBytes);

	//==如果不是最后一个数据包
    if(ulNumBytes == EP0_MAX_PACKET_SIZE)
    {
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX);
    }
	//==最后一个数据包
    else
    {
        g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX_LAST);
    }
}


/********************************************************************
函数功能：发送配置描述符。
入口参数：ulIndex:USB索引，一般为0。
返    回：无。
备    注：无。
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

  	//==当前配置
    psConfig = g_psUSBDevice[0].psInfo->ppConfigDescriptors[
               g_psUSBDevice[0].ucConfigIndex];

    ulNumBytes = g_psUSBDevice[0].ulEP0DataRemain;

    if(ulNumBytes > EP0_MAX_PACKET_SIZE)
    {
        ulNumBytes = EP0_MAX_PACKET_SIZE;
    }

    //==第一次调用
    if((g_psUSBDevice[0].ucSectionOffset == 0) &&
       (g_psUSBDevice[0].ucConfigSection == 0))
    {
     	//==配置描述符
        sConfDesc = *(tConfigDescriptor *)g_psUSBDevice[0].pEP0Data;

        sConfDesc.wTotalLength = (unsigned short)USBDCDConfigDescGetSize(
                                                                   psConfig);

        ulToSend = (ulNumBytes < sizeof(tConfigDescriptor)) ? ulNumBytes :
                        sizeof(tConfigDescriptor);
        USBEndpointDataPut(USB_EP_0, (unsigned char *)&sConfDesc,
                           ulToSend);

        //==第一段完
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
 		//==当前配置的段
        psSection = psConfig->psSections[g_psUSBDevice[0].ucConfigSection];

       //==计算当前段剩余字节
        ulSecBytes = (unsigned long)(psSection->usSize -
                     g_psUSBDevice[0].ucSectionOffset);

        //==指向要发送的数据位置
        pData = (unsigned char *)psSection->pucData +
                g_psUSBDevice[0].ucSectionOffset;

        if(ulSecBytes > ulToSend)
        {
            ulSecBytes = ulToSend;
        }

        USBEndpointDataPut(USB_EP_0, pData, ulSecBytes);

        ulToSend -= ulSecBytes;
        g_psUSBDevice[0].ucSectionOffset += (unsigned char)ulSecBytes;

		//==段结束
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

    //==不是最后一个包
    if(ulNumBytes == EP0_MAX_PACKET_SIZE)
    {
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX);
    }
	//==最后一个包
    else
    {
        USBEndpointDataSend(USB_EP_0, USB_FLAG_TX_LAST);
        g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
    }
}


//*****************************************************************************
//
// 11种标准请求函数（枚举阶段只需实现3个）
//
//*****************************************************************************
//==获取状态
static void USBDGetStatus(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rGetStatus\n");
}

//==清楚特性
static void USBDClearFeature(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rClearFeature\n");
}

//==设置特性
static void USBDSetFeature(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSetFeature\n");
}

//==设置地址
static void USBDSetAddress(void *pvInstance, tUSBRequest *pUSBRequest)
{
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device(); 	
    tDeviceInstance *psUSBControl;

	DbgPrintX(DBG_USBD_LEVEL,"\rSetAddress\n");

    psUSBControl = (tDeviceInstance *)pvInstance;

	//==返回ACK和数据完成标志
    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX_LAST);   
	usbdevregs->FUNC_ADDR_REG = pUSBRequest->wValue | 0x80;	
	psUSBControl->eEP0State = USB_STATE_IDLE;
}

//==获取描述符
static void USBDGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned char bConfig;
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;

	DbgPrintX(DBG_USBD_LEVEL,"\rGetDscriptor\n");

    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->psInfo;

	//==返回ACK和数据未完成标志
    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX);

    bConfig = 0;

    switch(pUSBRequest->wValue >> 8)
    {
		//==请求设备描述符
        case USB_DTYPE_DEVICE:
        {
			DbgPrintX(DBG_USBD_LEVEL,"\r--Device\n");

            psUSBControl->pEP0Data = (unsigned char *)psDevice->pDeviceDescriptor;
            psUSBControl->ulEP0DataRemain = psDevice->pDeviceDescriptor[0];
            break;
        }

		//==请求配置描述符
        case USB_DTYPE_CONFIGURATION:
       	{
            const tConfigHeader *psConfig;
            const tDeviceDescriptor *psDeviceDesc;
            unsigned char ucIndex;

			DbgPrintX(DBG_USBD_LEVEL,"\r--Configuration\n");

            ucIndex = (unsigned char)(pUSBRequest->wValue & 0xFF);

            psDeviceDesc =
                (const tDeviceDescriptor *)psDevice->pDeviceDescriptor;

			//==请求不合法配置
            if(ucIndex >= psDeviceDesc->bNumConfigurations)
            {
                USBDCDStallEP0();
                psUSBControl->pEP0Data = 0;
                psUSBControl->ulEP0DataRemain = 0;
            }
            else
            {
                psConfig = psDevice->ppConfigDescriptors[ucIndex];

                psUSBControl->ucConfigSection = 0;			  //==配置段
                psUSBControl->ucSectionOffset = 0;			  //==段内偏移
                psUSBControl->pEP0Data = (unsigned char *)
                                          psConfig->psSections[0]->pucData;
                psUSBControl->ulEP0DataRemain =
                                            USBDCDConfigDescGetSize(psConfig);
                psUSBControl->ucConfigIndex = ucIndex;		   //==配置值

                bConfig = 1;
            }
            break;
        }

		//==请求字符串描述符
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
		//==请求报告描述符
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

	//==如果有数据要发送
    if(psUSBControl->pEP0Data)
    {
        if(psUSBControl->ulEP0DataRemain > pUSBRequest->wLength)
        {
            psUSBControl->ulEP0DataRemain = pUSBRequest->wLength;
        }

        if(!bConfig)
        {
            USBDEP0StateTx();			//==一般发送
        }
        else
        {
            USBDEP0StateTxConfig();		//==发送配置描述符
        }
    }
}

//==设置描述符
static void USBDSetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSetDescriptor\n");
}

//==获取配置
static void USBDGetConfiguration(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rGetConfig\n");
}

//==设置配置
static void USBDSetConfiguration(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tDeviceInstance *psUSBControl;
    psUSBControl = (tDeviceInstance *)pvInstance;

	DbgPrintX(DBG_USBD_LEVEL,"\rSetConfig\n");
	
	//==返回ACK和数据完成标志
    USBEndpointDataAck(USB_EP_0 ,USB_FLAG_RX_LAST);

    if(pUSBRequest->wValue > psUSBControl->psInfo->pDeviceDescriptor[17])
    {
        USBDCDStallEP0();
    }
    else
    {
		//==保存配置值
        psUSBControl->ulConfiguration = pUSBRequest->wValue;
   		psUSBControl->eEP0State = USB_STATE_IDLE;
    }
}

//==获取接口
static void USBDGetInterface(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rGetInterface\n");
}

//==设置接口
static void USBDSetInterface(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSetInterface\n");
}

//==同步帧
static void USBDSyncFrame(void *pvInstance, tUSBRequest *pUSBRequest)
{
	DbgPrintX(DBG_USBD_LEVEL,"\rSyncFrame\n");
}


//*****************************************************************************
//
// 标准请求函数表
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
函数功能：读请求类型并且处理请求。
入口参数：ulIndex:USB索引，一般为0。
返    回：无。
备    注：无。
********************************************************************/
static void USBDReadAndDispatchRequest(void)
{
    unsigned long ulSize;
    tUSBRequest *pRequest;
#ifdef CONFIG_USB_UART
	tLineCoding sLineCoding;	//==串口属性
#endif

    pRequest = (tUSBRequest *)g_sRequest;
    ulSize = EP0_MAX_PACKET_SIZE;

	//==读请求类型
    USBEndpointDataGet(USB_EP_0, g_sRequest, ulSize);

	//==不是标准请求
    if((pRequest->bmRequestType & USB_RTYPE_TYPE_M) != USB_RTYPE_STANDARD)
    {
        switch(pRequest->bRequest)
        {
#ifdef CONFIG_USB_UART
			//==设置串口属性（CDC类请求）
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

			//==获取串口属性（CDC类请求）
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
			
			//==设置控制状态（CDC类请求）
			case USB_CDC_SET_CONTROL_LINE_STATE:
			{	
				DbgPrintX(DBG_USBD_LEVEL,"\rSET_CONTROL_LINE_STATE\n");

				g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
				USBEndpointDataAck(USB_EP_0, USB_FLAG_RX_LAST);
            	break;
			}  
#endif
			//==HID 类请求
#ifdef CONFIG_USB_HID_KBD
			//==设置空闲（HID类请求）
            case USBREQ_SET_IDLE:
			{
				DbgPrintX(DBG_USBD_LEVEL,"\rSetIdle\n");

				g_psUSBDevice[0].eEP0State = USB_STATE_IDLE;
				USBEndpointDataAck(USB_EP_0, USB_FLAG_RX_LAST);
            	break;
			}
			
			//==设置报告（HID类请求）
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
    else //==标准请求
    {
        if((pRequest->bRequest <
           (sizeof(g_psUSBDStdRequests) / sizeof(tStdRequest))) &&
           (g_psUSBDStdRequests[pRequest->bRequest] != 0))
        {
			//==调用标准请求函数表
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
函数功能：端点0枚举函数。
入口参数：*pDevInstance：设备实例。
返    回：无。
备    注：端点0状态改变图如下。
********************************************************************/
void Ep0Handler(tDeviceInstance *pDevInstance)
{
    unsigned long ulEPStatus;
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();

    usbdevregs->INDEX_REG=0;

	//==获取端点状态
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
		//==空闲状态
        case USB_STATE_IDLE:
        {
            if(ulEPStatus & EP0_OUT_PKT_READY)
            {
                USBDReadAndDispatchRequest();
            }
            break;
        }

		//==发送状态
        case USB_STATE_TX:
        {
            USBDEP0StateTx();
            break;
        }

		//==发送配置描述符
        case USB_STATE_TX_CONFIG:
        {
            USBDEP0StateTxConfig();
            break;
        }

		//==接收状态
        case USB_STATE_RX:
        {
			USBDEP0StateRx();
            break;
        }
  
		//==出错状态
        case USB_STATE_STALL:
        {
            if(ulEPStatus & EP0_SENT_STALL)
            {
				//==清除出错标志
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
函数功能：输出端点3处理函数，从USB（虚拟串口）接收数据到接收缓冲区。
入口参数：*pDevInstance：设备实例。
返    回：无。
备    注：输出端点1为批量传输。
********************************************************************/
void Ep3Handler(tDeviceInstance *pDevInstance)
{
    int i;
	unsigned long ulEPStatus;
	unsigned long ulCount;
	unsigned char pucData[EP3_PKT_SIZE];
	struct s3c24x0_usb_device * const usbdevregs = s3c24x0_get_base_usb_device();

    usbdevregs->INDEX_REG=3;

	//==获取端点状态
    ulEPStatus = usbdevregs->OUT_CSR1_REG;

	//DbgPrintX(USB_DBG_LEVEL,"\rEP3 Status:%d\n",ulEPStatus);

	//==接收到数据了
	if(ulEPStatus & EPO_OUT_PKT_READY)
	{	
		ulCount = usbdevregs->OUT_FIFO_CNT1_REG; 

		//==接收数据
		USBEndpointDataGet(USB_EP_3, pucData, ulCount);
        for(i=0; i<ulCount; i++)
            DbgPrintX(DBG_USBD_LEVEL,"\rReceive EP3 data=%-2c", pucData[i]);
        printf("\n");

	    //==返回应答ACK
		USBEndpointDataAck(USB_EP_3, USB_FLAG_RX);
 
		//==把接收到的数据发送回去
		Usb2UartSendData((unsigned char*)pucData, ulCount);
		
		
	}
}
#endif


/********************************************************************
函数功能：USB0中断处理函数。
入口参数：无。
返    回：无。
备    注：无。
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
函数功能：USB初始化。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void USBDInit(void)
{
	struct s3c24x0_gpio * const gpioregs = s3c24x0_get_base_gpio();
	struct s3c24x0_interrupt * intregs = s3c24x0_get_base_interrupt();

	udelay(100000);

	//==填充设备实例结构体
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
