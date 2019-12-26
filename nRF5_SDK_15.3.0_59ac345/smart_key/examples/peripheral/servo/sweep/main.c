/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
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

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_error.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "app_pwm.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "SEGGER_RTT.h"

#define PIN_SERVO               PIN_A4
#define DUTY_CYCLE_RESOLUTION   8

APP_PWM_INSTANCE(PWM1,1);                   // Create the instance "PWM1" using TIMER1.

static volatile bool ready_flag;            // A flag indicating PWM status.

/** @brief Function for initializing the nrf log module.
 */

static void log_init(void)
{
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);
  
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  
  SEGGER_RTT_WriteString(0, RTT_CTRL_CLEAR);
  SEGGER_RTT_WriteString(0, "log_init()\n");
}

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
  ready_flag = true;
}

void sweep_servo(void)
{
  SEGGER_RTT_WriteString(0, "sweep_servo()");

  ret_code_t err_code;
  
/** 2-channel PWM, 200Hz, output on DK LED pins.
    @ app_pwm.h
    @brief PWM instance default configuration (2 channels).
  #define APP_PWM_DEFAULT_CONFIG_2CH(period_in_us, pin0, pin1)                           \
      {                                                                                  \
          .pins            = {pin0, pin1},                                               \
          .pin_polarity    = {APP_PWM_POLARITY_ACTIVE_LOW, APP_PWM_POLARITY_ACTIVE_LOW}, \
          .num_of_channels = 2,                                                          \
          .period_us       = period_in_us                                                \
      }
**/
//  app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, PIN_SERVO, BSP_LED_1);
//  app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, BSP_LED_3, BSP_LED_1);
  app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(5000L, PIN_SERVO);
  
  /* Switch the polarity of the second channel. */
  pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
  
  /* Initialize and enable PWM. */
  err_code = app_pwm_init(&PWM1,&pwm1_cfg,pwm_ready_callback);
  APP_ERROR_CHECK(err_code);
  app_pwm_enable(&PWM1);
  
  uint32_t value;
  uint32_t angle = 90;
  if (angle > 180)
  {
    angle = 180;
  }
  else if (angle < 0)
  {
    angle = 0;
  }
  uint32_t max_duty_cycle = 100 * (180 - angle) / 180;
  
  //  while (true)
  for (uint8_t i = 0; i < 5; i++)
  {
    for (uint8_t i = 0; i < 40; ++i)
    {
//      value = (i < 20) ? (i * 5) : (100 - (i - 20) * 5);
      uint32_t unit = i * 5;
      value = (unit < max_duty_cycle) ? unit : (max_duty_cycle - unit);
      
      ready_flag = false;
      /* Set the duty cycle - keep trying until PWM is ready... */
      while (app_pwm_channel_duty_set(&PWM1, 0, value) == NRF_ERROR_BUSY);
      
      /* ... or wait for callback. */
      while (!ready_flag);
      APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, value));
      nrf_delay_ms(25);
    }
  }
}


//static void init_bsp()
//{
//  APP_ERROR_CHECK(nrf_drv_clock_init());
//  nrf_drv_clock_lfclk_request(NULL);
//
//  APP_ERROR_CHECK(app_timer_init());
//  SEGGER_RTT_WriteString(0, "init_bsp()\n");
//  APP_ERROR_CHECK(bsp_init(BSP_INIT_BUTTONS, bsp_evt_handler));
//  APP_ERROR_CHECK(bsp_buttons_enable());
//}


int main(void)
{
  log_init();
  SEGGER_RTT_WriteString(0, "PWM example started.");
  
  sweep_servo();

    for (;;)
    {
        // Wait for an event.
        __WFE();

        // Clear the event register.
        __SEV();
        __WFE();

        NRF_LOG_FLUSH();
    }
}
