/******************************************************************************
 * @file    	InternalFlash.c
 * @author  	Phinht
 * @version 	V1.0.0
 * @date    	05/09/2015
 * @brief   	
 ******************************************************************************/
 /******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include <stdio.h>
//#include "stm32f0xx_flash.h"
#include "gd32e23x.h"
#include "InternalFlash.h"
#include "Main.h"

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
 * @brief	:  Khoi tao Internal Flash
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/09/2015
 * @version	:
 * @reviewer:	
 */
static void InternalFlash_Init(void)
{	
	/* Unlock Flash */
	FLASH_Unlock();

	/* Clear All pending flags */
#if 0
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_PGERR | 
									FLASH_FLAG_WRPERR | FLASH_FLAG_EOP); 
#else
    // Clear all pending flags
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
#endif
}

/*****************************************************************************/
/**
 * @brief	: Erase sector
 * @param	:  Dia chi sector can xoa
 * @retval	:	0 if Success, > 0 if Fail
 * @author	:	Phinht
 * @created	:	15/09/2015
 * @version	:
 * @reviewer:	
 */
uint8_t InternalFlash_Prepare(uint32_t Address)
{			
	//Unlock Flash
	InternalFlash_Init();

	if (FLASH_ErasePage(Address)!= FLASH_COMPLETE)
	{
		return 1;
	}
			
	FLASH_Lock();
	return 0;
}

/*****************************************************************************/
/**
 * @brief	: Write config
 * @param	:  
 * @retval	:	0 if Success, > 0 if Fail
 * @author	:	Phinht
 * @created	:	15/09/2015
 * @version	:
 * @reviewer:	
 */
uint8_t InternalFlash_WriteConfig(void)
{
	uint8_t	 result = 0, retry = 5;
	
	//Unlock Flash
	InternalFlash_Init();

	while(retry)
	{
		if (FLASH_ErasePage(CONFIG_ADDR) != FLASH_COMPLETE)
		{
			DEBUG_INFO("CFG: Xoa bo nho cau hinh FAILED!\r\n");
			Delayms(200);
			retry--;
		}
		else
			break;
	}
	
	if(retry > 0)
	{
		DEBUG_INFO("CFG: Xoa bo nho cau hinh OK");
		
		/* Flag */
		if(FLASH_ProgramWord(CONFIG_ADDR, CONFIG_FLAG_VALUE) != FLASH_COMPLETE)
			result++;
		
		/* Operation mode */
		if(FLASH_ProgramWord(CONFIG_MODE_ADDR, xSystem.Parameter.Mode) != FLASH_COMPLETE)
			result++;
		
		/* Frequency */
		for(uint8_t i = 0; i < 5; i++) {
			if(FLASH_ProgramWord(CONFIG_FREQ_ADDR + i*4, xSystem.Parameter.Frequency[i]) != FLASH_COMPLETE)
				result++;
		}
		
		/* Frequency index */
		if(FLASH_ProgramWord(CONFIG_FREQ_INDEX_ADDR, xSystem.Parameter.FreqIndex) != FLASH_COMPLETE)
			result++;		
		
		/* Remote volume */
		if(FLASH_ProgramWord(CONFIG_VOLUME_ADDR, xSystem.Parameter.RemoteVolume) != FLASH_COMPLETE)
			result++;
		
		/* Local mode */
		if(FLASH_ProgramWord(CONFIG_LOCAL_MODE_ADDR, xSystem.Parameter.ModeLocal) != FLASH_COMPLETE)
			result++;
		
		/* Display delay sleep */
		if(FLASH_ProgramWord(CONFIG_DISP_DELAY_ADDR, xSystem.Parameter.DispDelay) != FLASH_COMPLETE)
			result++;
		
		if(result == 0)
			DEBUG_INFO("CFG: Luu cau hinh : %u - %s\r\n", result, result == 0 ? "OK" : "FAIL");
	}
	
	FLASH_Lock();
	
	if(retry == 0) return 1;
	
	return result;
}

/*****************************************************************************/
/**
 * @brief	: Read config
 * @param	:  
 * @retval	:	0 if Success, > 0 if Fail
 * @author	:	Phinht
 * @created	:	15/09/2015
 * @version	:
 * @reviewer:	
 */
void InternalFlash_ReadConfig(void)
{
	if(*(__IO uint32_t*)(CONFIG_ADDR) != CONFIG_FLAG_VALUE)
	{
		DEBUG_INFO("CHUA CO CAU HINH, DUNG CAU HINH MAC DINH!\r\n");
		
//		xSystem.Parameter.Mode = MODE_IDLE;
//		xSystem.Parameter.ModeLocal = MODE_IDLE;
		xSystem.Parameter.FreqIndex = 0;
		xSystem.Parameter.Frequency[0] = 10000;	//VOV1	100MHz
		xSystem.Parameter.Frequency[1] = 10270;	//VOV2	102.7MHz
		xSystem.Parameter.Frequency[2] = 9650;	//VOV3	96.5MHz
		xSystem.Parameter.LocalVolume = 95;
		xSystem.Parameter.LEDBrightness = LED_BRIGHTNESS_100;
		xSystem.Parameter.DispDelay = 0;
        xSystem.Status.FM.State = 0;
	}
	else 
	{
//		xSystem.Parameter.Mode = *(__IO uint32_t*)(CONFIG_MODE_ADDR);
		xSystem.Parameter.FreqIndex = *(__IO uint32_t*)(CONFIG_FREQ_INDEX_ADDR);
		xSystem.Parameter.RemoteVolume = *(__IO uint32_t*)(CONFIG_VOLUME_ADDR);
		for(uint8_t i = 0; i < 5; i++)
		{
			xSystem.Parameter.Frequency[i] = *(__IO uint32_t*)(CONFIG_FREQ_ADDR + i*4);
		}
		xSystem.Parameter.LEDBrightness = *(__IO uint32_t*)(CONFIG_LEDBRIGHT_ADDR);
		if(xSystem.Parameter.LEDBrightness == 0 || xSystem.Parameter.LEDBrightness > LED_BRIGHTNESS_100) {
			xSystem.Parameter.LEDBrightness = LED_BRIGHTNESS_100;
		}
		xSystem.Parameter.DispDelay = *(__IO uint32_t*)(CONFIG_DISP_DELAY_ADDR);
		if(xSystem.Parameter.DispDelay == 0xFFFF) xSystem.Parameter.DispDelay = 0;
	}
	
	/** Mac dinh khi khoi dong STOP module -> Khi co lenh tu master thi thay doi che do
	* => Chong hanh vi rut bo FM ra hoat dong rieng!
	*/
	xSystem.Parameter.Mode = MODE_NONE;
	xSystem.Parameter.ModeLocal = MODE_IDLE;
		
	if(xSystem.Parameter.DispDelay >= 5)
		xSystem.Status.DelaySleepLED7 = xSystem.Parameter.DispDelay;
	else
		xSystem.Status.DelaySleepLED7 = 0;
	
	DEBUG_INFO("Master Mode: %u, Local Mode: %u\r\n", xSystem.Parameter.Mode, xSystem.Parameter.ModeLocal); 
	DEBUG_INFO("Freq: %u - %u - %u\r\n", xSystem.Parameter.Frequency[0], xSystem.Parameter.Frequency[1],
		xSystem.Parameter.Frequency[2]);
	DEBUG_INFO("Freq index: %u\r\n", xSystem.Parameter.FreqIndex);
	DEBUG_INFO("Volume: %u\r\n", xSystem.Parameter.RemoteVolume);
	DEBUG_INFO("Display sleep delay: %u\r\n", xSystem.Parameter.DispDelay);
}

/*****************************************************************************/
/**
 * @brief	: Write config
 * @param	:  
 * @retval	:	0 if Success, > 0 if Fail
 * @author	:	Phinht
 * @created	:	15/09/2015
 * @version	:
 * @reviewer:	
 */
uint8_t InternalFlash_WriteVehicleCount(uint32_t vehicleCount)
{
	uint8_t	 result = 0, retry = 5;
	
//	FLASH_Status Status = FLASH_COMPLETE;
	
	//Unlock Flash
	InternalFlash_Init();

	while(retry)
	{
		if (FLASH_ErasePage(VEHICLE_COUNT_ADDR) != FLASH_COMPLETE)
		{
			DEBUG_INFO("VEHICLE: Xoa bo nho FAILED!\r\n");
			Delayms(200);
			retry--;
		}
		else
			break;
	}
	
	if(retry > 0)
	{
		DEBUG_INFO("VEHICLE: Xoa bo nho Vehicle counter OK\r\n");
		
		if(FLASH_ProgramWord(VEHICLE_COUNT_ADDR, VEHICLE_COUNT_FLAG_VALUE) != FLASH_COMPLETE)
			result++;
		
		if(FLASH_ProgramWord(VEHICLE_COUNT_NUM_ADDR, vehicleCount) != FLASH_COMPLETE)
			result++;
		
		if(result == 0)
			DEBUG_INFO("VEHICLE: Luu counter OK....\r\n");
	}
	
	FLASH_Lock();
	
	if(retry == 0) return 1;
	else 
		return result;
}

/*****************************************************************************/
/**
 * @brief	:   Write long number to internal flash
 * @param	:   Address, Data
 * @retval	:   0 if success, 1 if error
 * @author	:	
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
uint8_t InternalFlash_WriteLong(uint32_t Address, uint32_t Data) 
{
    FLASH_Status Status;
    
	//Unlock Flash
	InternalFlash_Init();
	Status = FLASH_ProgramWord(Address, Data);
	FLASH_Lock();      

	if(Status != FLASH_COMPLETE) return 1;
	return 0;
}

///*****************************************************************************/
///**
// * @brief	: Write bytes to internal flash
// * @param	:  Address, pBuffer, Length
// * @retval	:	0 if Success, > 0 if Fail
// * @author	:	Phinht
// * @created	:	15/09/2015
// * @version	:
// * @reviewer:	
// */
//uint8_t InternalFlash_WriteBytes(uint32_t Address, uint8_t *pBuffer, uint16_t Length)
//{
//	Int_t tmp;
//	uint16_t	i = 0;
//    
//    extern void app_wdt_feed();
//	//Unlock Flash
//	InternalFlash_Init();
//	
//	for(i = 0; i < Length; i++)
//	{
//		if(i > 0 && i%100 == 0)
//			app_wdt_feed();

//		tmp.bytes[0] = pBuffer[2*i];
//		tmp.bytes[1] = pBuffer[2*i+1];
//		if(FLASH_ProgramHalfWord(Address + 2*i, tmp.value) == FLASH_COMPLETE)
//		{
//			if(tmp.value != *(uint16_t *)(Address + 2*i))	//Verify
//				return 1;
//		}
//		else
//			return 2;
//	}
//		
//	FLASH_Lock();
//	return 0;
//}


/*****************************************************************************/
/**
 * @brief	:   Copy data in internal flash
 * @param	:   SrcAddress, DstAddress, Length
 * @retval	:   0 if success, > 0 if error
 * @author	:	Phinht
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
//uint8_t CopyDataInternalFlash(uint32_t DesAddress,uint32_t SourceAddress, uint32_t Length)
//{
//    uint32_t i;
//    FLASH_Status Status = FLASH_COMPLETE;
//    
//	 //Unlock Flash
//	InternalFlash_Init();
//	
//    DEBUG_INFO("Bat dau copy tu dia chi 0x%X den dia chi 0x%X, luong du lieu can copy: %d Bytes", SourceAddress, DesAddress, Length);
//	
//    for(i = 0; (i < Length) && (Status == FLASH_COMPLETE); i += 4, DesAddress += 4, SourceAddress += 4)
//    {         			
//		Status = FLASH_ProgramWord(DesAddress, *(__IO uint32_t*) SourceAddress);

//        if((*(__IO uint32_t*) SourceAddress) != (*(__IO uint32_t*) DesAddress)) 
//        {   
//            DEBUG_INFO("Khong copy duoc du lieu, Des: %d, Source: %d",*(__IO uint32_t*) DesAddress,*(__IO uint32_t*) SourceAddress);
//            return 0;
//        }
//		//Reset IWDG
//		if(i > 0 && (i%100 == 0))
//			IWDG_ReloadCounter();			
//    }
//    DEBUG_INFO("Copied xong %d Bytes", Length);
//	
//	FLASH_Lock();   
//    return 1;
//}

