#define PIN_GROVE_LED       (GROVE_ANALOG_1_1)
#define INTERVAL            (100)

const int thresholdvalue=10;         //The threshold for which the LED should turn on.
float Rsensor; //Resistance of Sensor in K


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  pinMode(PIN_GROVE_LED,OUTPUT);            //Set the LED on Digital 12 as an OUTPUT
}

void loop() {
  int sensorValue = analogRead(PIN_GROVE_LED);
//    Rsensor=(float)(1023-sensorValue)*10/sensorValue;
//    if(Rsensor>thresholdvalue) {
//        digitalWrite(ledPin,HIGH);
//    }
//    else {
//        digitalWrite(ledPin,LOW);
//    }

  Serial.println("the analog read data is ");
  Serial.println(sensorValue);
//    Serial.println("the Sensor resistance is ");
//    Serial.println(Rsensor,DEC);//show the light intensity on the serial monitor;
  delay(1000);
}