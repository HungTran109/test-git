#ifndef __HARDWARE_H__
#define __HARDWARE_H__

// #include "stm32f0xx.h"
#include "gd32e23x.h"
#include "gd32e23x_gpio.h"
#include "gd32e23x_usart.h"

#define GPIO_ReadOutputDataBit              gpio_output_bit_get
#define GPIO_ReadInputDataBit               gpio_input_bit_get
#define GPIOToggle                          gpio_bit_toggle
#define GPIO_WriteBit                       gpio_bit_write
#define GPIO_SetBits                        gpio_bit_set
#define GPIO_ResetBits                      gpio_bit_reset
#define BitAction                           bit_status
#define Bit_SET                             SET
#define Bit_RESET                           RESET

#define FLASH_Unlock                        fmc_unlock
#define FLASH_Lock                          fmc_lock
#define FLASH_ErasePage                     fmc_page_erase
#define FLASH_ProgramWord                   fmc_word_program
#define FLASH_Status                        fmc_state_enum
#define FLASH_COMPLETE                      FMC_READY


#define USART_GetITStatus(x, y)             usart_interrupt_flag_get((uint32_t)x, y)
#define USART_IT_RXNE                       USART_INT_FLAG_RBNE
#define USART_ReceiveData(x)                usart_data_receive((uint32_t)x)
#define USART_GetFlagStatus(x, y)           usart_flag_get((uint32_t)x, y)
#define USART_ClearFlag(x,y)                usart_flag_clear((uint32_t)x, y)
#define USART_FLAG_ORE                      USART_FLAG_ORERR
#define USART_FLAG_FE                       USART_FLAG_FERR

#define I2C_GetFlagStatus                   i2c_flag_get
#define I2C_ISR_BUSY                        I2C_FLAG_I2CBSY

#define KT0935_I2C                          I2C0

#define	GPS_TX_PIN			GPIO_PIN_10
#define	GPS_RX_PIN			GPIO_PIN_11

#define	GPS_UART_GPIO			GPIOB
#define GPS_UART                ((void*)0)

/************************* Defines for USART3 for ESP32 ***************************/
#define ESP32_UART ((void*)1)

#define ESP32_TX_PIN_SOURCE GPIO_PINSource10
#define ESP32_RX_PIN_SOURCE GPIO_PINSource11

#define ESP32_TX_PIN GPIO_PIN_10
#define ESP32_RX_PIN GPIO_PIN_11

#define ESP32_UART_GPIO GPIOB
#define ESP32_UART_IRQ USART3_4_IRQn

/************************* Defines for USART4 for FM ***************************/
//#define FM_UART ((void*)1)

#define FM_TX_PIN_SOURCE GPIO_PINSource0
#define FM_RX_PIN_SOURCE GPIO_PINSource1

#define FM_TX_PIN GPIO_PIN_0
#define FM_RX_PIN GPIO_PIN_1

#define FM_UART_GPIO GPIOA
#define FM_UART_IRQ USART3_4_IRQn
#define FM_UART_Handler USART0_IRQHandler

#define ADC_GPIO_PORT   GPIOA
#define ADC_GPIO_PIN    GPIO_PIN_0
#define BOARD_ADC_CHANNEL   ADC_CHANNEL_0


//User buttons
#define	BUTTON_UP_PIN       GPIO_PIN_4	
#define	BUTTON_UP_PORT      GPIOA

#define	BUTTON_DOWN_PIN	    GPIO_PIN_7
#define	BUTTON_DOWN_PORT    GPIOA

#define	BUTTON_LEFT_PIN	    GPIO_PIN_5
#define	BUTTON_LEFT_PORT    GPIOA

#define	BUTTON_RIGHT_PIN    GPIO_PIN_6
#define	BUTTON_RIGHT_PORT   GPIOA

#define	LED_MODE_4G_PIN		GPIO_PIN_1
#define	LED_MODE_4G_PORT		GPIOA

#define	LED_MODE_FM_PIN		GPIO_PIN_8
#define	LED_MODE_FM_PORT		GPIOA

//LED RGB
#define	RGB_RED_PIN		GPIO_PIN_15
#define	RGB_RED_PORT		GPIOA

#define	RGB_GREEN_PIN		GPIO_PIN_11
#define	RGB_GREEN_PORT		GPIOA

#define	RGB_BLUE_PIN		GPIO_PIN_12
#define	RGB_BLUE_PORT		GPIOA

//Cac chan dieu khien Anode LED7
#define	LED1_EN_PIN		GPIO_PIN_12
#define	LED1_EN_PORT		GPIOB

#define	LED2_EN_PIN		GPIO_PIN_13
#define	LED2_EN_PORT		GPIOB

#define	LED3_EN_PIN		GPIO_PIN_14
#define	LED3_EN_PORT		GPIOB

#define	LED4_EN_PIN		GPIO_PIN_15
#define	LED4_EN_PORT		GPIOB

#define 	LED7_DATA_PORT	GPIOB

#define GPS_POWER_PORT      GPIOC
#define GPS_POWER_PIN       GPIO_PIN_13

#define GPS_POWER_ON()      gpio_bit_set(GPS_POWER_PORT, GPS_POWER_PIN)
#define GPS_POWER_OFF()     gpio_bit_reset(GPS_POWER_PORT, GPS_POWER_PIN)
////==================== ADC ===================//
//#define ADC1_DR_Address 0x40012440

///* constant for adc resolution is 12 bit = 4096 */
#define ADC_12BIT_FACTOR 4096

///* constant for adc threshold value 3.3V */
#define ADC_VREF 3300
#define V_DIODE_DROP 250

//#define ADC_VIN_PIN GPIO_PIN_0
//#define ADC_VIN_PORT GPIOB
//#define ADC_VIN_CHANNEL ADC_Channel_8

#define ErrorStatus         ErrStatus

//Danh sach cac GPIO dieu khien
typedef union
{
    struct NameStruct_t
    {
        uint32_t AudioOutSW : 1;
        uint32_t AudioInSW : 1;
        uint32_t AudioPAEn : 1;
        uint32_t GSMPwrEn : 1;
        uint32_t GSMPwrKey : 1;
        uint32_t GSMReset : 1;
        uint32_t ISOOut1 : 1;
        uint32_t ISOOut2 : 1;
        uint32_t LED1Blue : 1;
        uint32_t LED1Red : 1;
        uint32_t LED2Blue : 1;
        uint32_t LED2Red : 1;
        //uint32_t NA : 20;
    } Name;
    uint32_t Value;
} GPIO_Control_t;

#endif /* __HARDWARE_H__ */
