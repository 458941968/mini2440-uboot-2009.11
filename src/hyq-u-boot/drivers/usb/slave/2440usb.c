/****************************************************************
 NAME: u2440mon.c
 DESC: u2440mon entry point,menu,download
 HISTORY:
 Mar.25.2002:purnnamu: S3C2400X profile.c is ported for S3C2410X.
 Mar.27.2002:purnnamu: DMA is enabled.
 Apr.01.2002:purnnamu: isDownloadReady flag is added.
 Apr.10.2002:purnnamu: - Selecting menu is available in the waiting loop. 
                         So, isDownloadReady flag gets not needed
                       - UART ch.1 can be selected for the console.
 Aug.20.2002:purnnamu: revision number change 0.2 -> R1.1       
 Sep.03.2002:purnnamu: To remove the power noise in the USB signal, the unused CLKOUT0,1 is disabled.
 ****************************************************************/
#include <common.h>
#if defined(CONFIG_S3C2400)
#include <s3c2400.h>
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#elif defined(CONFIG_S3C2440)
#include <s3c2440.h>
#endif

#include <s3c24x0.h>

#include "2440usb.h"


/********************************************************************
函数功能：清除setup。
入口参数：ulEndpoint：端口号。
返    回：无。
备    注：无。
********************************************************************/
void USBEndpointSetup(unsigned long ulEndpoint)
{
	struct s3c24x0_usb_device * const usbdevregs = s3c24x0_get_base_usb_device();   
    unsigned long ulEPStatus = usbdevregs->EP0_CSR_IN_CSR1_REG;

	usbdevregs->EP0_CSR_IN_CSR1_REG = ((ulEPStatus & (~EP0_WR_BITS)) | (EP0_SERVICED_SETUP_END));
}


/********************************************************************
函数功能：清除stall。
入口参数：ulEndpoint：端口号。
返    回：无。
备    注：无。
********************************************************************/
void USBEndpointStall(unsigned long ulEndpoint)
{
	struct s3c24x0_usb_device * const usbdevregs = s3c24x0_get_base_usb_device();   
    unsigned long ulEPStatus = usbdevregs->EP0_CSR_IN_CSR1_REG;

	usbdevregs->EP0_CSR_IN_CSR1_REG = ((ulEPStatus & (~EP0_WR_BITS)) & (~EP0_SENT_STALL));
}


/********************************************************************
函数功能：清除输出，返回ACK。
入口参数：ulEndpoint：端口号；bIsLastPacket：接收最后一个包。
返    回：无。
备    注：无。
********************************************************************/
void USBEndpointDataAck(unsigned long ulEndpoint, unsigned char bIsLastPacket)
{
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();   

    if( ulEndpoint == USB_EP_0 )
    {
        unsigned long ulEPStatus = usbdevregs->EP0_CSR_IN_CSR1_REG;

        if( bIsLastPacket )
        {
            usbdevregs->INDEX_REG=0;
            usbdevregs->EP0_CSR_IN_CSR1_REG = ((ulEPStatus & (~EP0_WR_BITS)) | (EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END));
        }else
        {
            usbdevregs->EP0_CSR_IN_CSR1_REG = ((ulEPStatus & (~EP0_WR_BITS)) | EP0_SERVICED_OUT_PKT_RDY );
        }
    }
    else if( ulEndpoint == USB_EP_3 )
    {
        usbdevregs->INDEX_REG=3;
        unsigned long ulEPStatus = usbdevregs->OUT_CSR1_REG;
        usbdevregs->OUT_CSR1_REG = ((ulEPStatus & (~ EPO_WR_BITS)) & (~EPO_OUT_PKT_READY) );
    }
}


/********************************************************************
函数功能：发送数据给主机。
入口参数：ulEndpoint：端口号；bIsLastPacket：发送最后一个包。
返    回：无。
备    注：无。
********************************************************************/
void USBEndpointDataSend(unsigned long ulEndpoint, unsigned char bIsLastPacket)
{
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();

    if( ulEndpoint == USB_EP_0 )
    {
        usbdevregs->INDEX_REG=0;
        unsigned long ulEPStatus = usbdevregs->EP0_CSR_IN_CSR1_REG;

        if( bIsLastPacket )
        {
            usbdevregs->EP0_CSR_IN_CSR1_REG=((ulEPStatus & (~EP0_WR_BITS)) | (EP0_IN_PKT_READY|EP0_DATA_END))	;
        }else
        {
            usbdevregs->EP0_CSR_IN_CSR1_REG=((ulEPStatus & (~EP0_WR_BITS)) | (EP0_IN_PKT_READY));
        }
    }
    else if( ulEndpoint == USB_EP_2 )
    {
        usbdevregs->INDEX_REG=2;
        unsigned long ulEPStatus = usbdevregs->EP0_CSR_IN_CSR1_REG;
        usbdevregs->EP0_CSR_IN_CSR1_REG = ((ulEPStatus & (~EPI_WR_BITS)) | EPI_IN_PKT_READY );
    }
}


/********************************************************************
函数功能：读数据。
入口参数：ulEndpoint：端口号；pucData：数据缓冲区；ulSize：数据大小。
返    回：无。
备    注：无。
********************************************************************/
void USBEndpointDataGet(unsigned long ulEndpoint, unsigned char *pucData, unsigned long ulSize)
{
    int i;
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();   
//	printf("read:");	
    for(i=0; i<ulSize; i++)
    {
        pucData[i] = (unsigned char)usbdevregs->fifo[ulEndpoint].EP_FIFO_REG;
//		printf("0x%-3x", pucData[i]);
	}  
//	printf("\n");
}
    

/********************************************************************
函数功能：写数据。
入口参数：ulEndpoint：端口号；pucData：数据缓冲区；ulSize：数据大小。
返    回：无。
备    注：无。
********************************************************************/
void USBEndpointDataPut(unsigned long ulEndpoint, unsigned char *pucData, unsigned long ulSize)
{
    int i;
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();      	
//	printf("read:");	
    for(i=0; i<ulSize; i++)
    {
        usbdevregs->fifo[ulEndpoint].EP_FIFO_REG=pucData[i];
//		printf("0x%-3x", pucData[i]);
    }
//	printf("\n");
}


/********************************************************************
函数功能：2440配置。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void ConfigUsbd(void)
{
// *** End point information ***
//   EP0: control
//   EP1: interrupt in end point
//   EP2: bulk in end point
//   EP3: bulk out end point
//   EP4: not used
	struct s3c24x0_usb_device * const usbdevregs = s3c24x0_get_base_usb_device();   
 
    usbdevregs->PWR_REG = DISABLE_SUSPEND;	//disable suspend mode

    usbdevregs->INDEX_REG = 0;	
    usbdevregs->MAXP_REG = FIFO_SIZE_8;   	//EP0 max packit size = 8 
    usbdevregs->EP0_CSR_IN_CSR1_REG = EP0_SERVICED_OUT_PKT_RDY | EP0_SERVICED_SETUP_END;	
 				//EP0:clear OUT_PKT_RDY & SETUP_END

    usbdevregs->INDEX_REG = 1;
    usbdevregs->MAXP_REG = FIFO_SIZE_32;	//EP2:max packit size = 32
    usbdevregs->EP0_CSR_IN_CSR1_REG = EPI_FIFO_FLUSH | EPI_CDT;	
    usbdevregs->IN_CSR2_REG = EPI_MODE_IN | EPI_IN_DMA_INT_MASK | EPI_BULK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG = EPO_CDT;   	
    usbdevregs->OUT_CSR2_REG = EPO_BULK | EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG = 2;
#if (EP1_PKT_SIZE==32)
    usbdevregs->MAXP_REG = FIFO_SIZE_32;	//EP1:max packit size = 32
#else
	usbdevregs->MAXP_REG = FIFO_SIZE_64;	//EP1:max packit size = 64
#endif	
    usbdevregs->EP0_CSR_IN_CSR1_REG = EPI_FIFO_FLUSH | EPI_CDT;
    usbdevregs->IN_CSR2_REG = EPI_MODE_IN | EPI_IN_DMA_INT_MASK | EPI_BULK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG = EPO_CDT;   	
    usbdevregs->OUT_CSR2_REG = EPO_BULK | EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG = 3;
#if (EP3_PKT_SIZE==32)
    usbdevregs->MAXP_REG = FIFO_SIZE_32;	//EP3:max packit size = 32
#else
	usbdevregs->MAXP_REG = FIFO_SIZE_64;	//EP3:max packit size = 64
#endif	
    usbdevregs->EP0_CSR_IN_CSR1_REG = EPI_CDT;
    usbdevregs->IN_CSR2_REG = EPI_MODE_OUT | EPI_IN_DMA_INT_MASK | EPI_BULK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG = EPO_FIFO_FLUSH | EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    usbdevregs->OUT_CSR2_REG = EPO_BULK | EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG=4;
    usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP4:max packit size = 64
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    usbdevregs->IN_CSR2_REG=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;    


    usbdevregs->EP_INT_REG = EP0_INT | EP1_INT | EP2_INT | EP3_INT | EP4_INT;
    usbdevregs->USB_INT_REG = RESET_INT | SUSPEND_INT | RESUME_INT; 
    usbdevregs->EP_INT_EN_REG = EP0_INT | EP3_INT;
    usbdevregs->USB_INT_EN_REG = RESET_INT;
}


/********************************************************************
函数功能：断开函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void udc_disconnect (void)
{
	struct s3c24x0_gpio * const  gpioregs = s3c24x0_get_base_gpio();
	gpioregs->GPCDAT = gpioregs->GPCDAT & ~(1<<5);	
//	UDCDBG ("disconnect, disable Pullup");
}


