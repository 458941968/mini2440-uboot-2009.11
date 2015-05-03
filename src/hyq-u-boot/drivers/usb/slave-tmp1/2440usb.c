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

#include "usbenum.h"
#include "2440usb.h"



/**************端点0读数据*************/
void RdPktEp0(unsigned char *buf,int num)
{
    int i;
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();   
 	
    for(i=0;i<num;i++)
    {
        buf[i]=(unsigned char)usbdevregs->fifo[0].EP_FIFO_REG;
    }
}
    

/**************端点0写数据***********/
void WrPktEp0(unsigned char *buf,int num)
{
    int i;
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();      	
    for(i=0;i<num;i++)
    {
        usbdevregs->fifo[0].EP_FIFO_REG=buf[i];	
    }
}


/**************USBD配置函数*************/
void ReconfigUsbd(void)
{
// *** End point information ***
//   EP0: control
//   EP1: bulk in end point
//   EP2: not used
//   EP3: bulk out end point
//   EP4: not used
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();   
 
    usbdevregs->PWR_REG=DISABLE_SUSPEND;	//disable suspend mode

    usbdevregs->INDEX_REG=0;	
    usbdevregs->MAXP_REG=FIFO_SIZE_8;   	//EP0 max packit size = 8 
    usbdevregs->EP0_CSR_IN_CSR1_REG=EP0_SERVICED_OUT_PKT_RDY|EP0_SERVICED_SETUP_END;	
 				//EP0:clear OUT_PKT_RDY & SETUP_END
    usbdevregs->INDEX_REG=1;
    #if (EP1_PKT_SIZE==32)
        usbdevregs->MAXP_REG=FIFO_SIZE_32;	//EP1:max packit size = 32
    #else
	usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP1:max packit size = 64
    #endif	
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT;	
    usbdevregs->IN_CSR2_REG=EPI_MODE_IN|EPI_IN_DMA_INT_MASK|EPI_BULK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG=2;
    usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP2:max packit size = 64
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    usbdevregs->IN_CSR2_REG=EPI_MODE_IN|EPI_IN_DMA_INT_MASK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG=3;
    #if (EP3_PKT_SIZE==32)
        usbdevregs->MAXP_REG=FIFO_SIZE_32;	//EP3:max packit size = 32
    #else
	usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP3:max packit size = 64
    #endif	
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    usbdevregs->IN_CSR2_REG=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG=4;
    usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP4:max packit size = 64
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    usbdevregs->IN_CSR2_REG=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	
    
    usbdevregs->EP_INT_REG=EP0_INT|EP1_INT|EP2_INT|EP3_INT|EP4_INT;
    usbdevregs->USB_INT_REG=RESET_INT|SUSPEND_INT|RESUME_INT; 
    	//Clear all usbd pending bits
    	
    //EP0,1,3 & reset interrupt are enabled
    usbdevregs->EP_INT_EN_REG=EP0_INT|EP1_INT|EP3_INT;
    usbdevregs->USB_INT_EN_REG=RESET_INT;
    ep0State=EP0_STATE_INIT;    
}


/**************USB设备初始化******************/
void usb_init_slave(void)
{
	struct s3c24x0_gpio * const gpioregs = s3c24x0_get_base_gpio();
	struct s3c24x0_interrupt * intregs = s3c24x0_get_base_interrupt();
	udelay(100000);

	gpioregs->MISCCR=gpioregs->MISCCR&~(1<<3); // USBD is selected instead of USBH1 
	gpioregs->MISCCR=gpioregs->MISCCR&~(1<<13); // USB port 1 is enabled.

    //ChangeUPllValue(0x38,2,1);	// UCLK=96Mhz     
    //ChangeUPllValue(0x38,2,2);	// UCLK=48Mhz     
    InitDescriptorTable();
    
    ReconfigUsbd();

	udelay(100000);
	intregs->INTMSK &= ~(BIT_USBD);  
	gpioregs->GPCDAT |= (1<<5);   /* enable USB Device, thisway.diy */
}


/**************USBD断开函数*************/
void udc_disconnect (void)
{
	struct s3c24x0_gpio 		* const  gpioregs	= s3c24x0_get_base_gpio();
	gpioregs->GPCDAT = gpioregs->GPCDAT & ~(1<<5);	
//	UDCDBG ("disconnect, disable Pullup");
}


/**************USBD中断处理函数*************/
void isr_usbd(void)
{
	struct s3c24x0_usb_device * const usbdevregs	= s3c24x0_get_base_usb_device();
	struct s3c24x0_interrupt * const intregs = s3c24x0_get_base_interrupt();
    unsigned char usbdIntpnd, epIntpnd;
    unsigned char saveIndexReg=usbdevregs->INDEX_REG;
    usbdIntpnd=usbdevregs->USB_INT_REG;
    epIntpnd=usbdevregs->EP_INT_REG;
    debugX( DBG_USBD_LEVEL,  "[INT:EP_I=%x,USBI=%x]", epIntpnd, usbdIntpnd );

    if(usbdIntpnd&SUSPEND_INT)
    {
    	usbdevregs->USB_INT_REG=SUSPEND_INT;
    	debugX( DBG_USBD_LEVEL,  "<SUS]\n");
    }

    if(usbdIntpnd&RESUME_INT)
    {
    	usbdevregs->USB_INT_REG=RESUME_INT;
    	debugX( DBG_USBD_LEVEL, "<RSM]\n");
    }

    if(usbdIntpnd&RESET_INT)
    {
    	debugX( DBG_USBD_LEVEL,  "<RST] ReconfigUsbd\n");  
    	
    	//ResetUsbd();
    	ReconfigUsbd();

    	usbdevregs->USB_INT_REG=RESET_INT;  //RESET_INT should be cleared after ResetUsbd().   	
    }

    if(epIntpnd&EP0_INT)
    {
	    usbdevregs->EP_INT_REG=EP0_INT;  
    	Ep0Handler();
    }

    if(epIntpnd&EP1_INT)
    {
    	usbdevregs->EP_INT_REG=EP1_INT;  
    	//Ep1Handler();
    }

    intregs->SRCPND = BIT_USBD;
	intregs->INTPND = BIT_USBD;
    
    usbdevregs->INDEX_REG=saveIndexReg;
}
