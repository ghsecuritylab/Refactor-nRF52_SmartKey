////
//// Created by NaotoFujihiro on 2019-12-24.
////
//
//#ifndef FEATHER_NRF52840_EXPRESS
//#define FEATHER_NRF52840_EXPRESS
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#include "nrf_gpio.h"
//
//// LEDs definitions for Adafruit nRF52840 Feather
//#define LEDS_NUMBER    4
//
//#define LED_1          NRF_GPIO_PIN_MAP(0,13)
//#define LED_2          NRF_GPIO_PIN_MAP(0,14)
//#define LED_3          NRF_GPIO_PIN_MAP(0,15)
//#define LED_4          NRF_GPIO_PIN_MAP(0,16)
//#define LED_START      LED_1
//#define LED_STOP       LED_4
//
//#define LEDS_ACTIVE_STATE 0
//
//#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }
//
//#define LEDS_INV_MASK  LEDS_MASK
//
//#define BSP_LED_0      13
//#define BSP_LED_1      14
//#define BSP_LED_2      15
//#define BSP_LED_3      16
//
//#define BUTTONS_NUMBER 4
//
//#define BUTTON_1       11
//#define BUTTON_2       12
//#define BUTTON_3       24
//#define BUTTON_4       25
//#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
//
//#define BUTTONS_ACTIVE_STATE 0
//
//#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4 }
//
//#define BSP_BUTTON_0   BUTTON_1
//#define BSP_BUTTON_1   BUTTON_2
//#define BSP_BUTTON_2   BUTTON_3
//#define BSP_BUTTON_3   BUTTON_4
//
//#define RX_PIN_NUMBER  8
//#define TX_PIN_NUMBER  6
//#define CTS_PIN_NUMBER 7
//#define RTS_PIN_NUMBER 5
//#define HWFC           true
//
//#define BSP_QSPI_SCK_PIN   19
//#define BSP_QSPI_CSN_PIN   17
//#define BSP_QSPI_IO0_PIN   20
//#define BSP_QSPI_IO1_PIN   21
//#define BSP_QSPI_IO2_PIN   22
//#define BSP_QSPI_IO3_PIN   23
//
//
//// serialization APPLICATION board - temp. setup for running serialized MEMU tests
//#define SER_APP_RX_PIN              NRF_GPIO_PIN_MAP(1,13)    // UART RX pin number.
//#define SER_APP_TX_PIN              NRF_GPIO_PIN_MAP(1,14)    // UART TX pin number.
//#define SER_APP_CTS_PIN             NRF_GPIO_PIN_MAP(0,2)     // UART Clear To Send pin number.
//#define SER_APP_RTS_PIN             NRF_GPIO_PIN_MAP(1,15)    // UART Request To Send pin number.
//
//#define SER_APP_SPIM0_SCK_PIN       NRF_GPIO_PIN_MAP(0,27)     // SPI clock GPIO pin number.
//#define SER_APP_SPIM0_MOSI_PIN      NRF_GPIO_PIN_MAP(0,2)      // SPI Master Out Slave In GPIO pin number
//#define SER_APP_SPIM0_MISO_PIN      NRF_GPIO_PIN_MAP(0,26)     // SPI Master In Slave Out GPIO pin number
//#define SER_APP_SPIM0_SS_PIN        NRF_GPIO_PIN_MAP(1,13)     // SPI Slave Select GPIO pin number
//#define SER_APP_SPIM0_RDY_PIN       NRF_GPIO_PIN_MAP(1,15)     // SPI READY GPIO pin number
//#define SER_APP_SPIM0_REQ_PIN       NRF_GPIO_PIN_MAP(1,14)     // SPI REQUEST GPIO pin number
//
//// serialization CONNECTIVITY board
//#define SER_CON_RX_PIN              NRF_GPIO_PIN_MAP(1,14)    // UART RX pin number.
//#define SER_CON_TX_PIN              NRF_GPIO_PIN_MAP(1,13)    // UART TX pin number.
//#define SER_CON_CTS_PIN             NRF_GPIO_PIN_MAP(1,15)    // UART Clear To Send pin number. Not used if HWFC is set to false.
//#define SER_CON_RTS_PIN             NRF_GPIO_PIN_MAP(0,2)     // UART Request To Send pin number. Not used if HWFC is set to false.
//
//
//#define SER_CON_SPIS_SCK_PIN        NRF_GPIO_PIN_MAP(0,27)    // SPI SCK signal.
//#define SER_CON_SPIS_MOSI_PIN       NRF_GPIO_PIN_MAP(0,2)     // SPI MOSI signal.
//#define SER_CON_SPIS_MISO_PIN       NRF_GPIO_PIN_MAP(0,26)    // SPI MISO signal.
//#define SER_CON_SPIS_CSN_PIN        NRF_GPIO_PIN_MAP(1,13)    // SPI CSN signal.
//#define SER_CON_SPIS_RDY_PIN        NRF_GPIO_PIN_MAP(1,15)    // SPI READY GPIO pin number.
//#define SER_CON_SPIS_REQ_PIN        NRF_GPIO_PIN_MAP(1,14)    // SPI REQUEST GPIO pin number.
//
//#define SER_CONN_CHIP_RESET_PIN     NRF_GPIO_PIN_MAP(1,1)    // Pin used to reset connectivity chip
//
//// Arduino board mappings
//#define ARDUINO_SCL_PIN             27    // SCL signal pin
//#define ARDUINO_SDA_PIN             26    // SDA signal pin
//#define ARDUINO_AREF_PIN            2     // Aref pin
//
//#define ARDUINO_13_PIN              NRF_GPIO_PIN_MAP(1, 15)  // Digital pin 13
//#define ARDUINO_12_PIN              NRF_GPIO_PIN_MAP(1, 14)  // Digital pin 12
//#define ARDUINO_11_PIN              NRF_GPIO_PIN_MAP(1, 13)  // Digital pin 11
//#define ARDUINO_10_PIN              NRF_GPIO_PIN_MAP(1, 12)  // Digital pin 10
//#define ARDUINO_9_PIN               NRF_GPIO_PIN_MAP(1, 11)  // Digital pin 9
//#define ARDUINO_8_PIN               NRF_GPIO_PIN_MAP(1, 10)  // Digital pin 8
//
//#define ARDUINO_7_PIN               NRF_GPIO_PIN_MAP(1, 8) // Digital pin 7
//#define ARDUINO_6_PIN               NRF_GPIO_PIN_MAP(1, 7) // Digital pin 6
//#define ARDUINO_5_PIN               NRF_GPIO_PIN_MAP(1, 6) // Digital pin 5
//#define ARDUINO_4_PIN               NRF_GPIO_PIN_MAP(1, 5) // Digital pin 4
//#define ARDUINO_3_PIN               NRF_GPIO_PIN_MAP(1, 4) // Digital pin 3
//#define ARDUINO_2_PIN               NRF_GPIO_PIN_MAP(1, 3) // Digital pin 2
//#define ARDUINO_1_PIN               NRF_GPIO_PIN_MAP(1, 2) // Digital pin 1
//#define ARDUINO_0_PIN               NRF_GPIO_PIN_MAP(1, 1) // Digital pin 0
//
//#define ARDUINO_A0_PIN              3     // Analog channel 0
//#define ARDUINO_A1_PIN              4     // Analog channel 1
//#define ARDUINO_A2_PIN              28    // Analog channel 2
//#define ARDUINO_A3_PIN              29    // Analog channel 3
//#define ARDUINO_A4_PIN              30    // Analog channel 4
//#define ARDUINO_A5_PIN              31    // Analog channel 5
//
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif //FEATHER_NRF52840_EXPRESS



/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ha Thach for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FEATHER_NRF52840_EXPRESS
#define FEATHER_NRF52840_EXPRESS

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* Analog
 *------------------------------------------------------------------*/

#define PIN_A0                _PINNUM(0, 04)
#define PIN_A1                _PINNUM(0, 05)
#define PIN_A2                _PINNUM(0, 30)
#define PIN_A3                _PINNUM(0, 13)
#define PIN_A4                _PINNUM(0, 02)
#define PIN_A5                _PINNUM(0, 03)
#define PIN_A6                _PINNUM(0, 29)
#define PIN_A7                _PINNUM(0, 31)

static const uint8_t A0 = PIN_A0 ;
static const uint8_t A1 = PIN_A1 ;
static const uint8_t A2 = PIN_A2 ;
static const uint8_t A3 = PIN_A3 ;
static const uint8_t A4 = PIN_A4 ;
static const uint8_t A5 = PIN_A5 ;
static const uint8_t A6 = PIN_A6 ;
static const uint8_t A7 = PIN_A7 ;
#define ADC_RESOLUTION        14

// Other pins
#define PIN_AREF              PIN_A7
#define PIN_VBAT              PIN_A6
//#define PIN_NFC1              (33)
//#define PIN_NFC2              (2)

static const uint8_t AREF = PIN_AREF;

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
#define LEDS_NUMBER           5
#define LED_PRIMARY_PIN       _PINNUM(1, 15)
#define LED_SECONDARY_PIN     _PINNUM(1, 10)
#define LED_RED               LED_PRIMARY_PIN
#define LED_BLUE              LED_SECONDARY_PIN
#define LED_BUILTIN           LED_RED
#define LED_CONN              LED_BLUE

#define BSP_LED_0             LED_RED
#define BSP_LED_1             LED_BLUE
#define BSP_LED_2             PIN_A0
#define BSP_LED_3             PIN_A1
#define BSP_LED_4             PIN_A2

#define LED_0                 LED_RED
#define LED_1                 LED_BLUE
#define LED_2                 PIN_A0
#define LED_3                 PIN_A1
#define LED_4                 PIN_A2

#define LED_START             BSP_LED_0
#define LED_STOP              BSP_LED_1

#define LEDS_LIST             { LED_PRIMARY_PIN, LED_SECONDARY_PIN }
#define LEDS_ACTIVE_STATE     1
#define LED_STATE_ON          1

#define LED_NEOPIXEL          _PINNUM(0, 16)
#define NEOPIXELS_NUMBER      1

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
#define BUTTONS_NUMBER        2
#define BUTTON_0              _PINNUM(1, 02)
#define BUTTON_1              _PINNUM(0, 10)
#define BUTTON_PULL           NRF_GPIO_PIN_PULLUP
#define BUTTONS_ACTIVE_STATE  0
#define BUTTONS_LIST          { BUTTON_0, BUTTON_1 }

#define BSP_BUTTON_0          BUTTON_0
#define BSP_BUTTON_1          BUTTON_1

#define BUTTON_SWITCH         BUTTON_0
#define BUTTON_RESET          BUTTON_1

/*------------------------------------------------------------------*/
/* UART
 *------------------------------------------------------------------*/
#define PIN_SERIAL_RX         _PINNUM(0, 24)
#define PIN_SERIAL_TX         _PINNUM(0, 25)
#define RX_PIN_NUMBER         PIN_SERIAL_RX
#define TX_PIN_NUMBER         PIN_SERIAL_TX
#define CTS_PIN_NUMBER        0
#define RTS_PIN_NUMBER        0
#define HWFC                  false

/*------------------------------------------------------------------*/
/* BLE
 *------------------------------------------------------------------*/
// Used as model string in OTA mode
#define BLEDIS_MANUFACTURER   "Mira"
#define BLEDIS_MODEL          "SmartKey on nRF52840"

////--------------------------------------------------------------------+
//// USB
////--------------------------------------------------------------------+
//#define USB_DESC_VID           0x239A
//#define USB_DESC_UF2_PID       0x0029
//#define USB_DESC_CDC_ONLY_PID  0x002A
//
////------------- UF2 -------------//
//#define UF2_PRODUCT_NAME      "Adafruit Feather nRF52840 Express"
//#define UF2_VOLUME_LABEL      "FTHR840BOOT"
//#define UF2_BOARD_ID          "nRF52840-Feather-revD"
//#define UF2_INDEX_URL         "https://www.adafruit.com/product/4062"
//
//#ifdef __cplusplus
//}
//#endif

#endif // FEATHER_NRF52840_EXPRESS
