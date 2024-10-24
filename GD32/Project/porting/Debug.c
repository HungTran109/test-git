/******************************************************************************
 * @file    	Debug.c
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
#include "DataDefine.h"
#include "Utilities.h"
#include "HardwareManager.h"
#include "I2CFM.h"

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
#define TEST_INTERNAL_WDT 2
#define TEST_EXTERNAL_WDT 3
#define TEST_REBOOT_CORE 4

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
SmallBuffer_t DebugBuffer;
/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/
static void ProcessNewDebugData(void);

/*****************************************************************************/
/**
 * @brief	:  	Tick every 10ms
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	05/09/2015
 * @version	:
 * @reviewer:	
 */
static void DebugTick(void)
{
    if (DebugBuffer.State)
    {
        DebugBuffer.State--;
        if (DebugBuffer.State == 0)
        {
            ProcessNewDebugData();
            DebugBuffer.BufferIndex = 0;
        }
    }
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
static void DebugInit(void)
{
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	10/03/2016
 * @version	:
 * @reviewer:	
 */
static void ProcessNewDebugData(void)
{
    uint8_t MaLenh;

    DEBUG_INFO("DEBUG: %s\r\n", DebugBuffer.Buffer);

    if (strstr((char *)DebugBuffer.Buffer, "TEST,"))
    {
        MaLenh = GetNumberFromString(5, (char *)DebugBuffer.Buffer);
        switch (MaLenh)
        {
        case TEST_INTERNAL_WDT:
            DEBUG_INFO("Test Internal WDT\r\n");
            xSystem.Status.TestInternalWDT = 1;
            break;

        case TEST_EXTERNAL_WDT:
            DEBUG_INFO("Test External WDT\r\n");
            break;

        case 14:
            DEBUG_INFO("\rThuc hien RESET...\r\n");
            NVIC_SystemReset();
            break;

        default:
            break;
        }
    }

    /* Lenh doc/ghi thanh ghi FM */
    if (strstr((char *)DebugBuffer.Buffer, "SETFM,"))
    {
        uint8_t regNo = GetHexNumberFromString(6, (char *)DebugBuffer.Buffer);
        char sRegVal[10] = {0};
        if (CopyParameter((char *)&DebugBuffer.Buffer[6], sRegVal, '(', ')'))
        {
            uint8_t regVal = GetHexNumberFromString(0, sRegVal);
            uint16_t res = FM_WriteReg(regNo, regVal);
            DEBUG_INFO("Write FM reg 0x%02X to 0x%02X : %s\r\n", regNo, regVal, res == KT0935_OK ? "OK" : "ERR");
        }
    }

    if (strstr((char *)DebugBuffer.Buffer, "GETFM,"))
    {
        uint8_t regNo = GetHexNumberFromString(6, (char *)DebugBuffer.Buffer);
        uint16_t regVal = FM_ReadReg(regNo);
        if (regVal != KT0935_FAIL)
        {
            DEBUG_INFO("Read reg %02X OK, value: %02X\r\n", regNo, regVal & 0xFF);
        }
        else
        {
            DEBUG_INFO("Read reg %02X: FAIL", regNo);
        }
    }

    if (strstr((char *)DebugBuffer.Buffer, "SETVOL,"))
    {
        uint8_t volume = GetNumberFromString(7, (char *)DebugBuffer.Buffer);
        uint16_t res = FM_SetVolume(volume);
    }
    if (strstr((char *)DebugBuffer.Buffer, "SETFREQ,"))
    {
        uint32_t freq = GetNumberFromString(8, (char *)DebugBuffer.Buffer);
        uint16_t res = FM_SetFMChanFreq(freq);
    }
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
static void DebugAddData(uint8_t Data)
{
    DebugBuffer.Buffer[DebugBuffer.BufferIndex++] = Data;

    if (DebugBuffer.BufferIndex == SMALL_BUFFER_SIZE)
        DebugBuffer.BufferIndex = 0;
    DebugBuffer.Buffer[DebugBuffer.BufferIndex] = 0;

    DebugBuffer.State = 150;
}

///*******************************************************************************
// * Function Name  	: USART1_IRQHandler 
// * Return         	: None
// * Parameters 		: None
// * Created by		: Phinht
// * Date created	: 02/03/2016
// * Description		: This function handles USART1 global interrupt request. 
// * Notes			: 
// *******************************************************************************/
//void USART0_IRQHandler(void)
//{
//    if (USART_GetITStatus(USART0, USART_IT_RXNE) != RESET)
//    {
//        //Debug
//        // USART_ClearITPendingBit(USART0, USART_IT_RXNE);
//        USART_ReceiveData(USART0);
//        //DebugAddData(USART_ReceiveData(USART0));
//    }
//}

Debug_t DriverDebug =
    {
        DebugInit,
        DebugTick,
        DebugAddData};

/********************************* END OF FILE *******************************/
