#include "Arduino.h"
#include "AE_HX711.h"

//float offset;

void AE_HX711_Init(void)
{
  pinMode(pin_slk, OUTPUT);
  pinMode(pin_dout, INPUT);
}

void AE_HX711_Reset(void)
{
  digitalWrite(pin_slk,1);
  delayMicroseconds(100);
  digitalWrite(pin_slk,0);
  delayMicroseconds(100); 
}

long AE_HX711_Read(void)
{
  long data=0;
  while(digitalRead(pin_dout)!=0);
  delayMicroseconds(10);
  for(int i=0;i<24;i++)
  {
    digitalWrite(pin_slk,1);
    delayMicroseconds(5);
    digitalWrite(pin_slk,0);
    delayMicroseconds(5);
    data = (data<<1)|(digitalRead(pin_dout));
  }
  //Serial.println(data,HEX);   
  digitalWrite(pin_slk,1);
  delayMicroseconds(10);
  digitalWrite(pin_slk,0);
  delayMicroseconds(10);
  return data^0x800000; 
}


long AE_HX711_Averaging(long adc,char num)
{
  long sum = 0;
  for (int i = 0; i < num; i++) sum += AE_HX711_Read();
  return sum / num;
}

float AE_HX711_getGram(char num)
{
  #define HX711_R1  20000.0f
  #define HX711_R2  8200.0f
  #define HX711_VBG 1.25f
  #define HX711_AVDD      4.2987f//(HX711_VBG*((HX711_R1+HX711_R2)/HX711_R2))
  #define HX711_ADC1bit   HX711_AVDD/16777216 //16777216=(2^24)
  #define HX711_PGA 128
  #define HX711_SCALE     (OUT_VOL * HX711_AVDD / LOAD *HX711_PGA)
  
  float data;

  data = AE_HX711_Averaging(AE_HX711_Read(),num)*HX711_ADC1bit; 
  //Serial.println( HX711_AVDD);   
  //Serial.println( HX711_ADC1bit);   
  //Serial.println( HX711_SCALE);   
  //Serial.println( data);   
  data =  data / HX711_SCALE;


  return data;
}
