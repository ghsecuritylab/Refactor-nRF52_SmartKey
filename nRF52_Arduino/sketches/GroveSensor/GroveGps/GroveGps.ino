/*
 * http://wiki.seeedstudio.com/Grove-GPS/
 * This sketch enables to get GPS data.
 */
#include <NectisGroveSensorUart.h>

NectisGroveSensorUart GroveUart;


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  Serial.println("### Setup completed.");
}

void loop() {
  // Dont use delay(); Serial printed raw GPS data will be broken. GPS data will be sent in arbitrary time.
  const char *data = GroveUart.GpsRead();

  if (data != NULL && strncmp(data, "$GPGGA,", 7) == 0) {
    Serial.println(data);
  }

  if (GroveUart.IsGpsLocationUpdate()) {
    // GPS Data
    float lat = GroveUart.CalcGpsLatitude();
    float lng = GroveUart.CalcGpsLongitude();
    Serial.printf("latitude: %f\n", lat);
    Serial.printf("longitude: %f\n", lng);
  }
}