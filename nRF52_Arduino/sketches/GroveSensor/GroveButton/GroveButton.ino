/*
 * http://wiki.seeedstudio.com/Grove-Button/
 * This sketch enables to use GroveSensor-button.
 * When you press the button, you can find it on the serial monitor of ArduinoIDE.
 */

#define PIN_GROVE_BUTTON  (GROVE_ANALOG_1_1)
#define INTERVAL    (100)


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  pinMode(PIN_GROVE_BUTTON, INPUT);

  Serial.println("### Setup completed.");
}

void loop() {
  int buttonState = digitalRead(PIN_GROVE_BUTTON);
  Serial.print(buttonState ? '*' : '.');
  delay(INTERVAL);
}

