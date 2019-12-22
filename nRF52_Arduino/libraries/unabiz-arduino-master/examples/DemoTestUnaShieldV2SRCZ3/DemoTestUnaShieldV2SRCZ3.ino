//  Send sample SIGFOX messages with UnaBiz UnaShield V2S Arduino Shield.
//  This sketch includes diagnostics functions in the UnaShield.
//  For a simpler sample sketch, see examples/send-light-level.
#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "NOTUSED";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_JP;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V2S Dev Kit
static String response;  //  Will store the downlink response from SIGFOX.

void setup() {  //  Will be called only once.
  //  Initialize console so we can see debug messages (9600 bits per second).
  initPins();
  delay(1);
  
  Serial.begin(115200);
  delay(4000);
  
  Serial.printf("WISOL_TX: %u\n", PIN_SERIAL_RX);
  Serial.printf("WISOL_RX: %u\n", PIN_SERIAL_TX);
  
  //  pinMode(PIN_SERIAL_RX, OUTPUT);   digitalWrite(PIN_SERIAL_RX, LOW);
  //  pinMode(PIN_SERIAL_TX, OUTPUT);   digitalWrite(PIN_SERIAL_TX, LOW);
  
  Serial.println(F("Running setup..."));
  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.
  
  //  Send a raw 12-byte message payload to SIGFOX.  In the loop() function we will use the Message class, which sends structured messages.
  transceiver.sendMessage("0102030405060708090a0b0c");
  
  //  Delay 10 seconds before sending next message.
  Serial.println(F("Waiting 10 seconds..."));
  delay(10000);
  
  
  //  Send message counter, temperature and voltage as a structured SIGFOX message, up to 10 times.
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nRunning loop #")); Serial.println(counter);
  
  //  Get temperature and voltage of the SIGFOX module.
  float temperature;  float voltage;
  transceiver.getTemperature(temperature);
  transceiver.getVoltage(voltage);
  
  //  Convert the numeric counter, temperature and voltage into a compact message with binary fields.
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("ctr", counter);  //  4 bytes for the counter.
  msg.addField("tmp", temperature);  //  4 bytes for the temperature.
  msg.addField("vlt", voltage);  //  4 bytes for the voltage.
  //  Total 12 bytes out of 12 bytes used.
  
  //  Send the message.
  if (msg.send()) {
    successCount++;  //  If successful, count the message sent successfully.
  } else {
    failCount++;  //  If failed, count the message that could not be sent.
  }
  counter++;
  
  //  Send only 10 messages.
  if (counter >= 10) {
    //  If more than 10 times, display the results and hang here forever.
    stop(String(F("Messages sent successfully: ")) + successCount +
         F(", failed: ") + failCount);  //  Will never return.
  }
}

void loop() {  //  Will be called repeatedly.
  //  //  Send message counter, temperature and voltage as a structured SIGFOX message, up to 10 times.
  //  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  //  Serial.print(F("\nRunning loop #")); Serial.println(counter);
  //
  //  //  Get temperature and voltage of the SIGFOX module.
  //  float temperature;  float voltage;
  //  transceiver.getTemperature(temperature);
  //  transceiver.getVoltage(voltage);
  //
  //  //  Convert the numeric counter, temperature and voltage into a compact message with binary fields.
  //  Message msg(transceiver);  //  Will contain the structured sensor data.
  //  msg.addField("ctr", counter);  //  4 bytes for the counter.
  //  msg.addField("tmp", temperature);  //  4 bytes for the temperature.
  //  msg.addField("vlt", voltage);  //  4 bytes for the voltage.
  //  //  Total 12 bytes out of 12 bytes used.
  //
  //  //  Send the message.
  //  if (msg.send()) {
  //    successCount++;  //  If successful, count the message sent successfully.
  //  } else {
  //    failCount++;  //  If failed, count the message that could not be sent.
  //  }
  //  counter++;
  //
  //  //  Send only 10 messages.
  //  if (counter >= 10) {
  //    //  If more than 10 times, display the results and hang here forever.
  //    stop(String(F("Messages sent successfully: ")) + successCount +
  //                   F(", failed: ") + failCount);  //  Will never return.
  //  }
  //
  //  //  Delay 10 seconds before sending next message.
  //  Serial.println("Waiting 10 seconds...");
  //  delay(10000);
}

void initPins() {
  pinMode(PIN_A0, OUTPUT);   digitalWrite(PIN_A0, LOW);
  pinMode(PIN_A1, OUTPUT);   digitalWrite(PIN_A1, LOW);
  pinMode(PIN_A2, OUTPUT);   digitalWrite(PIN_A2, LOW);
  pinMode(PIN_A3, OUTPUT);   digitalWrite(PIN_A3, LOW);
  pinMode(PIN_A4, OUTPUT);   digitalWrite(PIN_A4, LOW);
  pinMode(PIN_A5, OUTPUT);   digitalWrite(PIN_A5, LOW);
  pinMode(PIN_A6, OUTPUT);   digitalWrite(PIN_A6, LOW);
  pinMode(PIN_A7, OUTPUT);   digitalWrite(PIN_A7, LOW);
  
  pinMode(PIN_SERIAL_RX, INPUT);
  pinMode(PIN_SERIAL_TX, OUTPUT);   digitalWrite(PIN_SERIAL_TX, HIGH);
}
