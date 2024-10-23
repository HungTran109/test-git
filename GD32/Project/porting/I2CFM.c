/******************************************************************************
 * @file    	I2CFM.c
 * @author  	
 * @version 	V1.0.0
 * @date    	15/01/2014
 * @brief   	
 ******************************************************************************/

/******************************************************************************
                                   INCLUDES					    			 
******************************************************************************/
#include <stdio.h>
#include "I2CFM.h"
#include "Main.h"
#include "DataDefine.h"
#include "Led7.h"
#include "KT_AMFMdrv.h"
#include "HardwareManager.h"

#define I2C0_OWN_ADDRESS7      0x72

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
#define __USE_EXTERNAL_CLOCK__ 1

#if __USE_EXTERNAL_CLOCK__
#warning !! __CHU Y DANG CAU HINH __USE_EXTERNAL_CLOCK__ !!
#endif

typedef union
{
    struct BitNameCh0_t
    {
        uint8_t FM_CHAN_11_8 : 4; /* FM_CHAN[11:8] */
        uint8_t NA1 : 2;
        uint8_t AM_FM : 1; /* AM/FM mode: 0 = FM mode, 1 = AM */
        uint8_t TUNE : 1;  /* 0 = Normal operation, 1 = Tune process enable */
    } Name;
    uint8_t Value;
} FMCHAN0_Value_t;

#ifdef FM_DELTN
const uint8_t sys_clk[641] =
    {
        0x00, //	76
        0x01, //	76.05
        0x01, //	76.1
        0x02, //	76.15
        0x02, //	76.2
        0x03, //	76.25
        0x02, //	76.3
        0x03, //	76.35
        0x04, //	76.4
        0x04, //	76.45
        0x05, //	76.5
        0x0C, //	76.55
        0x06, //	76.6
        0x06, //	76.65
        0x06, //	76.7
        0x02, //	76.75
        0x07, //	76.8
        0x08, //	76.85
        0x01, //	76.9
        0x01, //	76.95
        0x09, //	77
        0x09, //	77.05
        0x0A, //	77.1
        0x03, //	77.15
        0x0B, //	77.2
        0x0B, //	77.25
        0x00, //	77.3
        0x04, //	77.35
        0x0D, //	77.4
        0x0D, //	77.45
        0x02, //	77.5
        0x02, //	77.55
        0x0E, //	77.6
        0x07, //	77.65
        0x08, //	77.7
        0x00, //	77.75
        0x04, //	77.8
        0x05, //	77.85
        0x05, //	77.9
        0x0B, //	77.95
        0x01, //	78
        0x0B, //	78.05
        0x0B, //	78.1
        0x0E, //	78.15
        0x00, //	78.2
        0x03, //	78.25
        0x0D, //	78.3
        0x09, //	78.35
        0x05, //	78.4
        0x0A, //	78.45
        0x0A, //	78.5
        0x0F, //	78.55
        0x07, //	78.6
        0x00, //	78.65
        0x07, //	78.7
        0x0D, //	78.75
        0x09, //	78.8
        0x09, //	78.85
        0x00, //	78.9
        0x0E, //	78.95
        0x03, //	79
        0x0F, //	79.05
        0x0F, //	79.1
        0x00, //	79.15
        0x0C, //	79.2
        0x03, //	79.25
        0x0C, //	79.3
        0x02, //	79.35
        0x0E, //	79.4
        0x0E, //	79.45
        0x03, //	79.5
        0x0F, //	79.55
        0x02, //	79.6
        0x0D, //	79.65
        0x05, //	79.7
        0x0E, //	79.75
        0x02, //	79.8
        0x06, //	79.85
        0x0B, //	79.9
        0x0B, //	79.95
        0x00, //	80
        0x00, //	80.05
        0x0C, //	80.1
        0x01, //	80.15
        0x09, //	80.2
        0x02, //	80.25
        0x02, //	80.3
        0x00, //	80.35
        0x03, //	80.4
        0x0B, //	80.45
        0x04, //	80.5
        0x04, //	80.55
        0x03, //	80.6
        0x05, //	80.65
        0x0D, //	80.7
        0x06, //	80.75
        0x07, //	80.8
        0x07, //	80.85
        0x07, //	80.9
        0x05, //	80.95
        0x08, //	81
        0x09, //	81.05
        0x09, //	81.1
        0x09, //	81.15
        0x0A, //	81.2
        0x0A, //	81.25
        0x00, //	81.3
        0x0B, //	81.35
        0x01, //	81.4
        0x0C, //	81.45
        0x01, //	81.5
        0x02, //	81.55
        0x0D, //	81.6
        0x03, //	81.65
        0x0E, //	81.7
        0x0E, //	81.75
        0x04, //	81.8
        0x0F, //	81.85
        0x05, //	81.9
        0x0D, //	81.95
        0x05, //	82
        0x03, //	82.05
        0x06, //	82.1
        0x07, //	82.15
        0x07, //	82.2
        0x07, //	82.25
        0x03, //	82.3
        0x08, //	82.35
        0x0F, //	82.4
        0x04, //	82.45
        0x09, //	82.5
        0x05, //	82.55
        0x0A, //	82.6
        0x0B, //	82.65
        0x0B, //	82.7
        0x0B, //	82.75
        0x07, //	82.8
        0x0C, //	82.85
        0x0D, //	82.9
        0x0D, //	82.95
        0x0D, //	83
        0x0E, //	83.05
        0x0E, //	83.1
        0x0F, //	83.15
        0x0F, //	83.2
        0x0F, //	83.25
        0x01, //	83.3
        0x0E, //	83.35
        0x00, //	83.4
        0x02, //	83.45
        0x06, //	83.5
        0x03, //	83.55
        0x00, //	83.6
        0x02, //	83.65
        0x04, //	83.7
        0x08, //	83.75
        0x05, //	83.8
        0x06, //	83.85
        0x06, //	83.9
        0x06, //	83.95
        0x07, //	84
        0x07, //	84.05
        0x07, //	84.1
        0x08, //	84.15
        0x08, //	84.2
        0x0C, //	84.25
        0x09, //	84.3
        0x0D, //	84.35
        0x0A, //	84.4
        0x0A, //	84.45
        0x0E, //	84.5
        0x09, //	84.55
        0x0B, //	84.6
        0x01, //	84.65
        0x0C, //	84.7
        0x01, //	84.75
        0x0B, //	84.8
        0x0D, //	84.85
        0x02, //	84.9
        0x0E, //	84.95
        0x03, //	85
        0x0D, //	85.05
        0x0F, //	85.1
        0x04, //	85.15
        0x0E, //	85.2
        0x05, //	85.25
        0x05, //	85.3
        0x06, //	85.35
        0x06, //	85.4
        0x07, //	85.45
        0x0F, //	85.5
        0x08, //	85.55
        0x08, //	85.6
        0x08, //	85.65
        0x09, //	85.7
        0x09, //	85.75
        0x0A, //	85.8
        0x0A, //	85.85
        0x0A, //	85.9
        0x0B, //	85.95
        0x02, //	86
        0x0B, //	86.05
        0x0C, //	86.1
        0x0C, //	86.15
        0x0D, //	86.2
        0x0D, //	86.25
        0x0E, //	86.3
        0x0E, //	86.35
        0x0E, //	86.4
        0x0F, //	86.45
        0x0C, //	86.5
        0x0F, //	86.55
        0x0D, //	86.6
        0x0D, //	86.65
        0x0E, //	86.7
        0x0E, //	86.75
        0x0F, //	86.8
        0x0F, //	86.85
        0x0F, //	86.9
        0x0C, //	86.95
        0x0C, //	87		//khk 20130930
              //	0x0D,	//	87
        0x0D, //	87.05
        0x00, //	87.1
        0x0E, //	87.15
        0x00, //	87.2
        0x00, //	87.25
        0x00, //	87.3
        0x01, //	87.35
        0x01, //	87.4
        0x01, //	87.45
        0x02, //	87.5	//khk 20130929
              //	0x06,	//	87.5
        0x02, //	87.55
        0x00, //	87.6
        0x00, //	87.65
        0x00, //	87.7
        0x04, //	87.75
        0x01, //	87.8
        0x04, //	87.85
        0x05, //	87.9
        0x02, //	87.95
        0x03, //	88		//khk 20130930
              //	0x05,	//	88
        0x00, //	88.05
        0x00, //	88.1
        0x00, //	88.15
        0x01, //	88.2
        0x01, //	88.25
        0x01, //	88.3
        0x02, //	88.35
        0x02, //	88.4
        0x03, //	88.45
        0x01, //	88.5
        0x00, //	88.55
        0x00, //	88.6
        0x04, //	88.65
        0x01, //	88.7
        0x05, //	88.75
        0x05, //	88.8
        0x06, //	88.85
        0x04, //	88.9
        0x06, //	88.95
        0x03, //	89
        0x07, //	89.05
        0x07, //	89.1
        0x05, //	89.15
        0x08, //	89.2
        0x05, //	89.25
        0x09, //	89.3
        0x09, //	89.35
        0x06, //	89.4
        0x0A, //	89.45
        0x07, //	89.5
        0x08, //	89.55
        0x00, //	89.6
        0x08, //	89.65
        0x0A, //	89.7
        0x0C, //	89.75
        0x03, //	89.8
        0x0D, //	89.85
        0x0D, //	89.9
        0x04, //	89.95
        0x01, //	90
        0x00, //	90.05
        0x02, //	90.1
        0x03, //	90.15
        0x0C, //	90.2
        0x03, //	90.25
        0x00, //	90.3
        0x00, //	90.35
        0x04, //	90.4
        0x05, //	90.45
        0x01, //	90.5
        0x05, //	90.55
        0x02, //	90.6
        0x06, //	90.65
        0x08, //	90.7
        0x00, //	90.75
        0x07, //	90.8
        0x00, //	90.85
        0x01, //	90.9
        0x01, //	90.95
        0x05, //	91
        0x05, //	91.05
        0x02, //	91.1
        0x06, //	91.15
        0x0A, //	91.2
        0x0A, //	91.25
        0x07, //	91.3
        0x04, //	91.35
        0x01, //	91.4
        0x05, //	91.45
        0x0C, //	91.5
        0x09, //	91.55
        0x09, //	91.6
        0x06, //	91.65
        0x03, //	91.7
        0x0E, //	91.75
        0x07, //	91.8
        0x0B, //	91.85
        0x08, //	91.9
        0x08, //	91.95
        0x09, //	92
        0x09, //	92.05
        0x00, //	92.1
        0x00, //	92.15
        0x00, //	92.2
        0x01, //	92.25
        0x01, //	92.3
        0x01, //	92.35
        0x0C, //	92.4
        0x02, //	92.45
        0x02, //	92.5
        0x00, //	92.55
        0x0D, //	92.6
        0x00, //	92.65
        0x01, //	92.7
        0x04, //	92.75
        0x01, //	92.8
        0x05, //	92.85
        0x02, //	92.9
        0x06, //	92.95
        0x06, //	93
        0x03, //	93.05
        0x03, //	93.1
        0x07, //	93.15
        0x04, //	93.2
        0x08, //	93.25
        0x08, //	93.3
        0x05, //	93.35
        0x06, //	93.4
        0x03, //	93.45
        0x06, //	93.5
        0x0A, //	93.55
        0x07, //	93.6
        0x08, //	93.65
        0x0B, //	93.7
        0x08, //	93.75
        0x0C, //	93.8
        0x0C, //	93.85
        0x09, //	93.9
        0x0D, //	93.95
        0x0D, //	94
        0x0A, //	94.05
        0x00, //	94.1
        0x0E, //	94.15
        0x0F, //	94.2
        0x0C, //	94.25
        0x0F, //	94.3
        0x0C, //	94.35
        0x0A, //	94.4
        0x0D, //	94.45
        0x0D, //	94.5
        0x0B, //	94.55
        0x0E, //	94.6
        0x0F, //	94.65
        0x0C, //	94.7
        0x0F, //	94.75
        0x09, //	94.8
        0x0D, //	94.85
        0x04, //	94.9
        0x06, //	94.95
        0x07, //	95
        0x05, //	95.05
        0x07, //	95.1
        0x08, //	95.15
        0x06, //	95.2
        0x08, //	95.25
        0x09, //	95.3
        0x07, //	95.35
        0x0A, //	95.4
        0x0A, //	95.45
        0x08, //	95.5
        0x0B, //	95.55
        0x01, //	95.6
        0x0B, //	95.65
        0x09, //	95.7
        0x0A, //	95.75
        0x0C, //	95.8
        0x03, //	95.85
        0x0B, //	95.9
        0x0B, //	95.95
        0x02, //	96
        0x0C, //	96.05
        0x0E, //	96.1
        0x0F, //	96.15
        0x0D, //	96.2
        0x04, //	96.25
        0x05, //	96.3
        0x0E, //	96.35
        0x01, //	96.4
        0x06, //	96.45
        0x0F, //	96.5
        0x00, //	96.55
        0x07, //	96.6
        0x03, //	96.65
        0x03, //	96.7
        0x04, //	96.75
        0x02, //	96.8
        0x0C, //	96.85
        0x04, //	96.9
        0x01, //	96.95
        0x09, //	97
        0x05, //	97.05
        0x05, //	97.1
        0x06, //	97.15
        0x05, //	97.2
        0x06, //	97.25
        0x0B, //	97.3
        0x02, //	97.35
        0x02, //	97.4
        0x05, //	97.45
        0x07, //	97.5
        0x00, //	97.55
        0x00, //	97.6
        0x08, //	97.65
        0x00, //	97.7
        0x0A, //	97.75
        0x0A, //	97.8
        0x05, //	97.85
        0x02, //	97.9
        0x0A, //	97.95
        0x06, //	98
        0x03, //	98.05
        0x0C, //	98.1
        0x07, //	98.15
        0x0D, //	98.2
        0x0C, //	98.25
        0x08, //	98.3
        0x0E, //	98.35
        0x0D, //	98.4
        0x09, //	98.45
        0x0F, //	98.5
        0x05, //	98.55
        0x06, //	98.6
        0x07, //	98.65
        0x0C, //	98.7
        0x07, //	98.75
        0x08, //	98.8
        0x07, //	98.85
        0x08, //	98.9
        0x09, //	98.95
        0x0D, //	99
        0x01, //	99.05
        0x0A, //	99.1
        0x09, //	99.15
        0x0A, //	99.2
        0x0B, //	99.25
        0x0B, //	99.3
        0x0A, //	99.35
        0x00, //	99.4
        0x0B, //	99.45
        0x04, //	99.5
        0x0A, //	99.55
        0x0C, //	99.6
        0x0E, //	99.65
        0x02, //	99.7
        0x0D, //	99.75
        0x06, //	99.8
        0x0F, //	99.85
        0x0E, //	99.9
        0x07, //	99.95
        0x04, //	100
        0x0F, //	100.05
        0x08, //	100.1
        0x0E, //	100.15
        0x0F, //	100.2
        0x09, //	100.25
        0x0F, //	100.3
        0x0A, //	100.35
        0x00, //	100.4
        0x00, //	100.45
        0x00, //	100.5
        0x01, //	100.55
        0x08, //	100.6
        0x0C, //	100.65
        0x0C, //	100.7
        0x09, //	100.75
        0x09, //	100.8
        0x03, //	100.85
        0x01, //	100.9
        0x03, //	100.95
        0x03, //	101
        0x02, //	101.05
        0x0F, //	101.1
        0x04, //	101.15
        0x05, //	101.2
        0x06, //	101.25
        0x04, //	101.3
        0x06, //	101.35
        0x0D, //	101.4
        0x07, //	101.45
        0x07, //	101.5
        0x05, //	101.55
        0x06, //	101.6
        0x00, //	101.65
        0x00, //	101.7
        0x08, //	101.75
        0x01, //	101.8
        0x00, //	101.85
        0x09, //	101.9
        0x02, //	101.95
        0x02, //	102
        0x0A, //	102.05
        0x09, //	102.1
        0x03, //	102.15
        0x00, //	102.2
        0x04, //	102.25
        0x03, //	102.3
        0x0C, //	102.35
        0x05, //	102.4
        0x05, //	102.45
        0x02, //	102.5
        0x0E, //	102.55
        0x0C, //	102.6
        0x03, //	102.65
        0x0F, //	102.7
        0x04, //	102.75
        0x04, //	102.8
        0x04, //	102.85
        0x05, //	102.9
        0x05, //	102.95
        0x05, //	103
        0x09, //	103.05
        0x03, //	103.1
        0x04, //	103.15
        0x0A, //	103.2
        0x0A, //	103.25
        0x07, //	103.3
        0x0B, //	103.35
        0x03, //	103.4
        0x03, //	103.45
        0x0C, //	103.5
        0x09, //	103.55
        0x09, //	103.6
        0x0D, //	103.65
        0x0A, //	103.7
        0x05, //	103.75
        0x0D, //	103.8
        0x02, //	103.85
        0x02, //	103.9
        0x06, //	103.95
        0x0C, //	104
        0x0C, //	104.05
        0x07, //	104.1
        0x0D, //	104.15
        0x08, //	104.2
        0x08, //	104.25
        0x00, //	104.3
        0x04, //	104.35
        0x09, //	104.4
        0x01, //	104.45
        0x0F, //	104.5
        0x0A, //	104.55
        0x00, //	104.6
        0x0B, //	104.65
        0x0E, //	104.7
        0x00, //	104.75
        0x00, //	104.8
        0x0C, //	104.85
        0x0C, //	104.9
        0x01, //	104.95
        0x01, //	105
        0x00, //	105.05
        0x05, //	105.1
        0x0E, //	105.15
        0x01, //	105.2
        0x06, //	105.25
        0x03, //	105.3
        0x02, //	105.35
        0x04, //	105.4
        0x03, //	105.45
        0x07, //	105.5
        0x05, //	105.55
        0x05, //	105.6
        0x08, //	105.65
        0x06, //	105.7
        0x06, //	105.75
        0x05, //	105.8
        0x05, //	105.85
        0x07, //	105.9
        0x07, //	105.95
        0x08, //	106
        0x08, //	106.05
        0x08, //	106.1
        0x07, //	106.15
        0x0C, //	106.2
        0x0C, //	106.25
        0x08, //	106.3
        0x0A, //	106.35
        0x00, //	106.4
        0x09, //	106.45
        0x0B, //	106.5
        0x0B, //	106.55
        0x0A, //	106.6
        0x0A, //	106.65
        0x0C, //	106.7
        0x0F, //	106.75
        0x04, //	106.8
        0x0F, //	106.85
        0x05, //	106.9
        0x05, //	106.95
        0x0E, //	107
        0x0E, //	107.05
        0x06, //	107.1
        0x0F, //	107.15
        0x0F, //	107.2
        0x0E, //	107.25
        0x0E, //	107.3
        0x06, //	107.35
        0x0F, //	107.4
        0x08, //	107.45
        0x0B, //	107.5
        0x07, //	107.55
        0x09, //	107.6
        0x01, //	107.65
        0x0A, //	107.7
        0x0A, //	107.75
        0x03, //	107.8
        0x09, //	107.85
        0x0B, //	107.9
        0x0B, //	107.95
        0x0A, //	108
};
#endif
/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
__IO uint32_t KT0935Timeout = KT0935_LONG_TIMEOUT;

KT_Write_List_t KT_Write_List;
KT_Read_List_t KT_Read_List;

/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
******************************************************************************/
/**
  * @brief  DeInitializes peripherals used by the LM75 driver.
  * @param  None
  * @retval None
  */
void KT0935_LowLevel_DeInit(void)
{
#if 0
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Disable KT0935_I2C */
	I2C_Cmd(KT0935_I2C, DISABLE);

	/* DeInitializes the KT0935_I2C */
	I2C_DeInit(KT0935_I2C);

	/* KT0935_I2C Periph clock disable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);

	/* Configure KT0935_I2C pins: SCL */
	GPIO_InitStructure.GPIO_Pin = FM_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(FM_SCL_PORT, &GPIO_InitStructure);

	/* Configure KT0935_I2C pins: SDA */
	GPIO_InitStructure.GPIO_Pin = FM_SDA_PIN;
	GPIO_Init(FM_SDA_PORT, &GPIO_InitStructure);
#else
    /* enable KT0935_I2C clock */
    rcu_periph_clock_disable(RCU_I2C0);
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8 | GPIO_PIN_9);

#endif
}

/**
  * @brief  Initializes the I2C source clock and IOs used to drive the LM75
  * @param  None
  * @retval None
  */
void KT0935_LowLevel_Init(void)
{
#if 0
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* KT0935_I2C Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* Configure the I2C clock source. The clock is derived from the HSI */
	RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);	//RCC_I2C1CLK_HSI);	RCC_I2C1CLK_SYSCLK

	/* KT0935_I2C_SCL_GPIO_CLK, KT0935_I2C_SDA_GPIO_CLK 
	and KT0935_I2C_SMBUSALERT_GPIO_CLK Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,  ENABLE);

	/* Connect PXx to I2C_SCL */
	GPIO_PinAFConfig(GPIOB, FM_SCL_PIN_SOURCE, FM_I2C_AF);

	/* Connect PXx to I2C_SDA */
	GPIO_PinAFConfig(GPIOB, FM_SDA_PIN_SOURCE, FM_I2C_AF); 

	/* Configure KT0935_I2C pins: SCL */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = FM_SCL_PIN;
	GPIO_Init(FM_SCL_PORT, &GPIO_InitStructure);

	/* Configure KT0935_I2C pins: SDA */
	GPIO_InitStructure.GPIO_Pin = FM_SDA_PIN;
	GPIO_Init(FM_SDA_PORT, &GPIO_InitStructure);
#else
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable KT0935_I2C clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* connect PB8 to I2C0_SCL */
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_8);
    /* connect PB7 to I2C0_SDA */
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_9);
    /* configure GPIO pins of KT0935_I2C */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* I2C clock configure */
    i2c_clock_config(KT0935_I2C, 100000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(KT0935_I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
    /* enable KT0935_I2C */
    i2c_enable(KT0935_I2C);
    /* enable acknowledge */
    i2c_ack_config(KT0935_I2C, I2C_ACK_ENABLE);

    volatile uint32_t timeout = 0xFFFFF;
    /* wait until I2C bus is idle */
    while (i2c_flag_get(KT0935_I2C, I2C_FLAG_I2CBSY) && timeout--)
        ;
#endif
}

/**
  * @brief  DeInitializes the KT0935_I2C.
  * @param  None
  * @retval None
  */
void FM_DeInit(void)
{
    KT0935_LowLevel_DeInit();
}

/************************************************************************************/
/*? ? ?:KT_AMFM_PreInit												 			*/
/*????:????I2C????														*/
/*????:																		*/
/*????:KT_Byte_Read()?KT_Byte_Write()											*/
/*????:?																		*/
/*?    ?:?																		*/
/*?    ?:??:1					??:0											*/
/*? ? ?:Kanghekai				??:											*/
/*? ? ?:Zhoudongfeng			??:2013-06-18								*/
/*?    ?:V1.1		Add	 and "Read Manufactory ID"								*/
/************************************************************************************/
uint8_t KT_AMFM_PreInit(void)
{
    uint16_t regx;
    uint8_t regy;
    uint8_t i;
    //	regy = KT_Byte_Read(0x1a);
    //	KT_Byte_Write(0x1a, regy|0x80);	  //softmute disable
    //	regy = KT_Byte_Read(0x2e);
    //	KT_Byte_Write(0x2e, (regy&0xe0)|0x10);	  //fmagcd disable fmagc=4
    //	regy = KT_Byte_Read(0x26);
    //	KT_Byte_Write(0x26, regy|0x80);	  //fmrfagcd disable
    //	regy = KT_Byte_Read(0x27);
    //	KT_Byte_Write(0x27, 0x10);	  //fmrfagc=0x10
    //	regy = KT_Byte_Read(0x25);
    //	KT_Byte_Write(0x25, (regy&0xf8)|0x01);	  //
    regx = KT_Byte_Read(0x1e);
    KT_Byte_Write(0x1e, (regx | 0x70)); //BW_SMGAIN_LPF=1Hz
    regx = KT_Byte_Read(0x24);
    KT_Byte_Write(0x24, (regx & 0x7f));

    regy = KT_Byte_Read(0x31);
    KT_Byte_Write(0x31, (regy & 0xe0) | 0x06);
    regy = KT_Byte_Read(0x32);
    KT_Byte_Write(0x32, regy | 0x3f);
    
    for (i = 0; i < INIT_FAIL_TH; i++)
    {
        Delayms(10);
        regx = KT_Byte_Read(0x02); //Read Manufactory ID
        regy = KT_Byte_Read(0x03);
        uint16_t regxy = ((regx << 8) | regy);
        if (regxy != 0x4B54)
            continue;
        else
        {
            DEBUG_INFO("Reg02: %02X, Reg03: %02X\r\n", regx, regy);
            break;
        }
    }
    if (i == INIT_FAIL_TH)
        return (0);

    return 1;
}

uint8_t KT_AMFM_Standby(void) //0->Fail 1->Success
{
    uint8_t regx;

    regx = KT_Byte_Read(0x10);
    KT_Byte_Write(0x10, regx | 0x40); //Write STBYLDO_CALI_EN bit to 1
    Delayms(5);
    regx = KT_Byte_Read(0x76);
    KT_Byte_Write(0x76, regx & 0xFD); //Write STBYLDO_PD bit to 0
    Delayms(1);
    regx = KT_Byte_Read(0x0E);
    KT_Byte_Write(0x0E, regx | 0x20); //Write STBY bit to 1

    return (1);
}

uint8_t KT_AMFM_WakeUp(void) //0->Fail 1->Success
{
    uint8_t regx, check;

    regx = KT_Byte_Read(0x0E);
    KT_Byte_Write(0x0E, regx & 0xDF); //Write STBY bit to 0
    Delayms(1);
    regx = KT_Byte_Read(0x76);
    KT_Byte_Write(0x76, regx | 0x02); //Write STBYLDO_PD bit to 1
    check = KT_AMFM_Init();

//    KT_Byte_Write(0xFC, 0);
//    KT_Byte_Write(0xFD, 0);
//    KT_Byte_Write(0xFE, 1);
//    KT_Byte_Write(0xFF, 0x1C);
//    KT_Byte_Write(0xFB, 0x03);
    
#ifdef	RDS_ENABLE
	regx=KT_Byte_Read(0x41);
	KT_Byte_Write(0x41, (regx & 0xc0) | (RDS_ON << 7)| 0x10);			//RDS_enable gd_blk_max=0x10

	regx=KT_Byte_Read(0x43);
	KT_Byte_Write(0x43, (regx & 0x3F) | (RDS_INT_SEL << 6));	//RDS_INT_SEL	;

	regx = KT_Byte_Read(0x2D);
	KT_Byte_Write(0x2D, (regx & 0x9F));   //rds_pad_cnfg : output rds_int

	KT_Byte_Write(0x42, 0x64);   //bd_blk_max=0x32

	regx = KT_Byte_Read(0x3B);
	KT_Byte_Write(0x3B, regx | 0x80);

#endif
    if (check == 0)
        return 0;
    return (1);
}

void KT_Byte_Write(uint8_t Register_Address, uint8_t Byte_Data)
{
    FM_WriteReg(Register_Address, Byte_Data);
}

uint8_t KT_Byte_Read(uint8_t Register_Address)
{
    return (FM_ReadReg(Register_Address) & 0xFF);
}

uint8_t KT_AMFM_Init(void) //0->Fail 1->Success
{
    uint8_t regx;
    uint32_t countTimeout = 0x4ff;

    regx = KT_Byte_Read(0x4F);
    KT_Byte_Write(0x4F, (regx & 0xFC) | 2);

    //============= FM_GAIN=============
    regx = KT_Byte_Read(0x2A);
    KT_Byte_Write(0x2A, (regx & 0x0F) | (FM_GAIN << 4) | 0x80);

    //============= Configure Ramp up time =============
    regx = KT_Byte_Read(0x4E);
    KT_Byte_Write(0x4E, (regx & 0xC8) | (DEPOP_TC << 4) | AUDV_DCLVL); //DEPOP time:1s

    //	regx = KT_Byte_Read(0x24);
    //	KT_Byte_Write(0x24, (regx | 0x0c) );

    //============= System clock initialization =============
    KT_Byte_Write(0x05, DIVIDERP_L);
    regx = KT_Byte_Read(0x06);
    KT_Byte_Write(0x06, (regx & 0xF8) | DIVIDERN_H);
    KT_Byte_Write(0x07, DIVIDERN_L);
    regx = KT_Byte_Read(0x08);
    KT_Byte_Write(0x08, (regx & 0xF0) | FPFD_H);
    KT_Byte_Write(0x09, FPFD_M);
    KT_Byte_Write(0x0A, FPFD_L);
    regx = KT_Byte_Read(0x0D);
    KT_Byte_Write(0x0D, (regx & 0xEF) | RCLK_EN);
    regx = KT_Byte_Read(0x04);
    KT_Byte_Write(0x04, (regx & 0xF8) | DIVIDERP_H | (SYS_CFGOK << 7));

    countTimeout = 0x4ff;
    while (1)
    {
        countTimeout--;
        ResetWatchdog();

        regx = KT_Byte_Read(0x1B); // power on finish or not
        if ((regx & 0x04) == 0x04)
        {
            DEBUG_INFO(" [OK]");
            break;
        }
        Delayms(100);
    }
    regx = KT_Byte_Read(0x74);
    KT_Byte_Write(0x74, (regx & 0x1F) | (7 << 5));

    //============= FM/AM AFC=============
    regx = KT_Byte_Read(0x3E);
    KT_Byte_Write(0x3E, (regx & 0xB8) | (FM_AFCD << 6) | FM_TH_AFC);

    regx = KT_Byte_Read(0x3F);
    KT_Byte_Write(0x3F, (regx & 0xB8) | (AM_AFCD << 6) | AM_TH_AFC);

    regx = KT_Byte_Read(0x40);
    KT_Byte_Write(0x40, (regx & 0x87) | (3 << 4) | (1 << 3));

    regx = KT_Byte_Read(0x1B);
    KT_Byte_Write(0x1B, regx | 0x80);

    regx = KT_Byte_Read(0x3B);
    KT_Byte_Write(0x3B, (regx & 0x8F) | (3 << 4));

    //============= AM Baseband Gain =============
    regx = KT_Byte_Read(0x65);
    KT_Byte_Write(0x65, (regx & 0x80) | (5 << 4));
    KT_Byte_Write(0x66, 0x18);
    KT_Byte_Write(0x67, 0x1B);
    //	KT_Byte_Write(0x68, 0x1B);
    KT_Byte_Write(0x68, 0x16);
    regx = KT_Byte_Read(0x69);
    KT_Byte_Write(0x69, (regx & 0xF0) | AM_VOLUME);

    //============= AM audio Gain =============
    KT_Byte_Write(0x62, (AM_GAIN << 4) | FLT_SEL); //am_audio_gain,am channel filter

    //=================== ValidStation ===================
    KT_Byte_Write(0x7F, (5 << 4) | 5);
    KT_Byte_Write(0x80, (5 << 4) | 3);

    KT_Byte_Write(0x81, 0x3A);
    KT_Byte_Write(0x82, 0x35);

    KT_Byte_Write(0x83, 0x24);
    KT_Byte_Write(0x84, 0x1F);

    //  =================== Stereo Blend ===================
    KT_Byte_Write(0x34, BLEND_START_SNR);                         //blend_start_snr
    KT_Byte_Write(0x35, BLEND_STOP_SNR);                          //blend_stop_snr
    KT_Byte_Write(0x29, (BLND_MOD << 4));                         //?3bit????0,???
    KT_Byte_Write(0x2C, (BLND_START_RSSI << 4) | BLND_STOP_RSSI); //blend rssi
    KT_Byte_Write(0x2B, (MONO << 7) | (6 << 4) | (DE << 3) | DBLND);

    //============= SOFTMUTE =============
    KT_Byte_Write(0x19, (AM_SMUTE_MIN_GAIN << 5) | (FM_SMUTE_MIN_GAIN << 2));
    KT_Byte_Write(0x1D, 0x80 | AM_SMUTE_START_RSSI);
    KT_Byte_Write(0x1E, (2 << 4) | AM_SMUTE_SLOPE_RSSI);

    KT_Byte_Write(0x1F, (FM_SMUTE_START_RSSI << 4) | FM_SMUTE_SLOPE_RSSI);
    KT_Byte_Write(0x20, AM_SMUTE_START_SNR);
    KT_Byte_Write(0x21, (AM_SMUTE_SLOPE_SNR << 4) | FM_SMUTE_SLOPE_SNR);
    KT_Byte_Write(0x22, FM_SMUTE_START_SNR);

    regx = KT_Byte_Read(0x55);
    KT_Byte_Write(0x55, (regx & 0xF8) | 6); //AM_Q
    if (countTimeout == 0)
        return 0;
    return (1);
}

uint8_t KT_AMFM_SetMode(uint8_t cAMFM_Mode)
{
    uint8_t regx;

    regx = KT_Byte_Read(0x88);
    if (cAMFM_Mode == FM_MODE)
    {
        KT_Byte_Write(0x88, regx & 0xBF); //AM_FM=0
#ifdef FM_DELTN
        regx = KT_Byte_Read(0x87);
        KT_Byte_Write(0x87, (regx & 0xF7) | (1 << 3));
#endif
    }
    else
    {
        KT_Byte_Write(0x88, regx | 0x40); //AM_FM=1
#ifdef FM_DELTN
        regx = KT_Byte_Read(0x87);
        KT_Byte_Write(0x87, (regx & 0xF7) | (0 << 3));
#endif
    }

    return (1);
}

/************************************************************************************/
/*? ? ?:KT093x_User_FMTune														*/
/*????:??FM Tune???														*/
/*????: 																		*/
/*????:KT_FM_Tune()?KT_AMFM_VolumeSet()?FM_//Display()							*/
/*????:?																		*/
/*?    ?:uint Frequency															*/
/*?    ?:																		*/
/*? ? ?:Zhoudongfeng			??:2013-05-30								*/
/*? ? ?:						??:											*/
/*?    ?:V1.0																	*/
/************************************************************************************/
void KT093x_User_FMTune(uint16_t Frequency)
{
    KT_FM_Tune(Frequency);

    //	KT_AMFM_VolumeSet(xSystem.Parameter.LocalVolume);
}

uint8_t KT_AMFM_Mute(void)
{
    uint8_t regx;

    regx = KT_Byte_Read(0x0f);
    KT_Byte_Write(0x0f, regx & 0xe0);

    return (1);
}

uint8_t KT_AMFM_VolumeSet(uint8_t cVol) //Input: 0~31
{
    uint8_t regx;

    regx = KT_Byte_Read(0x0F);
    KT_Byte_Write(0x0F, (regx & 0xE0) | cVol);

    return (1);
}

/************************************************************************************/
/*? ? ?:KT_FM_Deltn																*/
/*????:FM ?Deltn??															*/
/*????:																		*/
/*????:KT_Byte_Write()															*/
/*			KT_AM_ReadRSSI() 														*/
/*????: ?																		*/
/*?    ?:?																		*/
/*? ? ?:Zhoudongfeng			??:2013-06-18								*/
/*? ? ?:						??:											*/
/*?    ?:V1.0																	*/
/************************************************************************************/
void KT_FM_Deltn(uint16_t Deltn)
{
    KT_Byte_Write(0x85, sys_clk[Deltn] & 0x0f);
}

//87500000
/**
* Dai tan so lam viec: 32MHz - 110 MHz
* Neu tan so cai dat ngoai dai -> bo qua
*/
uint8_t KT_FM_Tune(uint16_t iFrequency) //87.5MHz-->Frequency=8750; Mute the chip and Tune to Frequency
{
    uint8_t reg88;
    uint16_t temp_Freq;

    if (iFrequency == 0)
        return 0;

    KT_AMFM_Mute();
    temp_Freq = iFrequency / 5;

#ifdef FM_DELTN
    KT_FM_Deltn(temp_Freq - 0x05F0);
#endif

    KT_Byte_Write(0x89, temp_Freq & 0x00FF); //write FM channel
    reg88 = KT_Byte_Read(0x88);
    KT_Byte_Write(0x88, (reg88 & 0xF0) | ((temp_Freq >> 8) & 0x000F) | 0x80);

    return (1);
}

/**
  * @brief  Initializes the KT0935_I2C.
  * @param  None
  * @retval None
  */
void FM_Init(void)
{
    uint8_t RetryNumber = 0;
    uint16_t conf;

    KT0935_LowLevel_Init();
    
#if 0
    I2C_InitTypeDef I2C_InitStructure;
    /* KT0935_I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    I2C_InitStructure.I2C_DigitalFilter = 0x00;
    I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_Timing = KT0935_I2C_TIMING;

    /* Apply KT0935_I2C configuration after enabling it */
    I2C_Init(KT0935_I2C, &I2C_InitStructure);

    /* KT0935_I2C Peripheral Enable */
    I2C_Cmd(KT0935_I2C, ENABLE);
#endif

    /* ****************** Check status **********************/
    ErrorStatus res = ERROR;
    //xSystem.Status.FM.State = STATUS_FAIL;
    while (RetryNumber < 10)
    {
        res = FM_GetStatus();

        if (res == SUCCESS)
        {
            DEBUG_INFO("Init FM...OK\r\n");
            //xSystem.Status.FM.State = STATUS_OK;
            break;
        }
        else
        {
            RetryNumber++;
            DEBUG_WARN("Init FM...ERR, retry: %u\r\n", RetryNumber);
            Delayms(500);
        }
    }

    uint8_t power_on_check = 0;
    //0. Init i2c
    DEBUG_INFO("KT - 1 Check Reg0x02\r\n");

    //1. Check Reg0x02
    if (KT_AMFM_PreInit())
    {
        DEBUG_INFO(" OK\r\n");
    }
    else
    {
        DEBUG_ERROR(" Fail\r\n");
    }
    //2. Standby
    DEBUG_INFO("KT - 2 Stanby\r\n");
    //KT_AMFM_Standby();

    //3. Check Power on?
    DEBUG_INFO("KT - 3 Check power on\r\n");
    power_on_check = KT_AMFM_WakeUp();
    if (power_on_check == 0)
    {
        DEBUG_ERROR(" Fail");
    }
    else
    {
        DEBUG_INFO(" OK");
    }
    //KT093x_amp_auto();
    KT_AMFM_SetMode(FM_MODE);
    //KT093x_User_FMTune(xSystem.Parameter.Frequency[xSystem.Parameter.FreqIndex]); //MHz x100
    //FM_SetVolume(xSystem.Parameter.LocalVolume);
    xSystem.Parameter.FreqRun = 5750;
    KT093x_User_FMTune(5750);
    //KT093x_User_FMTune(10270);
    //xSystem.Parameter.Mode = MODE_IDLE;
    //xSystem.Parameter.ModeLocal = MODE_FM_LOCAL;
//    FM_SetVolume(50);
    return;
    //====================================

    //	uint16_t regx, regy;
    //	while (1)
    //	{
    //		Delayms(10);
    //		regx = FM_ReadReg(0x02);           			//Read Manufactory ID
    //		regy = FM_ReadReg(0x03);
    //		regx = (regx << 8) | regy ;
    //	  	if (regx != 0x4B54) continue;
    //		break;
    //	}
    //	FM_Standby(STANDBY_MODE);
    //	Delayms(1000);

    //	/* Set to Normal mode */
    //	FM_Standby(NORMAL_MODE);

    //	/* ================== Wakeup ===================*/
    //	regx = FM_ReadReg(0x0E);
    //	FM_WriteReg(0x0E, regx & 0xDF);	//Write STBY bit to 0
    //	Delayms(10);
    //	regx = FM_ReadReg(0x76);
    //	FM_WriteReg(0x76, regx | 0x02);	//Write STBYLDO_PD bit to 1
    //	KT_AMFM_Init();

    //	FM_WriteReg(0xFC, 0);
    //	FM_WriteReg(0xFD, 0);
    //	FM_WriteReg(0xFE, 1);
    //	FM_WriteReg(0xFF, 0x1C);
    //	FM_WriteReg(0xFB, 0x03);
    //
    //	KT_AMFM_SetMode(FM_MODE);
    //	KT_FM_Tune(8750);
    //	KT_AMFM_VolumeSet(30);

    //	Delayms(1000);
    //	/* Set to Normal mode */
    //	FM_Standby(NORMAL_MODE);
    //
    //	/* ****************** Check PowerOn finish **********************/
    //	while (1)
    //	{
    //		Delayms(100);
    //		conf = FM_ReadReg(0x1B);
    //		if(conf != KT0935_FAIL && (conf & 0x04)) break;
    //	}
    //	DEBUG_INFO("\rFM Power On Finish flag: %u", conf);

    /* ****************** Read DeviceID **********************/
    uint16_t devID0 = FM_ReadReg(FM_REG_DEVICEID0); //0x82
    uint16_t devID1 = FM_ReadReg(FM_REG_DEVICEID1); //0x06
    DEBUG_INFO("Read device id0: %02X, id1: %02X\r\n", devID0, devID1);

#if __USE_EXTERNAL_CLOCK__
    /* ****************** Config to use 32.768KHZ external reference clock **********************/
    //RCLK_EN = 1
    conf = FM_WriteReg(0x0D, 0xD3);
    DEBUG_INFO("Config to use External clock. Write to %02X: %X\r\n", 0x0D, conf);

#if 1
    /* Use freq 32.768 KHz, no need to config DIVIDERP/N, it's used same as internal crystal */
    //Write to SYSCLK_CFG0, SYSCLK_CFG1, SYSCLK_CFG2
    conf = FM_WriteReg(FM_REG_SYSCLKCFG0, 0x08);
    DEBUG_INFO("Write to %02X: %X\r\n", FM_REG_SYSCLKCFG0, conf);

    conf = FM_WriteReg(FM_REG_SYSCLKCFG1, 0x00);
    DEBUG_INFO("Write to %02X: %X\r\n", FM_REG_SYSCLKCFG1, conf);

    conf = FM_WriteReg(FM_REG_SYSCLKCFG2, 0x00);
    DEBUG_INFO("Write to %02X: %X\r\n", FM_REG_SYSCLKCFG2, conf);
#endif
#endif //__USE_EXTERNAL_CLOCK__ = 1

    //Read XTAL mode
    conf = FM_ReadReg(0x0D);
    DEBUG_INFO("FM XTAL mode: %s\r\n", (conf & 0x10) ? "External Ref clock" : "Internal Crystal");

    //Soft mute
    conf = FM_WriteReg(0x1A, 0xF8);
    DEBUG_INFO("FM Softmute disable: %02X\r\n", conf);

    //Read PLLCFG reg
    for (uint8_t regNo = 0x04; regNo <= 0x0A; regNo++)
    {
        uint16_t regVal = FM_ReadReg(regNo);
        DEBUG_INFO("FM: Read reg 0x%02X, value: 0x%02X\r\n", regNo, regVal);
    }

    /* ********************* Set FM Frequency **********************/
    conf = FM_SetFMChanFreq(100000000); //Set freq 102.7MHz

    /* ****************** Set FM Volume **********************/
    conf = FM_SetVolume(20);
}

/**
* Tick 500ms
*/
void FM_Tick(void)
{
    static uint8_t fmTick5s = 0;
    static uint8_t lastSNR = 0, lastRSSI = 0;
    static uint8_t errorTimeout = 0;

    //Đọc trạng thái mỗi 3s
    if (fmTick5s++ >= 6)
    {
        fmTick5s = 0;
        DEBUG_INFO("");

        xSystem.Status.FM.Volume = FM_GetVolume();
        xSystem.Status.FM.Frequency = FM_GetFMChanFreq(); /* Real Freq in Hz */
        xSystem.Status.FM.SNR = FM_GetSNR();
        xSystem.Status.FM.RSSI = FM_GetRSSI();

        DEBUG_RAW("Status 0: %02X\r\n", FM_ReadReg(0xDE));

        /** Phat hien trang thai loi IC FM -> Chi so tin hieu k thay doi trong 1 thoi gian
		* NOTE: Chi khi chay che do thu FM thi SNR, RSSI moi thay doi -> Khong check o che do khac
		*/
        if (xSystem.Parameter.Mode == MODE_FM_REMOTE || xSystem.Parameter.ModeLocal == MODE_FM_LOCAL)
        {
            if (lastSNR == xSystem.Status.FM.SNR && lastRSSI == xSystem.Status.FM.RSSI)
            {
                if (errorTimeout++ >= 20)
                {
                    DEBUG_ERROR("--->IC FM co loi, reset...\r\n");
                    SystemReset(0xF);
                }
            }
            else
            {
                lastSNR = xSystem.Status.FM.SNR;
                lastRSSI = xSystem.Status.FM.RSSI;
                errorTimeout = 0;
            }
        }
    }

    /** Kiểm tra lệnh cần Read/Write */
    if (KT_Write_List.total > 0)
    {
        for (uint8_t i = 0; i < KT_REG_LIST_MAX; i++)
        {
            if (KT_Write_List.List[i].isWrite)
            {
                DEBUG_INFO("KT: Write reg %X->%X\r\n", KT_Write_List.List[i].reg, KT_Write_List.List[i].value);

                FM_WriteReg(KT_Write_List.List[i].reg, KT_Write_List.List[i].value);
                KT_Write_List.List[i].isWrite = 0;
                KT_Write_List.total--;
            }
        }
    }
    else
    {
        if (KT_Read_List.total > 0)
        {
            for (uint8_t i = 0; i < KT_REG_LIST_MAX; i++)
            {
                if (KT_Read_List.List[i].isRead)
                {
                    DEBUG_INFO("KT: Read reg %X\r\n", KT_Read_List.List[i].reg);

                    KT_Read_List.List[i].value = FM_ReadReg(KT_Read_List.List[i].reg);
                    KT_Read_List.List[i].isRead = 0;
                    KT_Read_List.List[i].isReadDone = 1;
                    KT_Read_List.total--;
                }
            }
        }
    }
}

/*****************************************************************************/
/**
 * @brief	:  	Cau hinh tan so FM
 * @param	:  32 - 110MHz
 * @retval	:
 * @author	:	
 * @created	:
 * @version	:
 * @reviewer:	
 */
uint16_t FM_SetFMChanFreq(uint32_t freq)
{
    //100.000.000/10000
    uint32_t freqConf = freq / 10000;
    KT093x_User_FMTune(freqConf); //Freq in MHz x 100
    return 0;

    //FM Freq = 12bit, step = 50KHz
    FMCHAN0_Value_t FMCh0;

    //Check FM freq?
    if (freq < 32000000 || freq > 110000000)
    {
        DEBUG_ERROR("FM freq is out of range 32-110 MHz!\r\n");
        return KT0935_FAIL;
    }

    FMCh0.Name.AM_FM = 0; /* 0 - FM mode, 1 - AM mode */
    FMCh0.Name.TUNE = 1;  /* Normal operation */

    //Convert freq in Hz to config value with step = 50KHz
    uint16_t confVal = freq / 50000;

    //Get bit [11:8] = 4bit MSB
    FMCh0.Name.FM_CHAN_11_8 = (confVal >> 8) & 0x0F;

    //Get bit [7:0]
    uint8_t FMCh1Value = confVal & 0xFF;

    //Write to FMCHAN0_REG
    uint16_t res0 = FM_WriteReg(0x88, FMCh0.Value);

    //Write to FMCHAN1_REG
    uint16_t res1 = FM_WriteReg(0x89, FMCh1Value);

    DEBUG_RAW("\rFM: Set Freq to %u: %X-%X", freq, res0, res1);

    return (res0 | res1);
}

/*
* Get FM channel Frequency
* return: Freq in Hz
*/
uint32_t FM_GetFMChanFreq(void)
{
    uint16_t fmCh0, fmCh1;
    uint32_t fmFreq = 0;

#if 1
    fmCh0 = FM_ReadReg(0x88);
    fmCh1 = FM_ReadReg(0x89);

    DEBUG_RAW("FM: Receiver mode: %02X : %02X\r\n", fmCh0, fmCh1); //(fmCh0 & 0x40) ? "AM" : "FM");
    if (fmCh0 != KT0935_FAIL && fmCh1 != KT0935_FAIL)
    {
        fmCh0 &= 0x0F; //Get 4 bit FM[11:8]
        fmCh1 &= 0xFF; //Get LSB byte

        fmFreq = (fmCh0 << 8) | fmCh1;
        fmFreq *= 50000; //convert to Hz
    }

#else
    /* Co the doc thanh ghi STATUS6 (0xE4) va STATUS7 (0xE5) cung duoc 
	* FM.Freq = ([STATUS6]<< 8 | [STATUS7]) * 50KHz
	* Cau hinh 92.7MHz ma doc ra 0x06B8 = 86MHz (default) ???
	*/
    fmCh0 = FM_ReadReg(0xE4);
    fmCh1 = FM_ReadReg(0xE5);

    if (fmCh0 != KT0935_FAIL && fmCh1 != KT0935_FAIL)
    {
        fmCh0 &= 0x7F; //Get 6 bit FM[14:8]
        fmCh1 &= 0xFF; //Get LSB byte

        fmFreq = (fmCh0 << 8) | fmCh1;
        fmFreq *= 50000; //convert to Hz
    }
#endif

    //DEBUG_RAW("FM: Get Freq: %02X - %02X - %u\r\n", fmCh0, fmCh1, fmFreq);
    DEBUG_RAW("FM: Get Freq: %u\r\n", fmFreq / 10000);

    return fmFreq;
}

/*
* FM change operation mode: Normal - Standby
*/
uint16_t FM_Standby(FM_OperateMode_t newMode)
{
    uint8_t modeVal = 0x00;

    if (newMode == STANDBY_MODE)
        modeVal = 0x20;
    uint16_t res = FM_WriteReg(FM_REG_OPMODE, modeVal);

    DEBUG_RAW("Set FM operation mode %u : %X\r\n", newMode, res);

    return res;
}

/*
* Set FM Volume
*/
uint16_t FM_SetVolume(uint8_t volume)
{
    //0dB = 11111b, 00001 = -60dB, mute = 0
    //32 level = 100, step = 2dB

    if (volume >= 100)
        volume = 99;
    uint8_t cfgVal = volume / 3.125; //conver to 0-31 (mute - 0dB)
    uint16_t res = FM_WriteReg(FM_REG_VOLCONTROL, cfgVal);

    DEBUG_RAW("Set volume to %u/%u : %X\r\n", volume, cfgVal, res);

    return res;
}

/*
* Get FM Volume
*/
uint8_t FM_GetVolume(void)
{
    uint8_t volume = 0xFF;
    uint16_t regVal = FM_ReadReg(FM_REG_VOLCONTROL);

    if (regVal != KT0935_FAIL)
    {
        volume = (regVal + 1) * 3.125;
    }

    DEBUG_RAW("FM: Get volume: %u\r\n", volume);

    return volume;
}

/*
* Get FM SNR value, reg: 0xE2, value: 0 - 63
*/
uint8_t FM_GetSNR(void)
{
    uint8_t snr = 0xFF;
    uint16_t regVal = FM_ReadReg(FM_REG_FMSNR);

    if (regVal != KT0935_FAIL)
    {
        snr = regVal & 0x3F; //FMSNR<5:0>
    }
    DEBUG_RAW("FM: Get SNR: %u\r\n", snr);

    return snr;
}

/*
* Get FM RSSI value, reg: 0xE6, value: 0 - 127
* FM RSSI (dBm) = -110 + FM_RSSI<6:0>*1dB
*/
uint8_t FM_GetRSSI(void)
{
    uint8_t rssi = 0xFF;
    uint16_t regVal = FM_ReadReg(FM_REG_FMRSSI);

    if (regVal != KT0935_FAIL)
    {
        rssi = regVal & 0x7F; //FMRSSI<6:0>
    }
    xSystem.Status.FM.dBm = rssi - 110;

    DEBUG_RAW("FM: Get RSSI: %u, dBm: %d\r\n", rssi, rssi - 110);

    return rssi;
}

/**
 * @ingroup GA04 
 * @brief Returns true when the RDS system has valid information 
 * @details Returns true if RDS currently synchronized; the information are A, B, C and D blocks; and no errors 
 * @return  true or false
 */
bool hasRdsInfo()
{
    uint8_t sync_rds_reg = KT_Byte_Read(0x45);
    uint8_t err_rds_reg = KT_Byte_Read(0x46);
    return  ((sync_rds_reg & 0x80) && !(err_rds_reg & 0x00000011) );
}

/**
  * @brief  Checks the KT0935 status.
  * @param  None
  * @retval ErrorStatus: KT0935 Status (ERROR or SUCCESS).
  */
ErrorStatus FM_GetStatus(void)
{
/* wait until I2C bus is idle */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (i2c_flag_get(KT0935_I2C, I2C_FLAG_I2CBSY))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_ReadReg: I2C_GetFlagStatus() ISR_BUSY timeout\r\n");
            return ERROR;
        }
    }
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(KT0935_I2C);
    /* wait until SBSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_SBSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_ReadReg: I2C_GetFlagStatus() I2C_FLAG_SBSEND timeout\r\n");
            return ERROR;
        }
    }
    
    /* send slave address to I2C bus */
    i2c_master_addressing(KT0935_I2C, KT0935_ADDR, I2C_RECEIVER);
    /* wait until ADDSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_ADDSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_ReadReg: I2C_GetFlagStatus() I2C_FLAG_ADDSEND timeout\r\n");
            return ERROR;
        }
    }
    
    /* N=1,reset ACKEN bit before clearing ADDRSEND bit */
    i2c_ack_config(KT0935_I2C, I2C_ACK_DISABLE);
    /* clear ADDSEND bit */
    i2c_flag_clear(KT0935_I2C, I2C_FLAG_ADDSEND);
    /* N=1,send stop condition after clearing ADDRSEND bit */
    i2c_stop_on_bus(KT0935_I2C);
    
    /* wait until the RBNE bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_RBNE))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_ReadReg: I2C_GetFlagStatus() I2C_FLAG_RBNE timeout\r\n");
            return ERROR;
        }
    }
    
    i2c_stop_on_bus(I2C0);
    while (I2C_CTL0(I2C0)&0x0200)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_ReadReg: I2C_GetFlagStatus() I2C_FLAG_RBNE timeout\r\n");
            return ERROR;
        }
    }
    
    /* enable acknowledge */
    i2c_ack_config(KT0935_I2C, I2C_ACK_ENABLE);
    
    return SUCCESS;
}

/**
  * @brief  Read the specified register from the LM75.
  * @param  RegName: specifies the LM75 register to be read.
  *          This parameter can be one of the following values:  
  *            @arg KT0935_REG_TEMP: temperature register
  *            @arg KT0935_REG_TOS: Over-limit temperature register
  *            @arg KT0935_REG_THYS: Hysteresis temperature register
  * @retval LM75 register value.
  */
uint16_t FM_ReadReg(uint8_t RegName)
{
    uint8_t recvData = 0;
    /* wait until I2C bus is idle */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (i2c_flag_get(KT0935_I2C, I2C_FLAG_I2CBSY))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }
 
   
    /* send a start condition to I2C bus */
    i2c_start_on_bus(KT0935_I2C);
 
    /* wait until SBSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_SBSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }
 
    /* send slave address to I2C bus */
    i2c_master_addressing(KT0935_I2C, KT0935_ADDR, I2C_TRANSMITTER);
 
    /* wait until ADDSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_ADDSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(KT0935_I2C,I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (SET != i2c_flag_get( KT0935_I2C , I2C_FLAG_TBE))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }
 
    /* enable KT0935_I2C*/
    i2c_enable(KT0935_I2C);
    
    /* send the EEPROM's internal address to write to */
    i2c_data_transmit(KT0935_I2C, RegName);  
    
    /* wait until BTC bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_BTC))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(KT0935_I2C);
    
    /* wait until SBSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_SBSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }    
    
    /* send slave address to I2C bus */
    i2c_master_addressing(KT0935_I2C, KT0935_ADDR, I2C_RECEIVER);
 
     /* disable acknowledge */
    i2c_ack_config(KT0935_I2C,I2C_ACK_DISABLE);
    
    /* wait until ADDSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_ADDSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }        
    /* clear the ADDSEND bit */
    i2c_flag_clear(KT0935_I2C,I2C_FLAG_ADDSEND);
    
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(KT0935_I2C);
    
    /* while there is data to be read */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (KT0935Timeout--)
    {
        /* wait until the RBNE bit is set and clear it */
        if(i2c_flag_get(KT0935_I2C, I2C_FLAG_RBNE))
        {
            /* read a byte from the EEPROM */
            recvData = i2c_data_receive(KT0935_I2C);
            break;
        } 
    }
    
    /* wait until the stop condition is finished */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (I2C_CTL0(KT0935_I2C)&0x0200)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("[%s-%d]: I2C_GetFlagStatus() timeout\r\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
    }
    
    /* enable acknowledge */
    i2c_ack_config(KT0935_I2C,I2C_ACK_ENABLE);
 
    i2c_ackpos_config(KT0935_I2C,I2C_ACKPOS_CURRENT);


    return recvData;
}

/**
  * @brief  Write to the specified register of the LM75.
  * @param  RegName: specifies the LM75 register to be written.
  *          This parameter can be one of the following values:    
  *            @arg KT0935_REG_TOS: Over-limit temperature register
  *            @arg KT0935_REG_THYS: Hysteresis temperature register
  * @param  RegValue: value to be written to LM75 register.  
  * @retval None
  */
uint16_t FM_WriteReg(uint8_t RegName, uint8_t RegValue)
{
#if 0
    /* Test on BUSY Flag */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(KT0935_I2C, I2C_ISR_BUSY) != RESET)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_INFO("FM_WriteReg: I2C_GetFlagStatus() ISR_BUSY timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }

    /* Configure slave address, nbytes, reload, end mode and start or stop generation */
    I2C_TransferHandling(KT0935_I2C, KT0935_ADDR, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

    /* Wait until TXIS flag is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(KT0935_I2C, I2C_ISR_TXIS) == RESET)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_INFO("FM_WriteReg: I2C_GetFlagStatus() ISR_TXIS timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }

    /* Send Register address */
    I2C_SendData(KT0935_I2C, (uint8_t)RegName);

    /* Wait until TCR flag is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(KT0935_I2C, I2C_ISR_TCR) == RESET)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_INFO("FM_WriteReg: I2C_GetFlagStatus() ISR_TCR timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }

    /* Configure slave address, nbytes, reload, end mode and start or stop generation */
    I2C_TransferHandling(KT0935_I2C, KT0935_ADDR, 1, I2C_AutoEnd_Mode, I2C_No_StartStop); //2 -> 1 : only 1 byte address!

    /* Wait until TXIS flag is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(KT0935_I2C, I2C_ISR_TXIS) == RESET)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_INFO("FM_WriteReg: I2C_GetFlagStatus() ISR_TXIS timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }
    /* Write data to TXDR */
    I2C_SendData(KT0935_I2C, RegValue);

    /* Wait until STOPF flag is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(KT0935_I2C, I2C_ISR_STOPF) == RESET)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_INFO("FM_WriteReg: I2C_GetFlagStatus() ISR_STOPF timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }

    /* Clear STOPF flag */
    I2C_ClearFlag(KT0935_I2C, I2C_ICR_STOPCF);
#else
        /* wait until I2C bus is idle */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (i2c_flag_get(KT0935_I2C, I2C_FLAG_I2CBSY))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_WriteReg: I2C_GetFlagStatus() I2C_FLAG_I2CBSY timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }
    /* send a start condition to I2C bus */
    i2c_start_on_bus(KT0935_I2C);
    /* wait until SBSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_SBSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
             DEBUG_ERROR("FM_WriteReg: I2C_GetFlagStatus() I2C_FLAG_SBSEND timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }
    /* send slave address to I2C bus */
    i2c_master_addressing(KT0935_I2C, KT0935_ADDR, I2C_TRANSMITTER);
    
    /* wait until ADDSEND bit is set */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_ADDSEND))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_WriteReg: I2C_GetFlagStatus() I2C_FLAG_ADDSEND timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }
    /* clear ADDSEND bit */
    i2c_flag_clear(KT0935_I2C, I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_TBE))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_WriteReg: I2C_GetFlagStatus() I2C_FLAG_TBE timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }

    /* data transmission */
    i2c_data_transmit(KT0935_I2C, RegName);
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    /* wait until the TBE bit is set */
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_TBE))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_WriteReg: I2C_GetFlagStatus() I2C_FLAG_TBE timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }
    
    i2c_data_transmit(KT0935_I2C, RegValue);
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    /* wait until the TBE bit is set */
    while (!i2c_flag_get(KT0935_I2C, I2C_FLAG_TBE))
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("FM_WriteReg: I2C_GetFlagStatus() I2C_FLAG_TBE timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }
    
    /* send a stop condition to I2C bus */
    KT0935Timeout = KT0935_LONG_TIMEOUT;
    i2c_stop_on_bus(KT0935_I2C);
    while (I2C_CTL0(KT0935_I2C)&0x0200)
    {
        if ((KT0935Timeout--) == 0)
        {
            DEBUG_ERROR("send a stop condition to I2C bus I2C_FLAG_TBE timeout\r\n");
            return KT0935_TIMEOUT_UserCallback();
        }
    }
#endif
    return KT0935_OK;
}
#define RDS_GROUP_ID   17
static bool isDataValid[4] = {false, false, false, false}; // Kiem tra ca 4 group data valid => Set isValid = true
uint16_t Block_A, Block_B, Block_C, Block_D;
decoded_rds_data_t RDS_data[4];
uint16_t RDS_raw_data = 0;
static int8_t get_zoneId(uint16_t reg_data)
{
    int8_t zoneId = -1;
    if (reg_data == 0x0408)
    {
        zoneId = 0;
    }
    if (reg_data == 0x0409)
    {
        zoneId = 1;
    }
    if (reg_data == 0x040A)
    {
        zoneId = 2;
    }
    if (reg_data == 0x040F)
    {
        zoneId = 3;
    }
    return zoneId;
}
static uint8_t get_int_from_ascii(uint8_t ascii)
{
    uint8_t ret = 0xFF;
    //DEBUG_ERROR("ascii : 0x%02x\r\n", ascii);
    if (ascii >= 0x30 && ascii <= 0x39 )
        ret = ascii - 0x30;
    else if (ascii >= 0x41 && ascii <= 0x46)
        ret = ascii - 55;
    return ret;
}
static void decode(uint16_t reg)
{
    // get Rds value
    uint8_t data_high = (uint8_t)(reg >> 8) & 0xFF;
    uint8_t data_low = (uint8_t)reg & 0xFF;
    
    if (data_high > 0x46 | data_high < 0x30) return;
    if (data_low > 0x46 | data_low < 0x30) return;
    
    if (reg > 0x4646 | reg < 0x3030) return;
    uint8_t rds_high = get_int_from_ascii(data_high);
    uint8_t rds_low = get_int_from_ascii(data_low);
    if (rds_high > 0x0F | rds_low > 0x0F) return;
    uint8_t rds_value = (rds_high << 4) | rds_low;
    // Get Zone ID
    int8_t zoneId = get_zoneId(Block_B);
    if (zoneId == -1 | zoneId > 3 | zoneId  < 0) return;
    
    DEBUG_INFO("Zone ID = %d, 0x%02x, raw 0x%04x \r\n", zoneId, rds_value, reg); 
    isDataValid[zoneId] = true;
    RDS_data[zoneId].value = rds_value;
    RDS_raw_data = reg;
}
bool check_rds_has_data(uint8_t group_id)
{
    bool ret = false;
    if (group_id > 32) return false;
    uint8_t zone_id = (uint8_t)(group_id / 8);
    ret = isDataValid[zone_id];
    
    memset(isDataValid, 0, sizeof(isDataValid));
    return ret;
}
void rds_data_handle(uint8_t group_id)
{
    static uint16_t CountReadOff = 0;
    static uint16_t CountReadOn = 0;
    static uint16_t pre_wrong_value = 0;     
    if (group_id > 32) return;
    uint8_t zone_id = (uint8_t)(group_id / 8);
    uint8_t member_id = group_id - (uint8_t)(zone_id * 8);
    if (RDS_data[zone_id].value & (1 << (8 - member_id)))
    {
        if (++CountReadOn >= 2)
        {
            CountReadOn = 0;
            CountReadOff = 0;
            pre_wrong_value = 0;
            if (xSystem.Status.FM.State != 1)
            {
                xSystem.Status.FM.State = 1;
                xSystem.Parameter.Mode = MODE_IDLE;
                xSystem.Parameter.ModeLocal = MODE_FM_LOCAL;
                FM_SetVolume(50);
            }
        }
        DEBUG_ERROR("CHUAN,%d,%d\r\n", RDS_data[zone_id].value, (1 << (8 - member_id)));
    }
    else
    {
        DEBUG_ERROR("WRONG,%d,%d\r\n", RDS_data[zone_id].value, (1 << (8 - member_id)));
        
        if (pre_wrong_value == 0) pre_wrong_value = RDS_raw_data;
        else if (RDS_raw_data != pre_wrong_value)
        {
            pre_wrong_value = RDS_raw_data;
            return; // 3 gia tri wrong giong nhau thi moi turn off FM
        }
        
        if (++CountReadOff >= 3)
        {
            // Ðoc RDS Off lien tuc 3 lan => tat FM
            CountReadOn = 0;
            CountReadOff = 0;
            pre_wrong_value = 0;
            if (xSystem.Status.FM.State != 0)
            {
                xSystem.Status.FM.State = 0;
                xSystem.Parameter.ModeLocal = MODE_IDLE;
                FM_SetVolume(0);
            }
        }
    }
}
void RDS_Task(void)
{
    uint8_t regx;
    static uint16_t count_step = 0, count_read_time = 0, count_fail = 0;
    
    if (xSystem.Parameter.FreqRun != 5750) return;
    regx =  KT_Byte_Read(0x45);
    uint8_t reg2 =  KT_Byte_Read(0x46);
    //KT_Byte_Write(0x45, regx & 0xC7);	//Clear GRP_RDY_INT, BLK2_RDY_INT and BLK_RDY_INT
    DEBUG_WARN("REG - 0x%02x, 0x%02x \r\n", regx, reg2);
    uint8_t temp = regx & 0x07;
    if (!(regx & 0x80) && (regx & 0x40))
    {
        if (++count_fail >= 80)
        {
            count_fail = 0;
            if (xSystem.Status.FM.State != 0)
            {
                xSystem.Status.FM.State = 0;
                xSystem.Parameter.ModeLocal = MODE_IDLE;
                FM_SetVolume(0);
            }
            DEBUG_ERROR("FAIL, turn off FM\r\n");
        }
        return;
    }
    count_fail = 0;
    if (!(temp == 0x00 | temp == 0x04)) return;
    //if ((reg2 & 0x03) > 0x01) return;
    
    Block_A = KT_Byte_Read(0x47);
    Block_A = Block_A <<8;
    Block_A = Block_A | (KT_Byte_Read(0x48));

    Block_B = KT_Byte_Read(0x49);
    Block_B = Block_B <<8;
    Block_B = Block_B | (KT_Byte_Read(0x4a));

    Block_C = KT_Byte_Read(0x4b);
    Block_C = Block_C <<8;
    Block_C = Block_C | (KT_Byte_Read(0x4c));

    Block_D = KT_Byte_Read(0x4d);
    Block_D = Block_D <<8;
    Block_D = Block_D | (KT_Byte_Read(0x33));
    
    decode(Block_D);
    
    if (++count_read_time >= 5)
    {
        count_read_time = 0;
        if (check_rds_has_data(RDS_GROUP_ID))
        {
            rds_data_handle(RDS_GROUP_ID);
        }
    }
    
    DEBUG_WARN("Message RDS - a: 0x%04x, b: 0x%04x, c:0x%04x, d:0x%04x\r\n",
    Block_A,
    Block_B,
    Block_C,
    Block_D);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
