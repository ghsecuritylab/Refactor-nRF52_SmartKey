/*
    See:
    http://wiki.seeedstudio.com/Grove-125KHz_RFID_Reader
    https://github.com/Seeed-Studio/RFID_Library
    This sketch enables to read RFID card and convert the read data into RFID.
*/

#include <NectisCellular.h>
#include <NectisMcu.h>
#include <NectisGroveSensorUart.h>

#define PWM_LED_PIN         GROVE_ANALOG_1_1

// Uncomment following line to use Http
//#define HTTP
// Uncomment following line to use Udp
#define UDP

NectisCellular Nectis;
NectisMcu Mcu;
NectisGroveSensorUart GroveUart;

bool isGroveUartEnabled;

// Settings for SORACOM binary parser
// RFTag_UID:0:uint:24 rssi:3:int:8 battery:4:uint:8
// Reserve 6 bytes for RF tag ID (3 bytes) and '\0'
char postData[4];


void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("");
  Serial.println("--- START ---------------------------------------------------");

  Serial.println("### I/O Initialize.");
  Nectis.Init();
  delay(100);
  Serial.println("### Power supply cellular ON.");
  Nectis.PowerSupplyCellular(true);
  delay(100);
  Serial.println("### Power supply ON.");
  // Make sure that the MODULE_PWR_PIN is set to HIGH.
  Nectis.PowerSupplyGrove(true);
  delay(100);

  // Initialize Uart between BL654 and GroveSensor Sensor (Grove_GPIO_5_1, Grove_GPIO_5_2)
  GroveUart.GroveUartSetup();
  GroveUart.GroveUartBegin();
  isGroveUartEnabled = true;

  Serial.println("### Setup completed.");
}

void loop() {
  // Enable Uart between BL654 and GroveSensor Sensor in order to get data from the GroveSensor uart Sensor again and again.
  if (!isGroveUartEnabled) {
    Nectis.PowerSupplyGrove(true);
    GroveUart.GroveUartBegin();
    isGroveUartEnabled = true;
    delay(1000);
  }
    
  const char *rfidData = GroveUart.RfidRead();

  if (rfidData != NULL) {
    Mcu.PwmActivate(PWM_LED_PIN, PWM_PRESCALER_PRESCALER_DIV_128);
  }
    
  if (rfidData != NULL) {
    // End Uart for GroveSensor Sensor in order to switch Uart from GroveSensor Sensor to BG96
    GroveUart.GroveUartEnd();
    Nectis.PowerSupplyGrove(false);
    isGroveUartEnabled = false;
  }
    
  if (rfidData != NULL) {
    // Extract RF Tag ID from read data
    char rfUidHex[6];
    for (int i = 0;i < 6;i ++) {
      rfUidHex[i] = rfidData[i + 4];
    }

    // Convert rfUidHex to decimal in order to display the monitor so that you can check the UID is even with the RF Tag UID.
    uint32_t rfUidDecimal = (uint32_t)(strtoul(rfUidHex, NULL, 16));
    Serial.printf("RFTag_UID: %010d ( %s )\n", rfUidDecimal, rfUidHex);

    unsigned int sizeOfPostDataRfUid = Mcu.GetDataDigits((unsigned int)rfUidDecimal);
    char *postDataRfUid;
    char tmpPostDataRfUid[sizeOfPostDataRfUid];
    postDataRfUid = Mcu.ConvertIntoBinary(tmpPostDataRfUid, rfUidDecimal, sizeOfPostDataRfUid);


    // ToDo: Need to enter PSM mode
    // Switch Uart from GroveSensor Sensor to BG96
    Nectis.Bg96Begin();
    Nectis.InitLteM();
    // Stabilize LTE-M communication.
    delay(5000);


    // Create post data in binary.
    memset(&postData[0], 0x00, sizeof(postData));
    memcpy(&postData[0], postDataRfUid, sizeOfPostDataRfUid);


    #ifdef HTTP
      Mcu.PwmActivate(PWM_LED_PIN, PWM_PRESCALER_PRESCALER_DIV_1);
      Nectis.PostDataViaHttp(postData);
    #endif  // HTTP

    #ifdef UDP
      Mcu.PwmActivate(PWM_LED_PIN, PWM_PRESCALER_PRESCALER_DIV_1);
      // Substract 1 from the sizeof(postData) because of the last character of postData is '0x00' for print function etc.
      Nectis.PostDataViaUdp(postData, sizeof(postData)-1);
    #endif  // UDP

    // Enter BG96 into sleep mode with AT command
    Nectis.Bg96TurnOff();
    // End the Uart between nRF52840 and BG96
    Nectis.Bg96End();
  }
}
