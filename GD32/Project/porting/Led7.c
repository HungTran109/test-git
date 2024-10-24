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
#include "string.h"
#include "DataDefine.h"
#include "Hardware.h"
#include "Led7.h"
#include "Indicator.h"
#include "InternalFlash.h"
#include "HardwareManager.h"
#include "Main.h"

/******************************************************************************
                                   GLOBAL VARIABLES					    			 
 ******************************************************************************/
extern System_t xSystem;
//extern Setup_t Setup;

/******************************************************************************
                                   GLOBAL FUNCTIONS					    			 
 ******************************************************************************/

/******************************************************************************
                                   DATA TYPE DEFINE					    			 
 ******************************************************************************/
#define LED7_DATA_PORT GPIOB

#define LED_MODE_ON Bit_SET
#define LED_MODE_OFF Bit_RESET

//For LED7 digit power control
#define DIGIT_ON Bit_SET
#define DIGIT_OFF Bit_RESET

#define PW_DIGIT_DONVI_ON GPIO_BC(LED4_EN_PORT) = LED4_EN_PIN
#define PW_DIGIT_DONVI_OFF GPIO_BOP(LED4_EN_PORT) = LED4_EN_PIN

#define PW_DIGIT_CHUC_ON GPIO_BC(LED3_EN_PORT) = LED3_EN_PIN
#define PW_DIGIT_CHUC_OFF GPIO_BOP(LED3_EN_PORT) = LED3_EN_PIN

#define PW_DIGIT_TRAM_ON GPIO_BC(LED2_EN_PORT) = LED2_EN_PIN
#define PW_DIGIT_TRAM_OFF GPIO_BOP(LED2_EN_PORT) = LED2_EN_PIN

#define PW_DIGIT_NGHIN_ON GPIO_BC(LED1_EN_PORT) = LED1_EN_PIN
#define PW_DIGIT_NGHIN_OFF GPIO_BOP(LED1_EN_PORT) = LED1_EN_PIN

#define LED_4G(x) GPIO_WriteBit(LED_MODE_4G_PORT, LED_MODE_4G_PIN, (BitAction)x)
#define LED_FM(x) GPIO_WriteBit(LED_MODE_FM_PORT, LED_MODE_FM_PIN, (BitAction)x)

#define LED_4G_TOGGLE() GPIOToggle(LED_MODE_4G_PORT, LED_MODE_4G_PIN)
#define LED_FM_TOGGLE() GPIOToggle(LED_MODE_FM_PORT, LED_MODE_FM_PIN)

#define Abs(a) (a < 0) ? (-a) : (a)

/* Bang ma LED7 - anode common */
const uint8_t Code7SegTab[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, //Digit '0' - '9'
                               0xFF, 0xBF, 0xC1, 0x86, 0x8E, 0x87, 0xCF,                   //OFF,  '-', 'V', 'E', 'F', 't', '1' front
                               0xC7, 0xCF, 0xC8, 0xC2, 0x8C, 0xA1, 0x88, 0xC1, 0x89};      // 'L', 'I', 'N', 'G', 'P', 'd', 'A', 'U', 'X'/'H'

typedef enum
{
    Digit0 = 0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9,
    OFF,         //10
    Dash,        //11
    SymbolV,     //12
    SymbolE,     //13
    SymbolF,     //14
    SymbolT,     //15
    Digit1Front, //16 - so 1 hien thi net phia truoc
    SymbolL,
    SymbolI,
    SymbolN,
    SymbolG, //G thieu net
    SymbolP,
    SymbolD,
    SymbolA,
    SymbolU,
    SymbolX,
} Code7Seg_Name_t;

uint8_t Led7InitDone = 0;

typedef enum
{
    DG_DONVI = 0,
    DG_CHUC,
    DG_TRAM,
    DG_NGHIN
} DigitPos_t;

typedef union
{
    struct LED7Name_t
    {
        uint8_t DonVi;
        uint8_t Chuc;
        uint8_t Tram;
        uint8_t Nghin;
    } Name;
    uint8_t Array[5];
} LED7Value_t;

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
//Mang 4 LED 7Seg
static LED7Value_t LED7Seg;
static uint8_t LED7ScanIndex = 0;
static uint16_t displayVolumeTimeout = 0;

static uint8_t isBlinkDisplay = 0;
static uint8_t lastModeIndex = 0;
/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/
static void Led7_PushData(uint8_t data);
void Led7_UpdateSetupScreen(void);

/*****************************************************************************/
/**
 * @brief	:  	Hien thi so nguyen len LED7
 * @param	:  0 - 9999
 * @retval	:
 * @author	:	
 * @created	:
 * @version	:
 * @reviewer:	
 */
void Led7_DispInteger(uint16_t num)
{
    LED7Seg.Name.DonVi = Code7SegTab[num % 10];
    num /= 10;

    LED7Seg.Name.Chuc = Code7SegTab[num % 10];
    num /= 10;

    LED7Seg.Name.Tram = Code7SegTab[num % 10];
    num /= 10;

    LED7Seg.Name.Nghin = Code7SegTab[num % 10];
}

/*****************************************************************************/
/**
 * @brief	:  	Hien thi so nguyen len LED7
 * @param	:  0 - 9999
 * @retval	:
 * @author	:	
 * @created	:
 * @version	:
 * @reviewer:	
 */
void Led7_DispFrequency(uint16_t freq)
{
    //	DEBUG_INFO("\rDispFreq: %u", freq);

    uint16_t num = freq /= 10;

    //So le thap phan
    LED7Seg.Name.DonVi = Code7SegTab[num % 10];
    num /= 10;

    //Don vi
    LED7Seg.Name.Chuc = Code7SegTab[num % 10];
    num /= 10;

    //Chuc
    LED7Seg.Name.Tram = Code7SegTab[num % 10];
    num /= 10;

    //Tram
    if (num % 10)
        LED7Seg.Name.Nghin = Code7SegTab[num % 10];
    else
        LED7Seg.Name.Nghin = Code7SegTab[OFF]; //Hang tram = 0 -> OFF

    //So hang don vi co them dau cham '.'
    LED7Seg.Name.Chuc &= 0x7F; //DP = 0
}

void Led7_DispModeMIC(void)
{
    /* Hien thi mode ' AUX' */
    LED7Seg.Name.Nghin = Code7SegTab[OFF];
    LED7Seg.Name.Tram = Code7SegTab[SymbolA];
    LED7Seg.Name.Chuc = Code7SegTab[SymbolU];
    LED7Seg.Name.DonVi = Code7SegTab[SymbolX];
}

void Led7_DispModeLineIn(void)
{
    /* Hien thi mode 'LINE' */
    LED7Seg.Name.Nghin = Code7SegTab[SymbolL];
    LED7Seg.Name.Tram = Code7SegTab[SymbolI];
    LED7Seg.Name.Chuc = Code7SegTab[SymbolN];
    LED7Seg.Name.DonVi = Code7SegTab[SymbolE];
}

void Led7_DispMode4G(void)
{
    /* Hien thi mode '-4G-' hoac INET*/
    LED7Seg.Name.Nghin = Code7SegTab[1];
    LED7Seg.Name.Tram = Code7SegTab[SymbolN];
    LED7Seg.Name.Chuc = Code7SegTab[SymbolE]; //G thieu net
    LED7Seg.Name.DonVi = Code7SegTab[SymbolT];
}

void Led7_DispModeNone(void)
{
    /* Hien thi StOP */
    LED7Seg.Name.Nghin = Code7SegTab[5];
    LED7Seg.Name.Tram = Code7SegTab[SymbolT];
    LED7Seg.Name.Chuc = Code7SegTab[0];
    LED7Seg.Name.DonVi = Code7SegTab[SymbolP];
}

void Led7_DispModeIdle(void)
{
    /* Hien thi Idle */
    LED7Seg.Name.Nghin = Code7SegTab[1];
    LED7Seg.Name.Tram = Code7SegTab[SymbolD];
    LED7Seg.Name.Chuc = Code7SegTab[SymbolL];
    LED7Seg.Name.DonVi = Code7SegTab[SymbolE];
}

/*****************************************************************************/
/**
 * @brief	:  	Hien thi Speaker volume
 * @param	:  0 - 100
 * @retval	:
 * @author	:	
 * @created	:
 * @version	:
 * @reviewer:	
 */
void Led7_DispVolume(uint8_t volume)
{
    uint8_t tr, ch, dv;

    /* Neu dang hien thi man hinh cai dat -> khong hien thi Volume */
    if (isBlinkDisplay)
        return;

    tr = volume / 100;
    ch = (volume - tr * 100) / 10;
    dv = volume - tr * 100 - ch * 10;

    LED7Seg.Name.DonVi = Code7SegTab[dv];
    LED7Seg.Name.Chuc = Code7SegTab[ch];
    LED7Seg.Name.Tram = Code7SegTab[tr];

    LED7Seg.Name.Nghin = Code7SegTab[SymbolV]; //Hien thi chu 'V' hang nghin
    if (tr == 0)
    {
        LED7Seg.Name.Tram = Code7SegTab[OFF]; // tram OFF
    }

    displayVolumeTimeout = 10; /* 5 seconds */
}

void Led7_DispOFF(void)
{
    LED7Seg.Name.DonVi = Code7SegTab[OFF];
    LED7Seg.Name.Chuc = Code7SegTab[OFF];
    LED7Seg.Name.Tram = Code7SegTab[OFF];
    LED7Seg.Name.Nghin = Code7SegTab[OFF];
}

void Led7_DispSetupMode(uint8_t modeIndex, uint8_t blinkDisp)
{
    DEBUG_INFO("Setup mode: %u\r\n", modeIndex);

    lastModeIndex = modeIndex;

    if (modeIndex <= LOCAL_FM3)
        Led7_DispFrequency(xSystem.Parameter.Frequency[modeIndex]);
    else if (modeIndex == LOCAL_MIC)
    {
        Led7_DispModeMIC();
    }
    else if (modeIndex == LOCAL_LINE)
    {
        Led7_DispModeLineIn();
    }
    else if (modeIndex == LOCAL_IDLE)
    {
        Led7_DispModeIdle();
    }
    else
        return;

    isBlinkDisplay = blinkDisp;
}

/*****************************************************************************/
/**
 * @brief	: Hien thi cac che do cai dat cua Local mode, Called 500ms
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	02/03/2016
 * @version	:
 * @reviewer:	
 */
void Led7_DispLocalMode(void)
{
    static uint8_t blinkCount = 0;

    /* Timeout thoi gian hien thi cai dat Volume */
    if (displayVolumeTimeout)
    {
        displayVolumeTimeout--;
    }
    else
    {
        /* Neu trong che do cai dat -> nhap nhay */
        if (isBlinkDisplay)
        {
            if (blinkCount)
            {
                DEBUG_INFO("Led 7 seg in Setup mode\r\n");
                Led7_DispSetupMode(lastModeIndex, 1);
            }
            else
            {
                Led7_DispOFF();
            }
            blinkCount ^= 1;
        }
        else /** Hien thi cai dat cua che do hien tai */
        {
            switch (xSystem.Parameter.ModeLocal)
            {
            case MODE_FM_LOCAL:
                Led7_DispFrequency(xSystem.Parameter.Frequency[xSystem.Parameter.FreqIndex]);
                break;
            case MODE_MIC:
                Led7_DispModeMIC();
                break;
            case MODE_LINE_IN:
                Led7_DispModeLineIn();
                break;
            case MODE_IDLE:
                Led7_DispModeIdle();
                break;

            default:
                Led7_DispOFF();
                break;
            }
        }
    }

    /** LED chi thi FM/INTERNET */
    if (xSystem.Parameter.ModeLocal == MODE_FM_LOCAL)
        LED_FM(LED_MODE_ON);
    else
        LED_FM(LED_MODE_OFF);
}

/*****************************************************************************/
/**
 * @brief	: Call 500ms
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	02/03/2016
 * @version	:
 * @reviewer:	
 */
void Led7_Tick(void)
{
    static uint8_t r = 0, g = 1, b = 0;
    static uint8_t tick1s = 0;
    if (xSystem.Parameter.enter_test_mode == 0)
    {
        /** Hien thi LED che do hoat dong
        * 08/06/20: Add Neu Remote dang chay Internet/FM/MIC -> chuyen LocalMode = IDLE
        * Muc dich: Sau khi remote ngung phat thi local cung STOP che do local, 
        * tranh truong hop ko có người trực -> máy vẫn phát
        */
        LED_4G(LED_MODE_OFF);
        LED_FM(LED_MODE_OFF);
        switch (xSystem.Parameter.Mode)
        {
        case MODE_INTERNET:
            LED_4G(LED_MODE_ON);
            Led7_DispMode4G();
            xSystem.Parameter.ModeLocal = MODE_IDLE;
            break;

        case MODE_FM_REMOTE:
            LED_FM(LED_MODE_ON);
            Led7_DispFrequency(xSystem.Parameter.FreqRun);
            xSystem.Parameter.ModeLocal = MODE_IDLE;
            break;

        case MODE_MIC:
            Led7_DispModeMIC();
            xSystem.Parameter.ModeLocal = MODE_IDLE;
            break;

        case MODE_IDLE:
            /* LED7 Hien thi mode theo lua chon duoi local */
            Led7_DispLocalMode();
            break;

        case MODE_NONE:
            xSystem.Parameter.ModeLocal = MODE_IDLE;
            Led7_DispModeNone();
            break;
        default:
            break;
        }

        /** Hien thi LED RGB audio level, neu dang IDLE -> LED OFF */
        if (xSystem.Parameter.Mode == MODE_IDLE && xSystem.Parameter.ModeLocal == MODE_IDLE)
        {
            r = g = b = LED_MODE_OFF;
        }
        else
        {
            r = g = b = LED_MODE_OFF;
            switch (xSystem.Status.AudioLevel)
            {
            case 1:
                //B+G = Xanh nhạt da trời
                b = LED_MODE_ON;
                g = LED_MODE_ON; /** Dang phat thanh, volume phu hop < 75 */
                break;
            case 2:
                //G = Xanh lá
                g = LED_MODE_ON; /** Dang phat thanh, volume trung binh < 80 */
                break;
            case 3:
                //B = Xanh biển Blue
                b = LED_MODE_ON; /** Dang phat thanh, volume hoi cao < 85 */
                break;
            case 4:
                //G+R = Vàng
                g = LED_MODE_ON;
                r = LED_MODE_ON; /** Dang phat thanh, volume cao <90 */
                break;
            case 5:
                //B+R = Tím
                b = LED_MODE_ON;
                r = LED_MODE_ON; /** Dang phat thanh, volume qua cao < 95 */
                break;
            case 6:
                //Đỏ
                r = LED_MODE_ON; /** Volume rat cao > 95 */
                break;
            default:
                break;
            }
        }
        GPIO_WriteBit(RGB_RED_PORT, RGB_RED_PIN, (BitAction)r);
        GPIO_WriteBit(RGB_GREEN_PORT, RGB_GREEN_PIN, (BitAction)g);
        GPIO_WriteBit(RGB_BLUE_PORT, RGB_BLUE_PIN, (BitAction)b);     
    }
    else
    {
        if (tick1s % 2)
        {
            static uint32_t count = 0;
            if (count == 0)
            {
                count++;
                GPIO_WriteBit(RGB_RED_PORT, RGB_RED_PIN, 1);
                GPIO_WriteBit(RGB_GREEN_PORT, RGB_GREEN_PIN, 0);
                GPIO_WriteBit(RGB_BLUE_PORT, RGB_BLUE_PIN, 0);     
                LED_4G(0);
                LED_FM(0);
            }
            else if (count == 1)
            {
                count++;
                GPIO_WriteBit(RGB_RED_PORT, RGB_RED_PIN, 0);
                GPIO_WriteBit(RGB_GREEN_PORT, RGB_GREEN_PIN, 1);
                GPIO_WriteBit(RGB_BLUE_PORT, RGB_BLUE_PIN, 0);    
                LED_4G(0);
                LED_FM(0);            
            } 
            else if (count == 2)
            {
                count++;
                GPIO_WriteBit(RGB_RED_PORT, RGB_RED_PIN, 0);
                GPIO_WriteBit(RGB_GREEN_PORT, RGB_GREEN_PIN, 0);
                GPIO_WriteBit(RGB_BLUE_PORT, RGB_BLUE_PIN, 1);  
                LED_4G(0);
                LED_FM(0);            
            }
            else if (count == 3)
            {
                count++;
                GPIO_WriteBit(RGB_RED_PORT, RGB_RED_PIN, 0);
                GPIO_WriteBit(RGB_GREEN_PORT, RGB_GREEN_PIN, 0);
                GPIO_WriteBit(RGB_BLUE_PORT, RGB_BLUE_PIN, 0); 
                LED_4G(1);
                LED_FM(0);            
            }
            else if (count == 4)
            {
                count = 0;
                GPIO_WriteBit(RGB_RED_PORT, RGB_RED_PIN, 0);
                GPIO_WriteBit(RGB_GREEN_PORT, RGB_GREEN_PIN, 0);
                GPIO_WriteBit(RGB_BLUE_PORT, RGB_BLUE_PIN, 0); 
                LED_4G(0);
                LED_FM(1);            
            }
        }
        static uint32_t integer_count = 0;
        Led7_DispInteger(integer_count);
        integer_count += 1111;
        if (integer_count > 9999)
        {
            integer_count = 0;
        }
    }

    tick1s++;
    if (tick1s % 2 == 0)
    {
        if (xSystem.Status.DelaySleepLED7 > 1)
            xSystem.Status.DelaySleepLED7--;
    }
}

/*****************************************************************************
* Ham push 1 byte ra LED7_DATA_PORT 
*/
static void Led7_PushData(uint8_t data)
{
    //Read data output first

    uint16_t curVal = GPIO_OCTL(LED7_DATA_PORT);

    //Assign new value to LSBs byte */
    curVal &= 0xFF00;
    curVal |= data;

    //Write new value
    GPIO_OCTL(LED7_DATA_PORT) = curVal;
}

/*****************************************************************************/
/**
 * @brief	:  	Tick every 5ms for push data to 595. 
 * @param	:  Quet LED 7 sau moi 5ms -> dieu chinh do sang LED bang cach ON/OFF theo duty
 * @retval	:
 * @author	:	
 * @created	:
 * @version	:
 * @reviewer:	
 */

void Led7_ScanSeg(void)
{
    uint32_t current_tick = sys_get_ms();
    static uint8_t periodNum = 0;

    //Control digit power & Push data to only one LED7Seg digit
    PW_DIGIT_DONVI_OFF;
    PW_DIGIT_CHUC_OFF;
    PW_DIGIT_TRAM_OFF;
    PW_DIGIT_NGHIN_OFF;

    //Nếu hết timeout sáng màn hình -> tắt LED7
    if (xSystem.Status.DelaySleepLED7 == 1)
        return;

    //Dimmer:
    periodNum++;
//    xSystem.Parameter.LEDBrightness = 1;
    if (periodNum < xSystem.Parameter.LEDBrightness)
        return;

    //Push LED7Seg value
    Led7_PushData(LED7Seg.Array[LED7ScanIndex]);

    if (LED7ScanIndex == DG_DONVI)
    {
        PW_DIGIT_DONVI_ON;
    }
    else if (LED7ScanIndex == DG_CHUC)
    {
        PW_DIGIT_CHUC_ON;
    }
    else if (LED7ScanIndex == DG_TRAM)
    {
        PW_DIGIT_TRAM_ON;
    }
    else if (LED7ScanIndex == DG_NGHIN)
        PW_DIGIT_NGHIN_ON;

    LED7ScanIndex++;
    if (LED7ScanIndex > DG_NGHIN)
    {
        LED7ScanIndex = 0;
    }

    if (periodNum >= 5) //Chia lam 5 khoang 5ms -> tan so quet 20ms
    {
        periodNum = 0;
    }
}

/*****************************************************************************
* Timer Interrupt period 5ms
*/

#if 0
void TIM3_IRQHandler(void)
{		
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {        							
        //Chi hien thi mat LED khi o trang thai UI ON
    //		if(xSystem.Parameters.UIState == DEVICE_UI_ON)
        {
            Led7_ScanSeg();
        }		
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);             
    }    
}
#else
void TIMER2_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_UP))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);
        Led7_ScanSeg();
    }
}
#endif

/*****************************************************************************/
/**
 * @brief	:  Khoi tao Timer for LED7 scan, period 5ms
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void InitTimerForLED(void)
{
#if 0
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* Khoi tao cho Timer 3 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 4000 - 1; // 32000 = 8ms, 20000 = 5ms, 8000 = 2ms, 4000 = 1ms
	TIM_TimeBaseStructure.TIM_Prescaler = 12 - 1; 	//48 MHz Clock down to 4 MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Khoi tao ngat Timer 3 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);    
	
	/* TIM IT Update enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	/* TIM3 enable */
	TIM_Cmd(TIM3, ENABLE);
#else
    /* ----------------------------------------------------------------------------
    TIMER2 Configuration: 
    TIMER2CLK = SystemCoreClock/3600 = 20KHz.
    TIMER2 configuration is timing mode, and the timing is 0.005s(20/20000 = 0.001s).
    CH0 update rate = TIMER2 counter clock/CH0CV = 20000/20 = 1000Hz.
    ---------------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER2);

    /* deinit a TIMER */
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler = 3599;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 20;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);

    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER2);

    nvic_irq_enable(TIMER2_IRQn, 2);
#endif
}

/*****************************************************************************/
/**
 * @brief	:  Khoi tao cho LED
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	
 * @version	:
 * @reviewer:	
 */
void Led7_Init(void)
{
#if 0    
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        
	//LED digit power control
	GPIO_InitStructure.GPIO_PIN =  LED1_EN_PIN;
	GPIO_Init(LED1_EN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_PIN =  LED2_EN_PIN;
	GPIO_Init(LED2_EN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_PIN =  LED3_EN_PIN;
	GPIO_Init(LED3_EN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_PIN =  LED4_EN_PIN;
	GPIO_Init(LED4_EN_PORT, &GPIO_InitStructure);
	
	//LED7 Data pin
	GPIO_InitStructure.GPIO_PIN =  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
	GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//LED status
	GPIO_InitStructure.GPIO_PIN =  LED_MODE_4G_PIN;
	GPIO_Init(LED_MODE_4G_PORT, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_PIN =  LED_MODE_FM_PIN;
	GPIO_Init(LED_MODE_FM_PORT, &GPIO_InitStructure);

	//LED RGB
	GPIO_InitStructure.GPIO_PIN =  RGB_RED_PIN;
	GPIO_Init(RGB_RED_PORT, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_PIN =  RGB_GREEN_PIN;
	GPIO_Init(RGB_GREEN_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_PIN =  RGB_BLUE_PIN;
	GPIO_Init(RGB_BLUE_PORT, &GPIO_InitStructure);
#else
    //LED digit power control
    gpio_mode_set(LED1_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED1_EN_PIN);
    gpio_mode_set(LED2_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED2_EN_PIN);
    gpio_mode_set(LED3_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED3_EN_PIN);
    gpio_mode_set(LED4_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED4_EN_PIN);
    //LED7 Data pin
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    gpio_mode_set(LED_MODE_4G_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED_MODE_4G_PIN);
    gpio_mode_set(LED_MODE_FM_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED_MODE_FM_PIN);
    gpio_mode_set(RGB_RED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, RGB_RED_PIN);
    gpio_mode_set(RGB_BLUE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, RGB_BLUE_PIN);
	gpio_mode_set(RGB_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, RGB_GREEN_PIN);

#endif
    //Digit power OFF
    PW_DIGIT_DONVI_OFF;
    PW_DIGIT_CHUC_OFF;
    PW_DIGIT_TRAM_OFF;
    PW_DIGIT_NGHIN_OFF;

    //Timer for LED
    InitTimerForLED();

    Led7_DispInteger(8888);
    Delayms(500);
}

/*************************************	End of file ***********************************/
