#include "Ultrasonic.h"

Ultrasonic ultrasonic(GROVE_ANALOG_1_1);


void setup() {
    Serial.begin(1152000);
    delay(4000);
    Serial.println("");
    Serial.println("--- START ---------------------------------------------------");

    Serial.println("### Setup completed.");
}
void loop() {
    long RangeInInches;
    long RangeInCentimeters;

    Serial.println("The distance to obstacles in front is: ");
    RangeInInches = ultrasonic.MeasureInInches();
    Serial.print(RangeInInches);//0~157 inches
    Serial.println(" inch");
    delay(250);

    RangeInCentimeters = ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
    Serial.print(RangeInCentimeters);//0~400cm
    Serial.println(" cm");
    delay(250);
}
