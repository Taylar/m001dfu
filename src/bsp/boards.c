/**
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include "boards.h"
#include <stdint.h>
#include <stdbool.h>

static const uint8_t m_board_led_list[LEDS_NUMBER] = LEDS_LIST;

static const uint8_t m_board_movt_list[MOVTS_NUMBER] = MOVTS_LIST;

static const uint8_t m_board_btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;

bool bsp_board_led_state_get(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    bool pin_set = nrf_gpio_pin_out_read(m_board_led_list[led_idx]) ? true : false;
    return (pin_set == (LEDS_ACTIVE_STATE ? true : false));
}

void bsp_board_led_on(uint32_t led_idx)
{
        ASSERT(led_idx < LEDS_NUMBER);
        nrf_gpio_pin_write(m_board_led_list[led_idx], LEDS_ACTIVE_STATE ? 1 : 0);
}

void bsp_board_led_off(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_write(m_board_led_list[led_idx], LEDS_ACTIVE_STATE ? 0 : 1);
}

void bsp_board_leds_off(void)
{
    uint32_t i;
    for (i = 0; i < LEDS_NUMBER; ++i)
    {
        bsp_board_led_off(i);
    }
}

void bsp_board_leds_on(void)
{
    uint32_t i;
    for (i = 0; i < LEDS_NUMBER; ++i)
    {
        bsp_board_led_on(i);
    }
}

void bsp_board_led_invert(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_toggle(m_board_led_list[led_idx]);
}


static void bsp_board_leds_init(void)
{
    uint32_t i;
    for (i = 0; i < LEDS_NUMBER; ++i)
    {
        nrf_gpio_cfg_output(m_board_led_list[i]);
    }
    bsp_board_leds_off();
}

uint32_t bsp_board_led_idx_to_pin(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    return m_board_led_list[led_idx];
}

uint32_t bsp_board_pin_to_led_idx(uint32_t pin_number)
{
    uint32_t ret = 0xFFFFFFFF;
    uint32_t i;
    for (i = 0; i < LEDS_NUMBER; ++i)
    {
        if (m_board_led_list[i] == pin_number)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

// ***********************************************************************************************************************************************
bool bsp_board_movt_state_get(uint32_t movt_idx)
{
    ASSERT(movt_idx < MOVTS_NUMBER);
    bool pin_set = nrf_gpio_pin_out_read(m_board_movt_list[movt_idx]) ? true : false;
    return (pin_set == (LEDS_ACTIVE_STATE ? true : false));
}

void bsp_board_movt_on(uint32_t movt_idx)
{
        ASSERT(movt_idx < MOVTS_NUMBER);
        nrf_gpio_pin_write(m_board_movt_list[movt_idx], LEDS_ACTIVE_STATE ? 1 : 0);
}

void bsp_board_movt_off(uint32_t movt_idx)
{
    ASSERT(movt_idx < MOVTS_NUMBER);
    nrf_gpio_pin_write(m_board_movt_list[movt_idx], LEDS_ACTIVE_STATE ? 0 : 1);
}

void bsp_board_movts_off(void)
{
    uint32_t i;
    for (i = 0; i < MOVTS_NUMBER; ++i)
    {
        bsp_board_movt_off(i);
    }
}

void bsp_board_movts_on(void)
{
    uint32_t i;
    for (i = 0; i < MOVTS_NUMBER; ++i)
    {
        bsp_board_movt_on(i);
    }
}

void bsp_board_movt_invert(uint32_t movt_idx)
{
    ASSERT(movt_idx < MOVTS_NUMBER);
    nrf_gpio_pin_toggle(m_board_movt_list[movt_idx]);
}

void bsp_board_movts_init(void)
{
    uint32_t i;
    for (i = 0; i < MOVTS_NUMBER; ++i)
    {
        nrf_gpio_cfg_output(m_board_movt_list[i]);
    }
    bsp_board_movts_off();
}

uint32_t bsp_board_movt_idx_to_pin(uint32_t movt_idx)
{
    ASSERT(movt_idx < MOVTS_NUMBER);
    return m_board_movt_list[movt_idx];
}

uint32_t bsp_board_pin_to_movt_idx(uint32_t pin_number)
{
    uint32_t ret = 0xFFFFFFFF;
    uint32_t i;
    for (i = 0; i < MOVTS_NUMBER; ++i)
    {
        if (m_board_movt_list[i] == pin_number)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

// ***********************************************************************************************************************************************
bool bsp_board_button_state_get(uint32_t button_idx)
{
    ASSERT(button_idx < BUTTONS_NUMBER);
    bool pin_set = nrf_gpio_pin_read(m_board_btn_list[button_idx]) ? true : false;
    return (pin_set == (BUTTONS_ACTIVE_STATE ? true : false));
}

static void bsp_board_buttons_init(void)
{
    uint32_t i;
    for (i = 0; i < BUTTONS_NUMBER; ++i)
    {
        nrf_gpio_cfg_input(m_board_btn_list[i], BUTTON_PULL);
    }
}

uint32_t bsp_board_pin_to_button_idx(uint32_t pin_number)
{
    uint32_t i;
    uint32_t ret = 0xFFFFFFFF;
    for (i = 0; i < BUTTONS_NUMBER; ++i)
    {
        if (m_board_btn_list[i] == pin_number)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

uint32_t bsp_board_button_idx_to_pin(uint32_t button_idx)
{
    ASSERT(button_idx < BUTTONS_NUMBER);
    return m_board_btn_list[button_idx];
}


void bsp_board_init(uint32_t init_flags)
{

    if (init_flags & BSP_INIT_LEDS)
    {
        bsp_board_leds_init();
    }

    if (init_flags & BSP_INIT_BUTTONS)
    {
        bsp_board_buttons_init();
    }

    if (init_flags & BSP_INIT_MOVTS)
    {
        bsp_board_movts_init();
    }

}
// ***********************************************************************************************************************************************
// movt timer

const nrf_drv_timer_t TIMER_MOVT = NRF_DRV_TIMER_INSTANCE(1);

void bsp_movt_timer_init(nrfx_timer_event_handler_t timeCb)
{
    uint32_t time_us = 500; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER_MOVT, &timer_cfg, timeCb);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_MOVT, time_us);

    nrf_drv_timer_extended_compare(
         &TIMER_MOVT, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    
}

void bsp_movt_timer_start(void)
{
    nrf_drv_timer_enable(&TIMER_MOVT);
}


void bsp_movt_timer_stop(void)
{
    nrf_drv_timer_disable(&TIMER_MOVT);
}

void bsp_movt_timer_set(uint32_t time_us)
{
    uint32_t time_ticks;

    time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_MOVT, time_us);
    nrf_drv_timer_extended_compare(
         &TIMER_MOVT, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
}

// ***********************************************************************************************************************************************
// rtc

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC0. */


/** @brief Function starting the internal LFCLK XTAL oscillator.
 */
static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

/** @brief Function initialization and configuration of RTC driver instance.
 */
void rtc_config(nrfx_rtc_handler_t handler)
{
    uint32_t err_code;

    lfclk_config();

    //Initialize RTC instance
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    config.prescaler = 4095;
    err_code = nrf_drv_rtc_init(&rtc, &config, handler);
    APP_ERROR_CHECK(err_code);

    //Enable tick event & interrupt
    nrf_drv_rtc_tick_enable(&rtc,true);

    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
   // err_code = nrf_drv_rtc_cc_set(&rtc,0, 8,true);
   // APP_ERROR_CHECK(err_code);

    //Power on RTC instance
}

void rtc_enable(void)
{
    nrf_drv_rtc_enable(&rtc);
}


void rtc_disable(void)
{
    nrf_drv_rtc_disable(&rtc);
}

void rtc_reset(void)
{
    nrf_drv_rtc_cc_set(&rtc,0, 8,true);
}
