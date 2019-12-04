/*
 * http://wiki.seeedstudio.com/Grove-Relay/
 * This sketch enables to use GroveSensor-relay.
 */

#define PIN_GROVE_RELAY     (GROVE_ANALOG_1_1)


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  pinMode(PIN_GROVE_RELAY, OUTPUT);

  Serial.println("### Setup completed.");
}

void loop() {
  digitalWrite(PIN_GROVE_RELAY, HIGH);
}

