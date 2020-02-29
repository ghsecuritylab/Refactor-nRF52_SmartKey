/*
本スケッチは、SuicaをFeliCaリーダー/ライターにかざすことで、サーボモーターを回すことができる。
登録されたSuicaかどうか検証する機能は、実装されていない。
Suica以外のFeliCaカードでも、同様の機能を実装することはできる。
*/
#include "felica_sweep.h"

#include <inttypes.h>
#include <string.h>

#include <RCS620S.h>
#include <Servo.h>

#define PIN_LED     LED_BUILTIN
#define PIN_SERVO   PIN_A4

constexpr uint16_t COMMAND_TIMEOUT = 400;
constexpr uint16_t PUSH_TIMEOUT = 2100;
constexpr uint16_t POLLING_INTERVAL = 500;

int servoPosition = 0;
int waitCardReleased = 0;

enum event_id {
  FELICA_READ,
  FELICA_WRITE,
  SERVO_SWEEP
};

typedef void (*funcPointer) ();

typedef struct {
  funcPointer fp;
  uint8_t event_id;
} felica_evt_t;

typedef struct {
  funcPointer fp;
  uint8_t event_id;
} servo_evt_t;

typedef struct {
  uint8_t eventId;
  felica_evt_t felica_read_evt;
  felica_evt_t felica_write_evt;
  servo_evt_t servo_sweep_evt;
} event_t;

felica_evt_t felica_read_evt;
felica_evt_t felica_write_evt;
servo_evt_t servo_sweep_evt;
// typedef void (* _felica_evt_read_cb) ();
// typedef void (* _felica_evt_write_cb) ();
// typedef void (* _servo_evt_sweep_cb) ();
void (* _felica_evt_read_cb) ();
void (* _felica_evt_write_cb) ();
void (* _servo_evt_sweep_cb) ();
void (* _event_cb) ();

event_t *pEvt;
event_t evt;

RCS620S rcs620s;
Servo servo;


void printFelicaId() {
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

void felicaReadCallback() {
  _event_cb = nullptr;
  printFelicaId();

  _event_cb = _servo_evt_sweep_cb;
  if (_event_cb != nullptr) _event_cb();
}

void felicaWriteCallback() {
  _event_cb = nullptr;
  return;
}

void servoSweepCallback() {
  _event_cb = nullptr;

  int pos;
  for (pos = 0; pos <= 180; pos += 15) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(150);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 15) { // goes from 180 degrees to 0 degrees
    servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(150);                       // waits 15ms for the servo to reach the position
  }
}

void initPointer() {
  _event_cb = nullptr;
}

void initFelica() {
  // felica_read_evt->fp = felicaReadCallback;
  // felica_read_evt->event_id = FELICA_READ;

  // felica_write_evt->fp = felicaWriteCallback;
  // felica_write_evt->event_id = FELICA_WRITE;
  felica_read_evt.fp = felicaReadCallback;
  felica_read_evt.event_id = FELICA_READ;

  felica_write_evt.fp = felicaWriteCallback;
  felica_write_evt.event_id = FELICA_WRITE;
}

void initServo() {
  // servo_sweep_evt->fp = servoSweepCallback;
  // servo_sweep_evt->event_id = SERVO_SWEEP;
  servo_sweep_evt.fp = servoSweepCallback;
  servo_sweep_evt.event_id = SERVO_SWEEP;
}

void initEvent() {
  pEvt = &evt;

  Serial.println("intiEvent");
  Serial.printf("pEvt=0x%x\n", pEvt);
  Serial.printf("eventId=%u\n", pEvt->eventId);

  pEvt->eventId = UINT8_MAX;

  Serial.printf("pEvt=0x%x\n", pEvt);
  Serial.printf("eventId=%u\n", pEvt->eventId);

  pEvt->felica_read_evt = felica_read_evt;
  pEvt->felica_write_evt = felica_write_evt;
  pEvt->servo_sweep_evt = servo_sweep_evt;
}

void setFelicaEventReadCallback() {
  // _felica_evt_read_cb = felica_read_evt->fp;
  _felica_evt_read_cb = felica_read_evt.fp;
  Serial.printf("_felica_evt_read_cb=0x%x\n", _felica_evt_read_cb);

}

void setFelicaEventWriteCallback() {
  // _felica_evt_write_cb = felica_write_evt->fp;
  _felica_evt_write_cb = felica_write_evt.fp;
}

void setServoEventSweepCallback() {
  // _servo_evt_sweep_cb = servo_sweep_evt->fp;
  _servo_evt_sweep_cb = servo_sweep_evt.fp;
}

void setEventCallbacks() {
  setFelicaEventReadCallback();
  setFelicaEventWriteCallback();
  setServoEventSweepCallback();
}

void eventHandler(event_t *pEvt) {
  Serial.println("eventHandler");

  Serial.printf("pEvt=0x%x\n", pEvt);
  Serial.printf("eventId=%u\n", pEvt->eventId);

  Serial.flush();
  delay(2500);

  switch (pEvt->eventId) {
    case FELICA_READ:
      _event_cb = _felica_evt_read_cb;

      Serial.printf("_event_cb=0x%x\n", _event_cb);

      Serial.flush();
      delay(1);
      delay(2500);

      if (_event_cb != nullptr) { _event_cb(); }
      break;
    case FELICA_WRITE:
      _event_cb = _felica_evt_write_cb;
      if (_event_cb) { _event_cb(); }
      break;
    case SERVO_SWEEP:
      _event_cb = _servo_evt_sweep_cb;
      if (_event_cb) { _event_cb(); }
      break;
    default:
      Serial.println("No available callback function found.");
      return;
  }
}


void setup() {
  pinMode(PIN_LED, OUTPUT);   digitalWrite(PIN_LED, LOW);

  Serial.begin(115200);
  // while (!Serial) { delay(10); }
  delay(4000);

  initPointer();
  initFelica();
  initServo();
  initEvent();
  setEventCallbacks();
  
  Serial.println("### Setup Start.");

  Serial.println("--- START ---------------------------------------------------");

  servo.attach(PIN_SERVO);  // attaches the servo on pin 9 to the servo object

  Serial.flush();
  delay(1);

  int ret = rcs620s.initDevice();
  while (!ret) {}

  Serial.println("### Setup Complete.");
}

void loop() {
  // Polling
  digitalWrite(PIN_LED, HIGH);

  rcs620s.timeout = COMMAND_TIMEOUT;
  int ret = rcs620s.polling();
  if (!ret) {
    if (waitCardReleased) {
      waitCardReleased = 0;
    }
  } else if (!waitCardReleased) {
    Serial.println("");

    Serial.println("FELICA_READ");
    Serial.printf("pEvt=0x%x\n", pEvt);
    Serial.printf("eventId=%u\n", pEvt->eventId);
    pEvt->eventId = FELICA_READ;
    Serial.printf("pEvt=0x%x\n", pEvt);
    Serial.printf("eventId=%u\n", pEvt->eventId);
    eventHandler(pEvt);
  }

  rcs620s.rfOff();

  digitalWrite(PIN_LED, LOW);
  delay(POLLING_INTERVAL);
}

