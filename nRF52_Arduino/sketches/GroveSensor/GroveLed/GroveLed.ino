/*
 * This sketch enables to blink GroveSensor-led.
 */

#define PIN_GROVE_LED   PIN_A0


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  Serial.println("### LED_CHECK_PIN ON.");
  pinMode(PIN_GROVE_LED, OUTPUT);
  
  Serial.println("### Setup completed.");
}

void loop() {
  Serial.println("### LED_CHECK_PIN HIGH/LOW.");

  digitalWrite(PIN_GROVE_LED, HIGH);
  delay(200);
  digitalWrite(PIN_GROVE_LED, LOW);
  delay(200);
}