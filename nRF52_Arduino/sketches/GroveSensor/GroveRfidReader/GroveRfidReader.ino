/*
    See:
    http://wiki.seeedstudio.com/Grove-125KHz_RFID_Reader
    https://github.com/Seeed-Studio/RFID_Library
    This sketch enables to read RFID card and convert the read data into RFID.
*/

#include <NectisGroveSensorUart.h>

#define PIN_LED_PWM         GROVE_ANALOG_1_1

NectisGroveSensorUart GroveUart;


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  Serial.println("### Setup completed.");
}

void loop() {
  const char *rfidData = GroveUart.RfidRead();

  if (rfidData != NULL) {
    Serial.printf("RF Tag ID: %s", rfidData);
    Mcu.PwmActivate(PWM_LED_PIN, PWM_PRESCALER_PRESCALER_DIV_128);
  }
}
