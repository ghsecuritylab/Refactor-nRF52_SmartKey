#include <RCS620S.h>
#include <inttypes.h>
#include <string.h>

#define COMMAND_TIMEOUT  400
#define PUSH_TIMEOUT    2100
#define POLLING_INTERVAL 500

#define LED_PIN LED_BUILTIN

RCS620S rcs620s;
int waitCardReleased = 0;

void setup()
{
  int ret;

  pinMode(LED_PIN, OUTPUT);   digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  Serial.flush();
  delay(1000);

  ret = rcs620s.initDevice();
  while (!ret) {}

  Serial.println("### Setup Complete.");
}

void loop()
{
  int ret;

  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  ret = rcs620s.polling();
  if (!ret) {
    if (waitCardReleased) {
      waitCardReleased = 0;
    }
  } else if (!waitCardReleased) {
      Serial.print("idm=0x");
      for (uint8_t i = 0; i < sizeof(rcs620s.idm); i++) {
        Serial.printf("%x", rcs620s.idm[i]);
      }
      Serial.println();

      Serial.print("pmm=0x");
      for (uint8_t i = 0; i < sizeof(rcs620s.pmm); i++) {
        Serial.printf("%x", rcs620s.pmm[i]);
      }
      Serial.println();
  }

  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}
