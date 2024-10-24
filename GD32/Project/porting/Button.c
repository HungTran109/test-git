/******************************************************************************
 * @file    	Led7.c
 * @author  	
 * @version 	V1.0.0
 * @date    	15/01/2014
 * @brief   	
 ******************************************************************************/
/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "DataDefine.h"
#include "Hardware.h"
#include "Led7.h"
#include "Indicator.h"
#include "InternalFlash.h"
//#include "MemoryManagement.h"
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
#define ADC1_DR_Address 0x40012440

#define KEY_PRESSED 0
#define KEY_RELEASED 1

#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_SETUP_TIMEOUT 100 /* 10s */

#define ADC_VOLUME_MIN 2162 //1955
#define ADC_VOLUME_MAX 4086
#define ADC_VOLUME_STEP 19.8 //21.4 <=> (4095 - 1955)/100 muc volume

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
uint8_t keyUpState = KEY_RELEASED;
uint8_t keyDownState = KEY_RELEASED;
uint8_t keyLeftState = KEY_RELEASED;
uint8_t keyRightState = KEY_RELEASED;

uint16_t setupModeTimeout;
uint16_t keyEnterHoldTimeout = 0;
uint16_t keyCancelHoldTimeout = 0;

KeySetup_t Setup;

__IO uint32_t encodeACount = 0;
__IO uint32_t encodeBCount = 0;
extern __IO uint16_t adc_volume;
uint16_t ADCVolumeAvg[2] = {0};

/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/
static void ADC_Config(void);

uint16_t AVGADCVolume(uint16_t curADCVolume)
{
    //Khoi tao ban dau
    if (ADCVolumeAvg[0] == 0 && ADCVolumeAvg[1] == 0)
    {
        ADCVolumeAvg[1] = curADCVolume;
    }

    ADCVolumeAvg[0] = ADCVolumeAvg[1];
    ADCVolumeAvg[1] = ADCVolumeAvg[0] + (curADCVolume - ADCVolumeAvg[0]) / 10;

    return ADCVolumeAvg[1];
}

//static void clearSetupParam(void)
//{
//	keyEnterHoldTimeout = 0;
//	setupModeTimeout = 0;
//	xSystem.Status.isSetupState = 0;
//	memset(Setup.DigitArray, 0, DIGIT_ARRAY_SIZE);
//	Setup.index = 0;
//}

/*****************************************************************************/
/**
* @brief   : Dieu khien cac phim setup: chon che do hoat dong: FM, MIC
 * @param   :  
 * @retval  :
 * @author  :   
 * @created :   15/01/2014
 * @version :
 * @reviewer:   
 */
static void XuLyPhimSetup(uint8_t keyCode)
{
    //reset setup timeout
    setupModeTimeout = 150; /* 15s */
    xSystem.Parameter.LEDBrightness = LED_BRIGHTNESS_100;

    /* Sáng màn hình LED7 */
    if (xSystem.Parameter.DispDelay >= 5)
        xSystem.Status.DelaySleepLED7 = xSystem.Parameter.DispDelay;
    else
        xSystem.Status.DelaySleepLED7 = 0;

    /* Neu Master dang IDLE thi moi cho dieu khien setup */
    if (xSystem.Parameter.Mode != MODE_IDLE)
    {
        return;
    }

    DEBUG_INFO("Button event %d\r\n", keyCode);
    switch (keyCode)
    {
    case KEY_UP:
        //Neu trong man hinh setup -> thay doi tact time
        if (xSystem.Status.isSetupState == 1)
        {
            if (Setup.DigitArray[Setup.index] < 9)
                Setup.DigitArray[Setup.index]++;
        }
        else
        {
            /* Chuyen qua lai mode : FM1, FM2, FM3, MIC, IDLE */
            if (Setup.modeIndex == MODE_ARRAY_SIZE - 1)
                Setup.modeIndex = 0;
            else
                Setup.modeIndex++;

            Setup.PressKeyTimout = KEY_SETUP_TIMEOUT; /* 10s */
            Led7_DispSetupMode(Setup.modeIndex, 1);
        }
        break;

    case KEY_DOWN:
        //Neu trong man hinh setup -> thay doi tact time
        if (xSystem.Status.isSetupState == 1)
        {
            if (Setup.DigitArray[Setup.index] > 0)
                Setup.DigitArray[Setup.index]--;
        }
        else
        {
            /* Chuyen qua lai mode : FM1, FM2, FM3, MIC, IDLE */
            if (Setup.modeIndex == 0)
                Setup.modeIndex = MODE_ARRAY_SIZE - 1;
            else
                Setup.modeIndex--;

            Setup.PressKeyTimout = KEY_SETUP_TIMEOUT; /* 10s */
            Led7_DispSetupMode(Setup.modeIndex, 1);
        }
        break;

    case KEY_LEFT:
        //Thoat che do cai dat
        //			if(xSystem.Status.isSetupState == 1) {
        //				clearSetupParam();
        //			}

        /* Huy cai dat, restore mode ve nhu luc dau */
        Setup.PressKeyTimout = 0;
        Setup.modeIndex = Setup.lastModeIndex;
        Led7_DispSetupMode(Setup.modeIndex, 0);
        break;

    case KEY_RIGHT:
        //			//Chuyen sang dieu chinh so ke tiep
        //			if((Setup.index == DIGIT_ARRAY_SIZE - 1) || Setup.index == 0xFF)
        //				Setup.index = 0;
        //			else
        //				Setup.index++;

        /* Chap nhan cai dat -> chuyen che do local */
        if (Setup.PressKeyTimout)
        {
            if (Setup.modeIndex <= LOCAL_FM3)
            {
                xSystem.Parameter.FreqIndex = Setup.modeIndex;
                xSystem.Parameter.ModeLocal = MODE_FM_LOCAL;
                xSystem.Parameter.FreqRun = xSystem.Parameter.Frequency[Setup.modeIndex];

                /* Setup Freq if it's valid frequency */
                if (xSystem.Parameter.Frequency[xSystem.Parameter.FreqIndex] > 100)
                {
                    KT093x_User_FMTune(xSystem.Parameter.Frequency[xSystem.Parameter.FreqIndex]); //MHz x100
                    FM_SetVolume(xSystem.Parameter.LocalVolume);
                }
            }
            else if (Setup.modeIndex == LOCAL_MIC)
            {
                xSystem.Parameter.ModeLocal = MODE_MIC;
            }
            else if (Setup.modeIndex == LOCAL_LINE)
            {
                xSystem.Parameter.ModeLocal = MODE_LINE_IN;
            }
            else if (Setup.modeIndex == LOCAL_IDLE)
            {
                xSystem.Parameter.ModeLocal = MODE_IDLE;
            }

            Setup.PressKeyTimout = 0;
            Setup.lastModeIndex = Setup.modeIndex;

            Led7_DispSetupMode(Setup.modeIndex, 0);
        }
        break;

    default:
        break;
    }
}

/*****************************************************************************/
/**
 * @brief   :   called every 10ms
 * @param   :  
 * @retval  :
 * @author  :   
 * @created :   15/01/2014
 * @version :
 * @reviewer:   
 */
void Volume_Tick(void)
{
    static uint8_t oldVolumeLevel = 0;
    static uint8_t curVolumeLevel = 0;

    static uint8_t Timeout250ms = 0;
    static uint8_t Timeout1000ms = 0;

    uint16_t avgAdcVolume = AVGADCVolume(adc_volume);
	if (avgAdcVolume > ADC_VOLUME_MAX)
	{
		avgAdcVolume = ADC_VOLUME_MAX;
	}
	else if (avgAdcVolume < ADC_VOLUME_MIN)
	{
		avgAdcVolume = ADC_VOLUME_MIN;
	}

    if (Timeout250ms++ >= 25)
    {
        Timeout250ms = 0;

        curVolumeLevel = (avgAdcVolume - ADC_VOLUME_MIN) / ADC_VOLUME_STEP;
//        uint8_t diff;
//        if (curVolumeLevel > oldVolumeLevel)
//        {
//            diff = curVolumeLevel - oldVolumeLevel;
//        }
//        else
//        {
//            diff = oldVolumeLevel - curVolumeLevel;
//        }
        
        if (oldVolumeLevel != curVolumeLevel)
        {
            DEBUG_INFO("Volume change %u -> %u\r\n", oldVolumeLevel, curVolumeLevel);
            oldVolumeLevel = curVolumeLevel;
            xSystem.Parameter.LocalVolume = curVolumeLevel;

            /* Sáng màn hình LED7 */
            if (xSystem.Parameter.DispDelay >= 5)
                xSystem.Status.DelaySleepLED7 = xSystem.Parameter.DispDelay;
            else
                xSystem.Status.DelaySleepLED7 = 0;

            /** Neu dang chay che do FM Local thi moi update volume cho IC FM */
            if (xSystem.Parameter.Mode == MODE_IDLE)
            {
                if (xSystem.Parameter.ModeLocal == MODE_FM_LOCAL)
                {
                    //Set volume for IC
                    FM_SetVolume(curVolumeLevel);
                }
                //Hien thi volume
                Led7_DispVolume(curVolumeLevel);
            }

#if 1
            //TEST Update muc am luong de hien thi den Volume level
            if (curVolumeLevel < 70)
                xSystem.Status.AudioLevel = 0;
            else
            {
                xSystem.Status.AudioLevel = (curVolumeLevel - 70) / 5;
            }
#endif
        }
    }

#if 1 //TEST Volume ADC
    if (Timeout1000ms++ >= 100)
    {
        Timeout1000ms = 0;

        // DEBUG_INFO("ADC: %d\r\n", avgAdcVolume);
    }
#endif
}

/*
* Called 100ms
*/
void Button_Tick(void)
{
    uint8_t tmpKeyUpState = GPIO_ReadInputDataBit(BUTTON_UP_PORT, BUTTON_UP_PIN);
    uint8_t tmpKeyDownState = GPIO_ReadInputDataBit(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN);
    uint8_t tmpKeyLeftState = GPIO_ReadInputDataBit(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);
    uint8_t tmpKeyRightState = GPIO_ReadInputDataBit(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);

    //	//Xu ly phim START & PAUSE
    //	if(tmpKeyStartState ==  KEY_PRESSED && tmpKeyPauseState == KEY_PRESSED)
    //	{
    //		DEBUG_INFO("\rKEY: Bam 2 phim cung luc, khong xu ly!");
    //	}
    //	else
    //	{
    //		if(tmpKeyStartState != keyStartState)
    //		{
    //			keyStartState = tmpKeyStartState;
    //			if(keyStartState == KEY_PRESSED)
    //			{
    //				DEBUG_INFO("\rKEY: Nhan phim START!");
    //			}
    //			else {
    //				DEBUG_INFO("\rKEY: Nha phim START!");
    //				XuLyPhimStartPause(KEY_START);
    //			}
    //		}
    //
    //		if(tmpKeyPauseState != keyPauseState)
    //		{
    //			keyPauseState = tmpKeyPauseState;
    //			if(keyPauseState == KEY_PRESSED)
    //			{
    //				DEBUG_INFO("\rKEY: Nhan phim PAUSE!");
    //			}
    //			else {
    //				DEBUG_INFO("\rKEY: Nha phim PAUSE!");
    //				XuLyPhimStartPause(KEY_PAUSE);
    //			}
    //		}
    //	}
    //
    //	//Xu ly cac phim setup
    //	if(tmpKeyEnterState != keyEnterState)
    //	{
    //		keyEnterState = tmpKeyEnterState;
    //		if(keyEnterState == KEY_RELEASED)
    //		{
    //			//Nha phim Enter -> neu trong che do Setup => Xu ly
    //			if(xSystem.Status.isSetupState == 1) {
    //				XuLyPhimSetup(KEY_ENTER);
    //			}
    //			keyEnterHoldTimeout = 0;
    //		}
    //		else {
    //			//Bat dau dem thoi gian nhan giu phim Enter
    //			if(keyEnterHoldTimeout == 0)
    //				keyEnterHoldTimeout = 1;
    //		}
    //	}
    //
    //	//Xu ly nhan giu phim CANCEL
    //	if(tmpKeyCancelState != keyCancelState)
    //	{
    //		keyCancelState = tmpKeyCancelState;
    //		if(keyCancelState == KEY_PRESSED)
    //		{
    //			DEBUG_INFO("\rKEY: Nhan phim CANCEL!");
    //			//Bat dau dem thoi gian nhan giu phim CANCEL neu khong o che do setup
    //			if(xSystem.Status.isSetupState == 0 && keyCancelHoldTimeout == 0) {
    //				keyCancelHoldTimeout = 1;
    //			}
    //		}
    //		else {
    //			DEBUG_INFO("\rKEY: Nha phim CANCEL!");
    //			if(xSystem.Status.isSetupState == 1) {
    //				XuLyPhimSetup(KEY_CANCEL);
    //			}
    //			keyCancelHoldTimeout = 0;
    //		}
    //	}

    //Xet cac phim UP, DOWN
    if (tmpKeyUpState != keyUpState)
    {
        keyUpState = tmpKeyUpState;
        if (keyUpState == KEY_PRESSED)
        {
            DEBUG_INFO("KEY: Nhan phim UP!\r\n");
        }
        else
        {
            DEBUG_INFO("KEY: Nha phim UP!\r\n");
            XuLyPhimSetup(KEY_UP);
        }
    }

    if (tmpKeyDownState != keyDownState)
    {
        keyDownState = tmpKeyDownState;
        if (keyDownState == KEY_PRESSED)
        {
            DEBUG_INFO("KEY: Nhan phim DOWN!\r\n");
        }
        else
        {
            DEBUG_INFO("KEY: Nha phim DOWN!\r\n");
            XuLyPhimSetup(KEY_DOWN);
        }
    }

    //Xet cac phim LEFT, RIGHT
    if (tmpKeyLeftState != keyLeftState)
    {
        keyLeftState = tmpKeyLeftState;
        if (keyLeftState == KEY_PRESSED)
        {
            DEBUG_INFO("KEY: Nhan phim LEFT!\r\n");
        }
        else
        {
            DEBUG_INFO("KEY: Nha phim LEFT!\r\n");
            XuLyPhimSetup(KEY_LEFT);
        }
    }

    if (tmpKeyRightState != keyRightState)
    {
        keyRightState = tmpKeyRightState;
        if (keyRightState == KEY_PRESSED)
        {
            DEBUG_INFO("KEY: Nhan phim RIGHT\r\n");
        }
        else
        {
            DEBUG_INFO("KEY: Nha phim RIGHT\r\n");
            XuLyPhimSetup(KEY_RIGHT);
        }
    }

    /* Timeou bam phim UP/DOWN */
    if (Setup.PressKeyTimout)
    {
        Setup.PressKeyTimout--;
        if (Setup.PressKeyTimout == 0)
        {
            DEBUG_INFO("KEY: Timeout setup...\r\n");
            Led7_DispSetupMode(Setup.lastModeIndex, 0);
        }
    }

    //	//Nhan giu ENTER >= 3s -> vao che do SETUP
    //	if(keyEnterHoldTimeout)
    //	{
    //		keyEnterHoldTimeout++;
    //		if(keyEnterHoldTimeout >= 30)
    //		{
    //			keyEnterHoldTimeout = 0;
    //
    //			//Vao che do Setup
    //			if(xSystem.Status.isSetupState == 0)
    //			{
    //				xSystem.Status.isSetupState = 1;
    //				setupModeTimeout = 600;	//60s
    //
    //				//Vi tri so chinh sua dau tien -> so hang chuc nghin
    //				Setup.DigitArray[0] = xSystem.Parameter.TactTimeSetup/10000;
    //				Setup.DigitArray[1] = (xSystem.Parameter.TactTimeSetup - Setup.DigitArray[0]*10000)/1000;
    //				Setup.DigitArray[2] = (xSystem.Parameter.TactTimeSetup - Setup.DigitArray[0]*10000 - Setup.DigitArray[1]*1000)/100;
    //				Setup.DigitArray[3] = (xSystem.Parameter.TactTimeSetup - Setup.DigitArray[0]*10000 - Setup.DigitArray[1]*1000 - Setup.DigitArray[2]*100)/10;
    //				Setup.DigitArray[4] = xSystem.Parameter.TactTimeSetup - Setup.DigitArray[0]*10000 - Setup.DigitArray[1]*1000 - Setup.DigitArray[2]*100 - Setup.DigitArray[3]*10;
    //
    //				Setup.index = 0xFF;	//Danh dau lan dau bam Enter vao Setup
    //			}
    //			else if(xSystem.Status.isSetupState == 1)
    //			{
    //				//Dang trong che do Setup -> Luu cai dat va thoat Setup mode
    //				uint32_t tactTimeSetup = Setup.DigitArray[0]*10000 + Setup.DigitArray[1]*1000 + Setup.DigitArray[2]*100 + Setup.DigitArray[3]*10 + Setup.DigitArray[4];
    //
    //				//If success
    //				xSystem.Parameter.TactTimeSetup = tactTimeSetup;
    //				xSystem.Status.TactTimeCounter = tactTimeSetup;
    //				if(InternalFlash_WriteConfig(xSystem.Parameter.TactTimeSetup, xSystem.Parameter.LEDBrightness) == 0) {
    //					//Test beep
    //					BuzzerBeeps(1);
    //				}
    //
    //				//Thoat che do setup
    //				clearSetupParam();
    //				xSystem.Status.isSetupState = 0;
    //			}
    //		}
    //	}
    //
    //	//Dem thoi gian nhan giu CANCEL trong 3s -> clear du lieu vehicle counter ngay cu
    //	if(keyCancelHoldTimeout) {
    //		keyCancelHoldTimeout++;
    //		if(keyCancelHoldTimeout >= 30) {
    //			keyCancelHoldTimeout = 0;
    //
    //			//Clear du lieu ca truoc -> Bat dau ca lam viec moi
    //			xSystem.Status.DelayTimeSum = 0;
    //
    //			//Xoa vehicle counter trong bo nho
    //			xSystem.Status.VehicleCountInDay = 0;
    //			if(InternalFlash_WriteVehicleCount(xSystem.Status.VehicleCountInDay) == 0)
    //			{
    //				BuzzerBeeps(2);
    //			}
    //
    //			//Reset Tact time counter cho cong doan moi
    //			xSystem.Status.TactTimeCounter = xSystem.Parameter.TactTimeSetup;
    //			xSystem.Status.WorkingState = TACT_PAUSE;
    //		}
    //	}

    //Timeout trong che do Setup
    if (setupModeTimeout)
    {
        setupModeTimeout--;
        if (setupModeTimeout == 0)
        {
            xSystem.Status.isSetupState = 0;

            /* Giam do sang LED */
            xSystem.Parameter.LEDBrightness = LED_BRIGHTNESS_20;
        }
    }
}

void Button_Init(void)
{
    gpio_mode_set(BUTTON_UP_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BUTTON_UP_PIN);
    gpio_mode_set(BUTTON_DOWN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BUTTON_DOWN_PIN);
    gpio_mode_set(BUTTON_LEFT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BUTTON_LEFT_PIN);
    gpio_mode_set(BUTTON_RIGHT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BUTTON_RIGHT_PIN);
//    GPIO_InitTypeDef GPIO_InitStructure;

//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

//    GPIO_InitStructure.GPIO_Pin = BUTTON_UP_PIN;
//    GPIO_Init(BUTTON_UP_PORT, &GPIO_InitStructure);

//    GPIO_InitStructure.GPIO_Pin = BUTTON_DOWN_PIN;
//    GPIO_Init(BUTTON_DOWN_PORT, &GPIO_InitStructure);

//    GPIO_InitStructure.GPIO_Pin = BUTTON_LEFT_PIN;
//    GPIO_Init(BUTTON_LEFT_PORT, &GPIO_InitStructure);

//    GPIO_InitStructure.GPIO_Pin = BUTTON_RIGHT_PIN;
//    GPIO_Init(BUTTON_RIGHT_PORT, &GPIO_InitStructure);

    /* Volume ADC */
//    ADC_Config();

    /* Params */
    if (xSystem.Parameter.Mode == MODE_MIC)
        Setup.lastModeIndex = MODE_ARRAY_SIZE - 1;
    else if (xSystem.Parameter.Mode == MODE_FM_LOCAL)
    {
        Setup.modeIndex = xSystem.Parameter.FreqIndex;
        Setup.lastModeIndex = xSystem.Parameter.FreqIndex;
    }
    setupModeTimeout = 150;
}

/**
  * @brief  ADC1 channel configuration
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
#if 0
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* ADC1 DeInit */
    ADC_DeInit(ADC1);

    /* GPIOC Periph clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* DMA1 clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA1 Channel1 Config */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC0VolumeValue;
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

    /* Configure ADC Channel0 as analog input */
    GPIO_InitStructure.GPIO_Pin = ADC_VOLUME_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(ADC_VOLUME_PORT, &GPIO_InitStructure);

    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);

    /* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Convert the ADC1 Channel11 and channel10 with 55.5 Cycles as sampling time */
    ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_55_5Cycles);

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
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY) && Timeout)
    {
        Timeout--;
        if (Timeout % 10000)
            ResetWatchdog();
    }

    /* ADC1 regular Software Start Conv */
    ADC_StartOfConversion(ADC1);
#endif
}

///*******************************************************************************
// * Function Name  	: EXTI4_15_IRQHandler 
// * Return         	: None
// * Parameters 		: None
// * Created by		: Phinht
// * Date created	: 28/02/2016
// * Description		: This function handles External lines 9 to 5 interrupt request.
// * Notes			: 
// *******************************************************************************/
//void EXTI4_15_IRQHandler(void)
//{
////    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
////    {
////        EXTI_ClearITPendingBit(EXTI_Line6);
////    }
//}

/*************************************	End of file ***********************************/
