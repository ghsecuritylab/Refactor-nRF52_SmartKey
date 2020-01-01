/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
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
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_SERVO)
#include "ble.h"
#include "ble_servo.h"
#include "ble_srv_common.h"

#define NRF_LOG_MODULE_NAME ble_servo
#if BLE_SERVO_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL       BLE_SERVO_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  BLE_SERVO_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR BLE_SERVO_CONFIG_DEBUG_COLOR
#else // BLE_SERVO_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL       0
#endif // BLE_SERVO_CONFIG_LOG_ENABLED
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();


#define BLE_UUID_SERVO_SWEEP_CHARACTERISTIC  (0x6CD1)             /**< The UUID of the Servo Sweep Characteristic. */

#define BLE_SERVO_MAX_RX_CHAR_LEN            BLE_SERVO_MAX_DATA_LEN   /**< Maximum length of the RX Characteristic (in bytes). */

#define SERVO_BASE_UUID {{ 0xF9, 0xC0, 0xDE, 0x2A, 0x8A, 0x23, 0x1D, 0xA0, 0x0A, 0x46, 0x5B, 0xFF, 0x00, 0x00, 0x9F, 0x77 }} /**< Used vendor specific UUID. */


/**@brief Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the SoftDevice.
 *
 * @param[in] p_servo   Servo Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(ble_servo_t * p_servo, ble_evt_t const * p_ble_evt)
{
    ret_code_t                      err_code;
    ble_servo_evt_t                 evt;
    ble_gatts_value_t               gatts_val;
    uint8_t                         cccd_value[2];
    ble_servo_client_context_t *    p_client = NULL;

    NRF_LOG_INFO("on_connect");

    err_code = blcm_link_ctx_get(p_servo->p_link_ctx_storage,
                                 p_ble_evt->evt.gap_evt.conn_handle,
                                 (void *) &p_client);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Link context for 0x%02X connection handle could not be fetched.",
                      p_ble_evt->evt.gap_evt.conn_handle);
    }
    
    /* Check the hosts CCCD value to inform of readiness to send data using the RX characteristic */
    memset(&gatts_val, 0, sizeof(ble_gatts_value_t));
    gatts_val.p_value = cccd_value;
    gatts_val.len     = sizeof(cccd_value);
    gatts_val.offset  = 0;

    err_code = sd_ble_gatts_value_get(p_ble_evt->evt.gap_evt.conn_handle,
                                      p_servo->rx_handles.cccd_handle,
                                      &gatts_val);

    if ((err_code == NRF_SUCCESS)     &&
        (p_servo->data_handler != NULL) &&
        ble_srv_is_notification_enabled(gatts_val.p_value))
    {
        if (p_client != NULL)
        {
            p_client->is_notification_enabled = true;
        }

        memset(&evt, 0, sizeof(ble_servo_evt_t));
        evt.type        = BLE_SERVO_EVT_COMM_STARTED;
        evt.p_servo     = p_servo;
        evt.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        evt.p_link_ctx  = p_client;

        p_servo->data_handler(&evt);
    }
}


#include "boards.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_pwm.h"
#include "nrf_drv_pwm.h"
#include "SEGGER_RTT.h"

static volatile bool ready_flag;            // A flag indicating PWM status.

APP_PWM_INSTANCE(PWM1,1);                   // Create the instance "PWM1" using TIMER1.
static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    ready_flag = true;
}

/**@brief  Function for initializing the Servo module.
 */
/**@snippet [Servo Initialization] */
static void servo_init(void)
{
    ret_code_t err_code;
#define PIN_SERVO               PIN_A4
#define DUTY_CYCLE_RESOLUTION   8
    
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
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, PIN_SERVO, BSP_LED_1);
    //    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(5000L, PIN_SERVO);
    
    /* Switch the polarity of the second channel. */
    pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
    
    /* Initialize and enable PWM. */
    err_code = app_pwm_init(&PWM1,&pwm1_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM1);
}
/**@snippet [Servo Initialization] */

/**@brief   Function for handling app_pwm events.
 *
 * @details This function will sweep the servo motor connected to PIN_A4
 * when the data is written to the Servo Service.
*/
/**@snippet [Handling the Servo] */
void sweep_servo(void)
{
    /*
     * This demo plays back two concatenated sequences:
     * - Sequence 0: Light intensity is increased in 25 steps during one second.
     * - Sequence 1: LED blinks twice (100 ms off, 100 ms on), then stays off
     *   for 200 ms.
     * The same output is generated on all 4 channels (LED 1 - LED 4).
     * The playback is repeated in a loop.
     */

    enum { // [local constants]
        TOP        = 10000,
        STEP_COUNT = 25
    };

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            BSP_LED_4 | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_500kHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = TOP,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, NULL));

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM.
    static nrf_pwm_values_common_t seq0_values[STEP_COUNT];
    uint16_t value = 0;
    uint16_t step  = TOP / STEP_COUNT;
    uint8_t  i;
    for (i = 0; i < STEP_COUNT; ++i)
    {
        value         += step;
        seq0_values[i] = value;
    }

    nrf_pwm_sequence_t const seq0 =
    {
        .values.p_common = seq0_values,
        .length          = NRF_PWM_VALUES_LENGTH(seq0_values),
        .repeats         = 1,
        .end_delay       = 0
    };

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
    static nrf_pwm_values_common_t /*const*/ seq1_values[] =
    {
             0,
        0x8000,
             0,
        0x8000,
             0,
             0
    };
    nrf_pwm_sequence_t const seq1 =
    {
        .values.p_common = seq1_values,
        .length          = NRF_PWM_VALUES_LENGTH(seq1_values),
        .repeats         = 4,
        .end_delay       = 0
    };

//    (void)nrf_drv_pwm_complex_playback(&m_pwm0, &seq0, &seq1, 1,
//                                       NRF_DRV_PWM_FLAG_LOOP);


    for (uint8_t i = 0; i < 5; i++)
    {
        (void)nrf_drv_pwm_complex_playback(&m_pwm0, &seq0, &seq1, 1,
                                   NRFX_PWM_FLAG_STOP);

        while (!nrf_drv_pwm_is_stopped(&m_pwm0))
        {
            nrf_delay_ms(25);
        }
    }
    
    nrf_drv_pwm_stop(&m_pwm0, true);
    nrf_drv_pwm_uninit(&m_pwm0);
    
    nrf_gpio_pin_write(BSP_LED_2, 0);
    nrf_gpio_pin_write(BSP_LED_3, 0);
    nrf_gpio_pin_write(BSP_LED_4, 0);
    
//    uint32_t value;
//    // ToDO: 引数として、angleを受け取れるようにする。
//    uint32_t angle = 90;
//    if (angle > 180)
//    {
//        angle = 180;
//    }
//    else if (angle < 0)
//    {
//        angle = 0;
//    }
//    uint32_t max_duty_cycle = 100 * (180 - angle) / 180;
//
//    //  while (true)
//    for (uint8_t i = 0; i < 5; i++)
//    {
//        for (uint8_t i = 0; i < 40; ++i)
//        {
//            //      value = (i < 20) ? (i * 5) : (100 - (i - 20) * 5);
//            uint32_t unit = i * 5;
//            value = (unit < max_duty_cycle) ? unit : (max_duty_cycle - unit);
//
//            ready_flag = false;
//            /* Set the duty cycle - keep trying until PWM is ready... */
//            while (app_pwm_channel_duty_set(&PWM1, 0, value) == NRF_ERROR_BUSY);
//
//            /* ... or wait for callback. */
//            while (!ready_flag);
//            APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, value));
//            nrf_delay_ms(25);
//        }
//    }
}
/**@snippet [Handling the Servo] */

/**@brief Function for handling a write response.
 *
 * @param[in] p_ble_gattc_evt   Pointer to the GATT Client event.
 */
static void on_write(const ble_gattc_evt_t * const p_ble_gattc_evt)
{
    NRF_LOG_INFO("on_write");

    // For readability.
    const ble_gattc_evt_write_rsp_t * p_write_rsp = &(p_ble_gattc_evt->params.write_rsp);

    // Display gattc write status.
    NRF_LOG_RAW_INFO("Type of write operation: 0x%x\r\n", p_write_rsp->write_op);
    NRF_LOG_RAW_INFO("Data was written to the server \r\n");

    SEGGER_RTT_printf(0, "BLE_SERVO_MAX_DATA_LEN: %u\n", BLE_SERVO_MAX_DATA_LEN);
    
    char write_data[BLE_SERVO_MAX_DATA_LEN];
    memset(&write_data[0], 0x00, BLE_SERVO_MAX_DATA_LEN);
    
    for (uint8_t i = 0; i < p_write_rsp->len; i++)
    {
//        NRF_LOG_RAW_INFO("%d ", p_write_rsp->data[i]);
        SEGGER_RTT_printf(0, "%x(%c) ", p_write_rsp->data[i], (char)p_write_rsp->data[i]);
        write_data[i] = p_write_rsp->data[i];
    }
    
    if (p_write_rsp->len > 0)
    {
        NRF_LOG_RAW_INFO("\r\n");
        SEGGER_RTT_printf(0, "\n");
        
        SEGGER_RTT_printf(0, "write_data: %s\n\n", &write_data[0]);
    }
    
    if (strcmp( write_data, "Servo" ) == 0)
    
    sweep_servo();
}


/**@brief Function for handling the @ref BLE_GATTS_EVT_HVN_TX_COMPLETE event from the SoftDevice.
 *
 * @param[in] p_servo   Servo Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_hvx_tx_complete(ble_servo_t * p_servo, ble_evt_t const * p_ble_evt)
{
    ret_code_t                      err_code;
    ble_servo_evt_t                 evt;
    ble_servo_client_context_t *    p_client;

    err_code = blcm_link_ctx_get(p_servo->p_link_ctx_storage,
                                 p_ble_evt->evt.gatts_evt.conn_handle,
                                 (void *) &p_client);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Link context for 0x%02X connection handle could not be fetched.",
                      p_ble_evt->evt.gatts_evt.conn_handle);
        return;
    }

    if (p_client->is_notification_enabled)
    {
        memset(&evt, 0, sizeof(ble_servo_evt_t));
        evt.type        = BLE_SERVO_EVT_TX_RDY;
        evt.p_servo     = p_servo;
        evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
        evt.p_link_ctx  = p_client;

        p_servo->data_handler(&evt);
    }
}


#include "SEGGER_RTT.h"
void ble_servo_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    ble_servo_t * p_servo = (ble_servo_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            SEGGER_RTT_WriteString(0, "BLE_GAP_EVT_CONNECTED\n\n");
            on_connect(p_servo, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            SEGGER_RTT_WriteString(0, "BLE_GATTS_EVT_WRITE\n\n");
//            on_write(p_servo, p_ble_evt);
            on_write(&(p_ble_evt->evt.gattc_evt));
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            SEGGER_RTT_WriteString(0, "BLE_GATTS_EVT_HVN_TX_COMPLETE\n\n");
            on_hvx_tx_complete(p_servo, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t ble_servo_init(ble_servo_t * p_servo, ble_servo_init_t const * p_servo_init)
{
    ret_code_t            err_code;
    ble_uuid_t            ble_uuid;
    ble_uuid128_t         servo_base_uuid = SERVO_BASE_UUID;
    ble_add_char_params_t add_char_params;

    VERIFY_PARAM_NOT_NULL(p_servo);
    VERIFY_PARAM_NOT_NULL(p_servo_init);

    
    // Initialize the Servo module.
    servo_init();
    
    
    // Initialize the service structure.
    p_servo->data_handler = p_servo_init->data_handler;
    if (p_servo_init->servo_service_name_str.length > 0)
    {
        p_servo->servo_service_name_str = p_servo_init->servo_service_name_str;
    }

    
    /**@snippet [Adding proprietary Service to the SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&servo_base_uuid, &p_servo->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_servo->uuid_type;
    ble_uuid.uuid = BLE_UUID_SERVO_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_servo->service_handle);
    /**@snippet [Adding proprietary Service to the SoftDevice] */
    VERIFY_SUCCESS(err_code);

    // Add the Servo Sweep Characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid                     = BLE_UUID_SERVO_SWEEP_CHARACTERISTIC;
    add_char_params.uuid_type                = p_servo->uuid_type;
    add_char_params.max_len                  = BLE_SERVO_MAX_RX_CHAR_LEN;
    add_char_params.init_len                 = p_servo->servo_service_name_str.length;
    add_char_params.is_var_len               = true;
    add_char_params.p_init_value             = p_servo->servo_service_name_str.p_str;

    add_char_params.char_props.read          = 1;
    add_char_params.char_props.write         = 1;
    add_char_params.char_props.write_wo_resp = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_servo->service_handle, &add_char_params, &p_servo->rx_handles);
//    if (err_code != NRF_SUCCESS)
//    {
//        return err_code;
//    }
    return err_code;


//    // Add the TX Characteristic.
//    /**@snippet [Adding proprietary characteristic to the SoftDevice] */
//    memset(&add_char_params, 0, sizeof(add_char_params));
//    add_char_params.uuid              = BLE_UUID_NUS_TX_CHARACTERISTIC;
//    add_char_params.uuid_type         = p_nus->uuid_type;
//    add_char_params.max_len           = BLE_NUS_MAX_TX_CHAR_LEN;
//    add_char_params.init_len          = sizeof(uint8_t);
//    add_char_params.is_var_len        = true;
//    add_char_params.char_props.notify = 1;
//
//    add_char_params.read_access       = SEC_OPEN;
//    add_char_params.write_access      = SEC_OPEN;
//    add_char_params.cccd_write_access = SEC_OPEN;
//
//    return characteristic_add(p_nus->service_handle, &add_char_params, &p_nus->tx_handles);
//    /**@snippet [Adding proprietary characteristic to the SoftDevice] */
}


#endif // NRF_MODULE_ENABLED(BLE_SERVO)
