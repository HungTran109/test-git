/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "gd32e230c_eval.h"
#include "app_wdt.h"
#include "board_hw.h"
#include "app_flash.h"
#include "Transfer.h"
#include "HardwareManager.h"
#include "InitSystem.h"
#include "Led7.h"
#include "I2CFM.h"
#include "gps.h"

extern System_t xSystem;


static void task_wdt(uint32_t timeout_ms);

static uint32_t sys_now_ms(void);
static app_flash_network_info_t m_net_info __attribute__((aligned(4)));

static uint32_t TimeOut10ms = 0;
static uint32_t TimeOut100ms = 0;
static uint32_t TimeOut500ms = 0;
static uint32_t TimeOut1000ms = 0;
static uint32_t TimeOut3000ms = 0;

volatile uint32_t m_sys_tick = 0;
__IO uint16_t TimingDelay = 0;

static void ProcessTimeout10ms(void);
static void ProcessTimeout100ms(void);
static void ProcessTimeout500ms(void);
static void ProcessTimeout1000ms(void);

__IO uint16_t adc_volume = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

static uint32_t rtt_debug_output(const void *data, uint32_t size)
{
    return SEGGER_RTT_Write(0, data, size);
}

int main(void)
{
    app_wdt_start();
    systick_config();
    
    app_debug_init(sys_get_ms, (void*)0);
    app_debug_register_callback_print(rtt_debug_output);
    InitSystem();
    
    DEBUG_INFO("HELLO");

    while (1)
    {
        if(TimeOut10ms >= 10)
        {
            TimeOut10ms = 0;
            ProcessTimeout10ms();
        }

        if(TimeOut100ms >= 100)
        {
            TimeOut100ms = 0;
            ProcessTimeout100ms();
            GPS_ManagerTick();
        }
        
        if(TimeOut500ms >= 500)
        {
            TimeOut500ms = 0;
            ProcessTimeout500ms();
        }

        if(TimeOut1000ms >= 1000)
        {
            TimeOut1000ms = 0;
            ProcessTimeout1000ms();
            GPS_POWER_ON();
        }	
        if (TimeOut3000ms >= 300)
        {
            TimeOut3000ms = 0;
            RDS_Task();
        }
        
        /*if (SEGGER_RTT_HASDATA(0))
        {
            uint8_t c;
            while (SEGGER_RTT_Read(0, &c, 1))
            {
                xSystem.Debug->AddData(c);
            }
        }*/
    }
}

static void ProcessTimeout10ms(void)
{	
    ESP32_UART_Tick();
    xSystem.Debug->Tick();
    if (SET == adc_flag_get(ADC_FLAG_EOC))
    {
        adc_flag_clear(ADC_FLAG_EOC);
        adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        adc_volume = ADC_RDATA;
    }   
    Volume_Tick();
}

static void ProcessTimeout100ms(void)
{
    Hardware_XoaCoLoi();
    app_wdt_feed();
    Button_Tick();
}

/*
* 500ms
*/
static void ProcessTimeout500ms(void)
{
    Led7_Tick();
    FM_Tick();
}
	
static void ProcessTimeout1000ms(void)
{
    ESP32_ManagerTick();
}

void SystemManagementTask(void)
{
    m_sys_tick++;
    TimeOut10ms++;
    TimeOut100ms++;
    TimeOut500ms++;
    TimeOut1000ms++;
    TimeOut3000ms++;
}

void Delay_Decrement(void)
{
    if(TimingDelay > 0)
        TimingDelay--;
}

//Ham Delayms
void Delayms(uint16_t ms)
{
    DEBUG_INFO("[%u] Delay %dms\r\n", m_sys_tick, ms);
    TimingDelay = ms;
    while (TimingDelay)
    {
        if(TimingDelay % 5 == 0)
            app_wdt_feed();
    }
}

uint32_t sys_get_ms(void)
{
    return m_sys_tick;
}

