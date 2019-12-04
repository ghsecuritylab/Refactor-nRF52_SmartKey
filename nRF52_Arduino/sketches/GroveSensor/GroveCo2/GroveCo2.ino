/*
 * http://wiki.seeedstudio.com/Grove-CO2_Sensor/
 * This sketch enables you to measure CO2 consentration with GroveSensor-co2 Sensor
 */
#include <NectisCellular.h>
#include <NectisGroveSensorUart.h>

NectisCellular Nectis;
NectisGroveSensorUart GroveUart;


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  Serial.println("### I/O Initialize.");
  Nectis.Init();
  delay(100);
  Serial.println("### Power supply cellular ON.");
  Nectis.PowerSupplyCellular(true);
  delay(100);
  Serial.println("### Power supply ON.");
  //  Make sure that the MODULE_PWR_PIN is set to HIGH.
  Nectis.PowerSupplyGrove(true);
  delay(100);

  // Initialize Uart between BL654 and GroveSensor Sensor (Grove_GPIO_5_1, Grove_GPIO_5_2)
  GroveUart.GroveUartSetup();
  GroveUart.GroveUartBegin();

  GroveUart.Co2Calibration();

  Serial.println("### Setup completed.");
}

void loop() {
  uint16_t co2Ppm = 0;

  co2Ppm = GroveUart.Co2Read();

  Serial.printf("CO2: %u\n", co2Ppm);

  delay(1000);
}


