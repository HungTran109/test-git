/******************************************************************************
 * @file    	GPS.c
 * @author  	
 * @version 	V1.0.0
 * @date    	10/11/2014
 * @brief   	
 ******************************************************************************/


/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include <stdio.h>
#include "GPS.h"
#include "Hardware.h"
#include "Main.h"
#include "app_debug.h"
#include <string.h>

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


#define	GGA	0x00
#define	GGL	0x01
#define	GSA	0x02
#define	GSV	0x03
#define	RMC	0x04
#define	VTG	0x05

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
GPS_Manager_t GPS_Manager;

static uint8_t GPSHasDataTimeout = 10;
static uint8_t SendConfigL76BStep = 0;
static uint8_t GPSTimeout1000ms;

/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/
void GPS_Reset(void);

/*****************************************************************************/
/**
 * @brief	:  Tick 10ms
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void GPS_ManagerTick(void) 
{ 
    if (GPS_Manager.NewGPSData) 
	{
		GPSHasDataTimeout = 10;	//10s
		
		char *p = strstr((char*)GPS_Manager.DataBuffer.Buffer, "$GNRMC");
        if (!p)
        {
            p = strstr((char*)GPS_Manager.DataBuffer.Buffer, "$GPRMC");
        }
        char *q = NULL;
        if (p)
        {
            q = strstr(p, "\r\n");
        }
		if (p && q)
		{
            static uint32_t counter = 0;
            if (counter++ == 5)
            {
                counter = 0;
                DEBUG_INFO("%.*s", q - p + 2, p);
            }
		}
							
		GPS_ProcessNewMessage();		         
		GPS_Manager.NewGPSData = 0;
		GPS_Manager.DataBuffer.BufferIndex = 0;
		
		/* Send config command to Quectel L26 : Output only $GPRMC */
		if(SendConfigL76BStep < 5)
		{	
			UART_Puts(GPS_UART, "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
		}						
		if(SendConfigL76BStep < 5) SendConfigL76BStep++; 
	}
	
	/* Process 1000ms */
	if(GPSTimeout1000ms++ >= 100)
	{
		GPSTimeout1000ms = 0;
			
		if(GPS_Manager.Valid) GPS_Manager.Valid--;
		
		if(GPSHasDataTimeout)
		{
			GPSHasDataTimeout--;
						
			/* Mat du lieu duong truyen UART */
			if(GPSHasDataTimeout == 0)
			{
				GPS_Reset();
			}
		}
	}
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void InitGPS(void) 
{ 	
    UART_Init(GPS_UART, 9600);
}

void GPS_Reset(void)
{
	SendConfigL76BStep = 0;
}
	
/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
PointStruct_t GPS_GetPosition(void) 
{ 
	PointStruct_t tmpPoint;
		
	if(GPS_Manager.Valid)
	{
		tmpPoint = GPS_Manager.CurrentPosition;
	}
	else
	{
		tmpPoint.KinhDo.value = 0;
		tmpPoint.ViDo.value = 0;				
	}
		
	return tmpPoint;
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
uint8_t GPS_GetSpeed(void)
{		
	if(GPS_Manager.Valid == 0)
		GPS_Manager.CurrentSpeed = 0;
		
	return GPS_Manager.CurrentSpeed;
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
DateTime_t GPS_GetDateTime (void)
{	
	DateTime_t tmpDateTime;
	
	if(GPS_Manager.Valid == 0)			
	{		
		GPS_Manager.CurrentDateTime.Day = 0;
		GPS_Manager.CurrentDateTime.Month = 0;
		GPS_Manager.CurrentDateTime.Year = 0;
		GPS_Manager.CurrentDateTime.Hour = 0;
		GPS_Manager.CurrentDateTime.Minute = 0;
		GPS_Manager.CurrentDateTime.Second = 0;		
	}
	tmpDateTime = GPS_Manager.CurrentDateTime;
	
	return tmpDateTime;
}
/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
uint8_t GPS_IsValid (void)
{
    return GPS_Manager.Valid;
}

uint8_t GPS_IsLostData(void)
{
	if(GPSHasDataTimeout == 0) return 1;
	return 0;
}

static void GPS_AddNewData(uint8_t Data)
{ 
	GPS_Manager.TimeOutConnection = 0;	
	
	GPS_Manager.DataBuffer.Buffer[GPS_Manager.DataBuffer.BufferIndex++] = Data;
	
	if(GPS_Manager.DataBuffer.BufferIndex > 5 && 
		GPS_Manager.DataBuffer.Buffer[GPS_Manager.DataBuffer.BufferIndex - 2] == '\r' &&
		GPS_Manager.DataBuffer.Buffer[GPS_Manager.DataBuffer.BufferIndex - 1] == '\n')
	{
		GPS_Manager.NewGPSData = 1;
	}
	
	if(GPS_Manager.DataBuffer.BufferIndex >= SMALL_BUFFER_SIZE) GPS_Manager.DataBuffer.BufferIndex = 0;
	GPS_Manager.DataBuffer.Buffer[GPS_Manager.DataBuffer.BufferIndex] = 0;
}

void USART0_IRQHandler(void)
{
    volatile int handle = 0;
    if (USART_GetITStatus(USART0, USART_IT_RXNE) != RESET)
    {
        uint8_t rx = USART_ReceiveData(USART0);
        GPS_AddNewData(rx);
//        SEGGER_RTT_Write(0, &rx, 1);
        handle++;
    }
    
//    uint32_t tmp = USART_CTL0(USART0);
        

    if (RESET != usart_flag_get(USART0, USART_FLAG_ORERR))
    {
//        usart_data_receive(USART0);
		usart_flag_clear(USART0, USART_FLAG_ORERR);
        DEBUG_INFO("GPS ORE\r\n");
        handle++;
    } 	
    
    if (RESET != usart_flag_get(USART0, USART_FLAG_FERR))
    {
        usart_data_receive(USART0);
		usart_flag_clear(USART0, USART_FLAG_FERR);
        DEBUG_INFO("GPS FE\r\n");
        handle++;
    }    

    if (RESET != usart_flag_get(USART0, USART_FLAG_NERR))
    {
        usart_data_receive(USART0);
		usart_flag_clear(USART0, USART_FLAG_NERR);
        DEBUG_INFO("GPS NE\r\n");
        handle++;
    }
    
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_PERR))
    {
		usart_flag_clear(USART0, USART_FLAG_FERR);
        DEBUG_INFO("GPS PE\r\n");
        handle++;
    }
    
//        if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_PERR))
//    {
//		usart_flag_clear(USART0, USART_FLAG_FERR);
//        DEBUG_INFO("GPS PE\r\n");
//        handle++;
//    }
//    
    

    if (handle == 0)
    {
        DEBUG_INFO("GPS UART ERR\r\n");
    }        
}

/********************************* END OF FILE *******************************/
