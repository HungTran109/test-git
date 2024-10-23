/******************************************************************************
 * @file    	DriverUART.c
 * @author  	Phinht
 * @version 	V1.0.0
 * @date    	02/03/2016
 * @brief   	
 ******************************************************************************/

/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
//#include "stm32f0xx_usart.h"
#include "gd32e23x.h"
#include "gd32e23x_usart.h"

#include "Hardware.h"
#include "DataDefine.h"
#include <stdarg.h>
#include "Integer.h"
#include "DriverUART.h"
#include "lwrb/lwrb.h"
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
 * @author	:	Phinht
 * @created	:	28/02/2016
 * @version	:
 * @reviewer:	
 */

static lwrb_t m_ringbuff;
static uint8_t m_tx_buffer[256];
void UART_Init(void *USARTx, uint32_t BaudRate)
{
#if 0
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    USART_DeInit(USARTx);

    if (USARTx == ESP32_UART)
    {
        GPIO_PinAFConfig(ESP32_UART_GPIO, ESP32_RX_PIN_SOURCE, GPIO_AF_4);
        GPIO_PinAFConfig(ESP32_UART_GPIO, ESP32_TX_PIN_SOURCE, GPIO_AF_4);

        GPIO_InitStructure.GPIO_Pin = ESP32_RX_PIN | ESP32_TX_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(ESP32_UART_GPIO, &GPIO_InitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = USART3_4_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        USART_ITConfig(ESP32_UART, USART_IT_RXNE, ENABLE);
    }

    if (USARTx == FM_UART)
    {
        GPIO_PinAFConfig(FM_UART_GPIO, FM_RX_PIN_SOURCE, GPIO_AF_4);
        GPIO_PinAFConfig(FM_UART_GPIO, FM_TX_PIN_SOURCE, GPIO_AF_4);

        GPIO_InitStructure.GPIO_Pin = FM_RX_PIN | FM_TX_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(FM_UART_GPIO, &GPIO_InitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = USART3_4_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        //		USART_OverSampling8Cmd(FM_UART, ENABLE);

        USART_ITConfig(FM_UART, USART_IT_RXNE, ENABLE);
    }

    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USARTx, &USART_InitStructure);

    /* Clear IT pending before enable */
    USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
    USART_ClearITPendingBit(USARTx, USART_IT_IDLE);

    USART_Cmd(USARTx, ENABLE);
#else
    if ((uint32_t)USARTx == 1)      // ESP32 UART
    {
        lwrb_init(&m_ringbuff, m_tx_buffer, sizeof(m_tx_buffer));
        nvic_irq_enable(USART1_IRQn, 0);
        /* enable COM GPIO clock */
        rcu_periph_clock_enable(RCU_GPIOA);

        /* connect port to USARTx_Tx */
        gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);

        /* connect port to USARTx_Rx */
        gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);

        /* configure USART Tx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
        
        /* enable USART clock */
        rcu_periph_clock_enable(RCU_USART1);

        /* USART configure */
        usart_deinit(USART1);
        usart_word_length_set(USART1, USART_WL_8BIT);
        usart_stop_bit_set(USART1, USART_STB_1BIT);
        usart_parity_config(USART1, USART_PM_NONE);
        usart_baudrate_set(USART1, BaudRate);
        usart_receive_config(USART1, USART_RECEIVE_ENABLE);
        usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

        usart_enable(USART1);
        usart_interrupt_enable(USART1, USART_INT_RBNE);
        usart_interrupt_enable(USART1, USART_INT_TBE);
    }
    else if ((uint32_t)USARTx == 0)     // GPS UART
    {    
        nvic_irq_enable(USART0_IRQn, 1);
        /* enable COM GPIO clock */
        rcu_periph_clock_enable(RCU_GPIOA);

        /* enable USART clock */
        rcu_periph_clock_enable(RCU_USART0);

        /* connect port to USARTx_Tx */
        gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

        /* connect port to USARTx_Rx */
        gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

        /* configure USART Tx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

        /* USART configure */
        usart_deinit(USART0);
        usart_baudrate_set(USART0, BaudRate);
        usart_receive_config(USART0, USART_RECEIVE_ENABLE);
        usart_interrupt_disable(USART0, USART_INT_TBE);
//    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

        usart_enable(USART0);
        usart_interrupt_enable(USART0, USART_INT_RBNE);
    }
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
void UART_Putc(void *USARTx, uint8_t c)
{
#if 0
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USARTx, (uint16_t)c);
#else
    if ((uint32_t)USARTx == 1)
    {
        while (!lwrb_get_free(&m_ringbuff))
        {
            
        }
        uint32_t tmp = USART_CTL0(USART1);
        if (!(tmp & USART_CTL0_TBEIE))
        {
            usart_interrupt_enable(USART1, USART_INT_TBE);
        }
        lwrb_write(&m_ringbuff, &c, 1);
//        usart_data_transmit((uint32_t)USART1, c);
//        while (RESET == usart_flag_get((uint32_t)USART1, USART_FLAG_TBE));
    }
//    else if ((uint32_t)USARTx == 0)
//    {
//        usart_data_transmit((uint32_t)USART0, c);
//        while (RESET == usart_flag_get((uint32_t)USART0, USART_FLAG_TBE));
//    }
#endif
}


size_t driver_uart_get_new_data_to_send(uint8_t *c)
{
    return lwrb_read(&m_ringbuff, c, 1);
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
void UART_Puts(void *USARTx, const char *str)
{
//    DEBUG_INFO("UART%d send %s\r\n", (int)USARTx, str);
    while (*str != '\0')
    {
        UART_Putc(USARTx, (uint16_t)(*str));
        str++;
    }
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UART_Putb(void *USARTx, uint8_t *Data, uint16_t Length)
{
    uint16_t i;

    for (i = 0; i < Length; i++)
    {
        UART_Putc(USARTx, Data[i]);
    }
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UART_itoa(void *USARTx, long val, int radix, int len)
{
    BYTE c, r, sgn = 0, pad = ' ';
    BYTE s[20], i = 0;
    DWORD v;

    if (radix < 0)
    {
        radix = -radix;
        if (val < 0)
        {
            val = -val;
            sgn = '-';
        }
    }
    v = val;
    r = radix;
    if (len < 0)
    {
        len = -len;
        pad = '0';
    }
    if (len > 20)
        return;
    do
    {
        c = (BYTE)(v % r);
        if (c >= 10)
            c += 7;
        c += '0';
        s[i++] = c;
        v /= r;
    } while (v);
    if (sgn)
        s[i++] = sgn;
    while (i < len)
        s[i++] = pad;
    do
        UART_Putc(USARTx, s[--i]);
    while (i);
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UART_Printf(void *USARTx, const char *str, ...)
{
    va_list arp;
    int d, r, w, s, l, i;

    // Check if only string
    for (i = 0;; i++)
    {
        if (str[i] == '%')
            break;
        if (str[i] == 0)
        {
            UART_Puts(USARTx, str);
            return;
        }
    }

    va_start(arp, str);

    while ((d = *str++) != 0)
    {
        if (d != '%')
        {
            UART_Putc(USARTx, d);
            continue;
        }
        d = *str++;
        w = r = s = l = 0;
        if (d == '0')
        {
            d = *str++;
            s = 1;
        }
        while ((d >= '0') && (d <= '9'))
        {
            w += w * 10 + (d - '0');
            d = *str++;
        }
        if (s)
            w = -w;
        if (d == 'l')
        {
            l = 1;
            d = *str++;
        }
        if (!d)
            break;
        if (d == 's')
        {
            UART_Puts(USARTx, va_arg(arp, char *));
            continue;
        }
        if (d == 'c')
        {
            UART_Putc(USARTx, (char)va_arg(arp, int));
            continue;
        }
        if (d == 'u')
            r = 10;
        if (d == 'd')
            r = -10;
        if (d == 'X' || d == 'x')
            r = 16; // 'x' added by mthomas in increase compatibility
        if (d == 'b')
            r = 2;
        if (!r)
            break;
        if (l)
        {
            UART_itoa(USARTx, (long)va_arg(arp, long), r, w);
        }
        else
        {
            if (r > 0)
                UART_itoa(USARTx, (unsigned long)va_arg(arp, int), r, w);
            else
                UART_itoa(USARTx, (long)va_arg(arp, int), r, w);
        }
    }

    va_end(arp);
}

DriverUART_t DriverUART =
{
        UART_Init,
        UART_Putc,
        UART_Puts,
        UART_Putb,
        UART_Printf
};

/********************************* END OF FILE *******************************/
