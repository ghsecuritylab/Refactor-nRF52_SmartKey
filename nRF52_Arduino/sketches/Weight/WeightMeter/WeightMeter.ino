#include <Arduino.h>
#include "config.h"
extern "C" {
  #include "AE_HX711.h"
};


void ClearSerialBuffer() {
  Serial.println();
  Serial.flush();
  delay(10);
}

float CorrectData(float data) {
  float correctedData = 0.0;

  if (isnan(data) && (data < 0)) {
    Serial.println("Failed to read from AE_HX711 sensor!");
    correctedData = 0.0;
  } else {
    correctedData = data - offset;

    if (correctedData < 0.0) {
      Serial.println("測定した重量が負の値です。");
      correctedData = 0.0;
    } else if (correctedData < 10.0) {
    // 分解能を10gに設定しているため
      correctedData = 0.0;
    } else { }
  }

  return correctedData;
}


void setup() {
  Serial.begin(115200);
  // while (!Serial) { delay(10); }
  delay(4000);

  Serial.println("### Setup Start.");

  Serial.println("--- START ---------------------------------------------------");

  AE_HX711_Init();
  AE_HX711_Reset();
  offset = AE_HX711_getGram(50); 

  Serial.println("### Setup Complete.");
}
 
void loop() {
  Serial.println("### Start to weigh.");

  float dataGram = AE_HX711_getGram(127);
  Serial.printf("dataGram=%.2f\n", dataGram);

  float correctedData = CorrectData(dataGram);
  Serial.printf("correctedData=%.2f\n", correctedData);

  ClearSerialBuffer();
  delay(MEASUREMENT_INTERVAL);
}
 