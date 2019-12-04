#include <SoftwareSerial.h>
#include <base64.hpp>     // https://github.com/Densaugeo/base64_arduino
#include <NectisCellular.h>

#define SHUTTER_PIN     (GROVE_ANALOG_1_1)

#define PIC_MAX_SIZE 10000
 
#define PIC_PKT_LEN    128        //data length of each read, dont set this too big because ram is limited
#define PIC_FMT_VGA    7
#define PIC_FMT_CIF    5
#define PIC_FMT_OCIF   3
#define CAM_ADDR       0
 
#define PIC_FMT        PIC_FMT_CIF

const byte cameraAddr = (CAM_ADDR << 5);  // addr
unsigned long picTotalLen = 0;            // picture length
 
unsigned char image_data[PIC_MAX_SIZE];
unsigned long image_length = 0;

SoftwareSerial CAM_SERIAL(GROVE_UART_RX_PIN, GROVE_UART_TX_PIN);

NectisCellular Nectis;

unsigned char base64[PIC_MAX_SIZE * 2];


void setup() {
  Serial.println("--- WAIT 5 sec ---");
  delay(5000);

  Serial.println("--- SETUP START ---");

  Serial.println("### I/O Initialize.");
  Nectis.Init();
  delay(500);

  Serial.println("### Power supply cellular ON.");
  Nectis.PowerSupplyCellular(true);
  delay(500);

  Serial.println("### Power supply Grove ON");
  Nectis.PowerSupplyGrove(true);
  pinMode(SHUTTER_PIN, INPUT);
  delay(500);

  CAM_SERIAL.begin(9600);
  initialize();
  preCapture();

  Serial.println("--- SETUP END ---");
}

void loop() {
  uint8_t buttonStateInt;
  buttonStateInt = digitalRead(SHUTTER_PIN);
  Serial.println(buttonStateInt);

  if (buttonStateInt == 1) {
    Serial.println("Take a photo");
    Capture();
    GetData();
  }

  delay(100);
}

void clearRxBuf()
{
  while (CAM_SERIAL.available())
  {
    CAM_SERIAL.read();
  }
}

void sendCmd(char cmd[], int cmd_len)
{
  for (char i = 0; i < cmd_len; i++) CAM_SERIAL.write(cmd[i]);
}

int readBytes(char *dest, int len, unsigned int timeout)
{
  int read_len = 0;
  unsigned long t = millis();
  while (read_len < len)
  {
    while (CAM_SERIAL.available()<1)
    {
      if ((millis() - t) > timeout) return read_len;
    }
    *(dest+read_len) = CAM_SERIAL.read();
    // Serial.write(*(dest+read_len));
    read_len++;
  }
  return read_len;
}

void initialize()
{   
  char cmd[] = {0xaa,0x0d|cameraAddr,0x00,0x00,0x00,0x00} ;
  unsigned char resp[6];

  Serial.print("initializing camera...");

  while (1)
  {
    sendCmd(cmd,6);
    if (readBytes((char *)resp, 6,1000) != 6)
    {
      Serial.print(".");
      continue;
    }
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x0d && resp[4] == 0 && resp[5] == 0)
    {
      if (readBytes((char *)resp, 6, 500) != 6) continue;
      if (resp[0] == 0xaa && resp[1] == (0x0d | cameraAddr) && resp[2] == 0 && resp[3] == 0 && resp[4] == 0 && resp[5] == 0) break;
    }
  }
  cmd[1] = 0x0e | cameraAddr;
  cmd[2] = 0x0d;
  sendCmd(cmd, 6);
  //Serial.println("\nCamera initialization done.");
}

void preCapture()
{
  char cmd[] = { 0xaa, 0x01 | cameraAddr, 0x00, 0x07, 0x00, PIC_FMT };
  unsigned char resp[6];

  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (readBytes((char *)resp, 6, 100) != 6) continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x01 && resp[4] == 0 && resp[5] == 0) break;
  }
}

void Capture()
{
  char cmd[] = { 0xaa, 0x06 | cameraAddr, 0x08, PIC_PKT_LEN & 0xff, (PIC_PKT_LEN>>8) & 0xff ,0};
  unsigned char resp[6];

  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (readBytes((char *)resp, 6, 100) != 6) continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x06 && resp[4] == 0 && resp[5] == 0) break;
  }
  cmd[1] = 0x05 | cameraAddr;
  cmd[2] = 0;
  cmd[3] = 0;
  cmd[4] = 0;
  cmd[5] = 0;
  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (readBytes((char *)resp, 6, 100) != 6) continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x05 && resp[4] == 0 && resp[5] == 0) break;
  }
  cmd[1] = 0x04 | cameraAddr;
  cmd[2] = 0x1;
  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (readBytes((char *)resp, 6, 100) != 6) continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x04 && resp[4] == 0 && resp[5] == 0)
    {
      if (readBytes((char *)resp, 6, 1000) != 6) continue;
      if (resp[0] == 0xaa && resp[1] == (0x0a | cameraAddr) && resp[2] == 0x01)
      {
        picTotalLen = (resp[3]) | (resp[4] << 8) | (resp[5] << 16);
        Serial.print("picTotalLen:");
        Serial.println(picTotalLen);
        break;
      }
    }
  }
}

void GetData()
{
  unsigned int pktCnt = (picTotalLen) / (PIC_PKT_LEN - 6);
  if ((picTotalLen % (PIC_PKT_LEN-6)) != 0) pktCnt += 1;

  char cmd[] = { 0xaa, 0x0e | cameraAddr, 0x00, 0x00, 0x00, 0x00 };
  unsigned char pkt[PIC_PKT_LEN];

  image_length = 0;

  for (unsigned int i = 0; i < pktCnt; i++)
  {
    cmd[4] = i & 0xff;
    cmd[5] = (i >> 8) & 0xff;

    int retry_cnt = 0;
    retry:
    delay(10);
    clearRxBuf();
    sendCmd(cmd, 6);
    uint16_t cnt = readBytes((char *)pkt, PIC_PKT_LEN, 200);

    unsigned char sum = 0;
    for (int y = 0; y < cnt - 2; y++) sum += pkt[y];
    if (sum != pkt[cnt-2])
    {
      if (++retry_cnt < 100) goto retry;
      else break;
    }

    if(picTotalLen < PIC_MAX_SIZE)
    {
      int co;
      for(co = 0; co < (cnt-6) ; co++)
      {
        image_data[image_length] = pkt[co + 4];
        image_length++;
      }
    }
  }

    if(picTotalLen < PIC_MAX_SIZE)
    {
      memset(base64,0,sizeof(base64));
      Serial.print("data:image/jpeg;base64,");
      encode_base64(image_data, image_length, base64);
      Serial.println((char*)base64);
    }
  
    cmd[4] = 0xf0;
    cmd[5] = 0xf0;
    sendCmd(cmd, 6);
}
