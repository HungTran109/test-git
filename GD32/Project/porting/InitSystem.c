/******************************************************************************
 * @file    	InitSystem.c
 * @author  	Phinht
 * @version 	V1.0.0
 * @date    	28/02/2016
 * @brief   	
 ******************************************************************************/

/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "InitSystem.h"
#include "Version.h"
#include "DataDefine.h"
#include "Utilities.h"
#include "InternalFlash.h"
#include "HardwareManager.h"
#include "Led7.h"
#include "I2CFM.h"
#include "Indicator.h"
#include "Main.h"
#include "gps.h"
/******************************************************************************
                                   GLOBAL VARIABLES					    			 
 ******************************************************************************/
System_t xSystem;
extern DriverUART_t DriverUART;
extern Debug_t DriverDebug;
//extern uint8_t esp32RebootTimeout;

//extern Driver_InternalFlash_t DriverInternalFlash;

/******************************************************************************
                                   GLOBAL FUNCTIONS					    			 
 ******************************************************************************/

/******************************************************************************
                                   DATA TYPE DEFINE					    			 
 ******************************************************************************/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small DEBUG_INFO (option LD Linker->Libraries->Small DEBUG_INFO
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//#if   (defined ( __CC_ARM ))
//  __IO uint32_t VectorTable[48] __attribute__((at(0x20000000)));
//#elif (defined (__ICCARM__))
//#pragma location = 0x20000000
//  __no_init __IO uint32_t VectorTable[48];
//#elif defined   (  __GNUC__  )
//  __IO uint32_t VectorTable[48] __attribute__((section(".RAMVectorTable")));
//#elif defined ( __TASKING__ )
//  __IO uint32_t VectorTable[48] __at(0x20000000);
//#endif

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
const char WelcomeStr[] = "Copyright by BYTECH JSC\r\n";
//const char Bytech[] = {
//"\r    ______  ______________________  __       _______ ______\r"
//"   / __ ) \\/ /_  __/ ____/ ____/ / / /      / / ___// ____/\r"
//"  / __  |\\  / / / / __/ / /   / /_/ /  __  / /\\__ \\/ /     \r"
//" / /_/ / / / / / / /___/ /___/ __  /  / /_/ /___/ / /___\r"
//"/_____/ /_/ /_/ /_____/\\____/_/ /_/   \\____//____/\\____/   \r"
//};

/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/
static void RCC_Config(void);
static void InitIO(void);
static void ADC_Config(void);
static void InitVariable(void);
static void DrawScreen(void);

/*
* Su dung khi co bootloader
*/
//static void RelocateVectorTable(void)
//{
//	uint8_t i = 0;
//
//	/* Relocate by software the vector table to the internal SRAM at 0x20000000 ***/
//	/* Copy the vector table from the Flash (mapped at the base of the application
//	load address 0x08003000) to the base address of the SRAM at 0x20000000. */
//	for(i = 0; i < 48; i++)
//	{
//		VectorTable[i] = *(__IO uint32_t*)(APPLICATION_FW_ADDR + (i<<2));
//	}
//	/* Enable the SYSCFG peripheral clock*/
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//	/* Remap SRAM at 0x00000000 */
//	SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
//}

void LockReadOutFlash(void)
{
    //	if(FLASH_OB_GetRDP() != SET)
    //	{
    //		DEBUG_INFO("\rFlash: Chua Lock Read protection. Thuc hien Lock...");
    //
    //		FLASH_Unlock();
    //		FLASH_OB_Unlock();
    //		FLASH_OB_RDPConfig(OB_RDP_Level_1);
    //		FLASH_OB_Launch();                   // Option Bytes programming
    //		FLASH_OB_Lock();
    //		FLASH_Lock();
    //	}
    //	else
    //		DEBUG_INFO("\rFlash: Read protect is LOCKED!");
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/03/2016
 * @version	:
 * @reviewer:	
 */
void InitSystem(void)
{
    /* Set Vector table to offset address of main application - used with bootloader */
    //	RelocateVectorTable();

    RCC_Config();
    InitIO();
    WatchDogInit();

    InitVariable();
    //	xSystem.Driver.UART->Init(DEBUG_UART, 115200);

    //Delay khoi tao UART, khong truyen UART len ESP32 ngay khi khoi dong!
    DEBUG_RAW("GPS power off\r\n");
    GPS_POWER_OFF();
    Delayms(1000);
    DEBUG_RAW("GPS power on\r\n");

    xSystem.Driver.UART->Init(ESP32_UART, 115200);
    xSystem.Driver.UART->Init(GPS_UART, 9600U);
    DEBUG_RAW("Init GPS\r\n");
    GPS_POWER_ON();
        
//    InitGPS();
    
    ADC_Config();

    //Read protection -> luc nap lai se mass erase chip -> mat luon cau hinh
    //	LockReadOutFlash();

    DrawScreen();
    DetectResetReason();

    /* Init params */
    InternalFlash_ReadConfig();

    /* Init LED7 Seg */
    Led7_Init();
    Button_Init();

    //	/* Init FM */
    FM_Init();

    xSystem.Status.SystemInitDone = 1;
}

/*****************************************************************************/
/**
 * @brief	:  	DrawScreen
 * @param	:  	None
 * @retval	:	None
 * @author	:	Phinht
 * @created	:	28/02/2016
 * @version	:
 * @reviewer:	
 */
static void DrawScreen(void)
{
    DEBUG_RAW("\r\n");
    DEBUG_RAW("***********************************************************\r\n");
    DEBUG_RAW(WelcomeStr);
    DEBUG_RAW("**********************************************************\r\n");
    DEBUG_RAW("Device: VS-Worker\r\n");
    DEBUG_RAW("Firmware: %s%03u\r\n", FIRMWARE_VERSION_HEADER, FIRMWARE_VERSION_CODE);
    DEBUG_RAW("Released: %s - %s\r\n", __RELEASE_DATE_KEIL__, __RELEASE_TIME_KEIL__);
    DEBUG_RAW("***********************************************************\r\n");
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	05/09/2015
 * @version	:
 * @reviewer:	
 */
static void RCC_Config(void)
{
#if 0
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART4 | RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG | RCC_APB2Periph_USART1, ENABLE);
			
	/* Setup SysTick Timer for 1 msec interrupts */ 
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 		
		/* Capture error */
		NVIC_SystemReset();
	} 
	NVIC_SetPriority(SysTick_IRQn, 0x00);
#else
    
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);
    
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
#endif
}

/*****************************************************************************/
/**
 * @brief	:  Ham khoi tao cac chan I/O khac
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	02/03/2016
 * @version	:
 * @reviewer:	
 */
static void InitIO(void)
{
    gpio_mode_set(GPS_POWER_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPS_POWER_PIN);
    gpio_output_options_set(GPS_POWER_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPS_POWER_PIN);
    GPS_POWER_OFF();
    xSystem.Status.isESP32Enable = 1;
}

/**
  * @brief  ADC1 channel configuration
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
#if 0
	ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
	
	/* ADC1 DeInit */  
	ADC_DeInit(ADC1);

	/* GPIOC Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	/* ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* DMA1 clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

	/* DMA1 Channel1 Config */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_volume;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* DMA1 Channel1 enable */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* Configure ADC Channel8 as analog input */
	GPIO_InitStructure.GPIO_Pin = ADC_VIN_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(ADC_VIN_PORT, &GPIO_InitStructure);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure); 

	/* Convert the ADC1 Channel8 with 55.5 Cycles as sampling time */ 
	ADC_ChannelConfig(ADC1, ADC_VIN_CHANNEL , ADC_SampleTime_55_5Cycles); 

	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);  

	/* Enable the ADC peripheral */
	ADC_Cmd(ADC1, ENABLE);     
  
	/* Wait the ADRDY flag */
	uint32_t Timeout = 0xFFFFFFF;
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY) && Timeout) {
		Timeout--;
		if(Timeout % 10000) ResetWatchdog();
	}	  

	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
#else
    gpio_mode_set(ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO_PIN);

//    /* ADC_DMA_channel configuration */
//    dma_parameter_struct dma_data_parameter;

//    /* ADC DMA_channel configuration */
//    dma_deinit(DMA_CH0);

//    /* initialize DMA single data mode */
//    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA);
//    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
//    dma_data_parameter.memory_addr = (uint32_t)(&adc_volume);
//    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_DISABLE;
//    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
//    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
//    dma_data_parameter.number = 1U;
//    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
//    dma_init(DMA_CH0, &dma_data_parameter);

//    dma_circulation_enable(DMA_CH0);

    /* ADC contineous function enable */
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);

    /* ADC regular channel config */
    adc_regular_channel_config(0U, BOARD_ADC_CHANNEL, ADC_SAMPLETIME_55POINT5);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable();
    Delayms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable();

    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        
    /* ADC DMA function enable */
//    adc_dma_mode_enable();
    /* ADC software trigger enable */
        
    /* enable DMA channel */
#endif
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	05/09/2015
 * @version	:
 * @reviewer:	
 */
static void InitVariable(void)
{
    xSystem.Driver.UART = &DriverUART;
    xSystem.Debug = &DriverDebug;
    //    xSystem.Driver.InternalFlash = &DriverInternalFlash;
}

///*****************************************************************************/
///**
//  * @brief  Retargets the C library DEBUG_INFO function to the USART.
//  * @param  None
//  * @retval None
//  */
//PUTCHAR_PROTOTYPE
//{
//	xSystem.Driver.UART->Putc(DEBUG_UART, ch);
//	return ch;
//}

/********************************* END OF FILE *******************************/
