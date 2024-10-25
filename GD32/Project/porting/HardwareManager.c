/******************************************************************************
 * @file    	HardwareManager.c
 * @author  	Phinht
 * @version 	V1.0.0
 * @date    	05/09/2015
 * @brief   	
 ******************************************************************************/


/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "Main.h"
#include "DataDefine.h"
#include "Hardware.h"
#include "HardwareManager.h"

/******************************************************************************
                                   GLOBAL VARIABLES					    			 
 ******************************************************************************/
extern System_t xSystem;

/******************************************************************************
                                   GLOBAL FUNCTIONS					    			 
 ******************************************************************************/

/******************************************************************************
                                   DATA TYPE DEFINE					    			 
 ******************************************************************************/

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	05/09/2015
 * @version	:
 * @reviewer:	
 */
void DetectResetReason(void) 
{
//	xSystem.HardwareInfo.ResetReasion = 0;
//		
//	DEBUG_INFO("\rNguyen nhan Reset: ");
//	if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
//    {
////        xSystem.HardwareInfo.ResetReasion |= 1;
//		DEBUG_INFO("PIN, ");
//    }

//    if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
//    {
////        xSystem.HardwareInfo.ResetReasion |= 4;
//		DEBUG_INFO("SFT, ");
//    }

//    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
//    {
////        xSystem.HardwareInfo.ResetReasion |= 8;
//		DEBUG_INFO("IWD, ");
//    }

//    if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
//    {
////        xSystem.HardwareInfo.ResetReasion |= 16;
//		DEBUG_INFO("WWDG, ");
//    }

//    if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET)
//    {
////        xSystem.HardwareInfo.ResetReasion |= 32;
//		DEBUG_INFO("LPWR");
//    }		
//		
//    if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
//    {
////        xSystem.HardwareInfo.ResetReasion |= 2;
//		DEBUG_INFO("POR, ");
//    }
//	
//	RCC_ClearFlag();
}

void DetectHardwareVersion(void)
{
	
}
/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
/*      
    Ma          Ham goi 
    0   :   ResetSystemManagement 
    1   :   CheckInitDone
    2   :   ProcessCMD
    3   :   SysTick_Handler     : GSMResetCount
    4   :   SysTick_Handler     : MainLoopCount = 120
    5   :   PingProcess         
    6   :   TestHardware
    7   :   ProcessSetConfig    :   CFG_RESET
*/
void SystemReset(uint8_t NguyenNhanReset)
{ 
    NVIC_SystemReset();
}

#if 0
void GPIOToggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIO_WriteBit(GPIOx, GPIO_Pin, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin)));
}
#endif

void WatchDogInit(void)
{
#if 0
	uint32_t LsiFreq = 40000;

	/* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* IWDG counter clock: LSI/32 */
	IWDG_SetPrescaler(IWDG_Prescaler_128);

	/* Set counter reload value to obtain 250ms IWDG TimeOut.
	Counter Reload Value = 250ms/IWDG counter clock period
				  = 250ms / (1/LSI/32)
				  = 0.25s / (1/LsiFreq/32)
				  = LsiFreq/(32 * 4)
				  = LsiFreq/128
	*/
	IWDG_SetReload(LsiFreq/32);	//TimeOut 1s

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
#endif
}
	

void ResetWatchdog(void)
{
#if 0
	/* Reload IWDG counter */
	if(xSystem.Status.TestInternalWDT == 0)
	{
		IWDG_ReloadCounter();  
	}
#endif
}

/*******************************************************************************
 * Function Name	: Hardware_XoaCoLoi
 * Return			: None
 * Parameters		: None
 * Description		: Xoa cac co loi cua USART
*******************************************************************************/
void Hardware_XoaCoLoi(void)
{	
    // OverRun Error Flag	
    if(USART_GetFlagStatus(USART0, USART_FLAG_ORE) != RESET)
    {
        DEBUG_INFO("USART0: ORE set!\r\n");
        USART_ClearFlag(USART0, USART_FLAG_ORE);
    }

    //Framing Error Flag
    if(USART_GetFlagStatus(USART0, USART_FLAG_FE) != RESET)
    {
        DEBUG_INFO("USART0: FE set!\r\n");
        USART_ClearFlag(USART0, USART_FLAG_FE);
    }

    // OverRun Error Flag	
    if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        DEBUG_INFO("USART1: ORE set!\r\n");
        USART_ClearFlag(USART1, USART_FLAG_ORE);
    }

    //Framing Error Flag
    if(USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
    {
        DEBUG_INFO("USART1: FE set!\r\n");
        USART_ClearFlag(USART1, USART_FLAG_FE);
    }
}


/********************************* END OF FILE *******************************/
