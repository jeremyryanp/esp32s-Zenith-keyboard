#include <BleMKeyboard.h>
#include "KEYMAP.h"

BleMKeyboard bleKeyboard("Zenith Keyboard", "Penntech customs", 100);

#define TEST_MODE false
#define batteryPin 27
#define capsLockPin 23
#define numLockPin 15
#define addr0 17
#define addr1 16
#define demuxChip 19
#define demuxBank 18
#define ROWS 12
#define COLS 8
int readPin[] = {36, 39, 34, 35, 22, 33, 25, 26};

byte key[ROWS][COLS];
byte skey[ROWS][COLS];

int writePins[2][3] = { //values for 31(4/5),32(A/B)
  {HIGH, LOW, LOW},
  {LOW, HIGH, LOW}
};
int address[2][4] = {{0, 0, 1, 1}, {0, 1, 0, 1}};

boolean capsLock = false;
boolean numLock = false;
boolean scrollLock = false;
boolean scrollFlash = true;
byte pinValue;
boolean startFlash = true;

unsigned long previousMillis = 0;
unsigned long mouseMillis = 0;
long interval = 500;

uint8_t ledStatusStored;
int mX = 0;
int mY = 0;
long mk_delay = 50;
long mk_interval = 50;
int mk_max_speed = 200;
int mk_time_to_max = 1500;
int mk_curve = 0;
int mX_time = 0;
int mY_time = 0;

void setup() {
  Serial.begin(115200);

  initGPIOPins();
  initKeyArray();

  bleKeyboard.begin();

  initializeConnection();
}

void initGPIOPins() {
  pinMode(capsLockPin, OUTPUT);
  pinMode(numLockPin, OUTPUT);
  pinMode(addr0, OUTPUT);
  pinMode(addr1, OUTPUT);
  pinMode(demuxChip, OUTPUT);
  pinMode(demuxBank, OUTPUT);
  pinMode(batteryPin, INPUT);
}

void loop() {
  if (!bleKeyboard.isConnected()) {
    initializeConnection();
  } else {
    readArray();
    checkTimedEvents();
  }
}

// Mouse mode uses the scroll lock key to use the numpad as a crude mouse
void checkTimedEvents() {
  checkLedStatus();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    checkBattery();

    //flash scroll lock for mouse mode
    if (scrollLock) {
      digitalWrite(numLockPin, scrollFlash);
      scrollFlash = !scrollFlash;
      previousMillis = currentMillis;
    }
  }
}

/* bit 0 - NUM LOCK   bit 1 - CAPS LOCK   bit 2 - SCROLL LOCK*/
void checkLedStatus() {
  uint8_t tmp = bleKeyboard.getLedStatus();
  if (tmp != ledStatusStored) {
    numLock = tmp & (1 << 0);
    if (!scrollLock) {
      digitalWrite(numLockPin, numLock);
    }

    capsLock = tmp & (1 << 1);
    digitalWrite(capsLockPin, capsLock);
    ledStatusStored = tmp;
  }
}

int getBatteryPct() {
  int sensorValue = analogRead(batteryPin);
  float voltage = (sensorValue * (3.3 / 4095.0) - 1.2) * 2; //Convert to true voltage
  int percentage = map(voltage , 3.27, 4.20 , 0 , 100);

//  Serial.print("sensorValue ");
//  Serial.println(sensorValue);
//  Serial.print("voltage ");
//  Serial.println(voltage);
//  Serial.print("percentage ");
//  Serial.println(percentage);

  if (voltage < 3.3) {
    return 0;
  } else if (voltage > 4.2) {
    return 100;
  }

  return percentage;
}

void checkBattery() {
  if (bleKeyboard.isConnected()) {
    bleKeyboard.setBatteryLevel(getBatteryPct());
  }
}

//flash the leds
void initializeConnection() {
  unsigned long currentMillis = millis();
  while (!bleKeyboard.isConnected()) {
    //    if (currentMillis - previousMillis > 100) {
    //      digitalWrite(numLockPin, scrollFlash);
    //      digitalWrite(capsLockPin, scrollFlash);
    //      scrollFlash = !scrollFlash;
    //      previousMillis = currentMillis;
    //    }
  }

  checkBattery();
  checkLedStatus();
}

void readArray() {
  int r = 0;
  for (int b = 0 ; b < 3; b++) {
    digitalWrite(demuxChip, writePins[0][b]);
    digitalWrite(demuxBank, writePins[1][b]);
    for (int a = 0 ; a < 4; a++) {
      digitalWrite(addr0, address[0][a]);
      digitalWrite(addr1, address[1][a]);

      for (int c = 0; c < COLS; c++) {
        pinMode(readPin[c], INPUT_PULLUP);
        pinValue = digitalRead(readPin[c]);
        pinMode(readPin[c], INPUT);

        r = (b * 4 + a);

        if (scrollLock && SCROLL_MAP[r][c] != 0) {
          //SUM MOUSE INPUT
          handleCursor(r, c);

          if (skey[r][c] != pinValue) {
            skey[r][c] = pinValue;
            handleClick(r, c);
          }
        } else if (key[r][c] != pinValue) {
          key[r][c] = pinValue;
          printInput(r, c, pinValue);

          //if CL, SL, or NL toggle?
          if (r == 0 && c == 5 ) { //scroll lock
            if (pinValue == LOW) {
              scrollLock = !scrollLock;
              if (!scrollLock) {
                digitalWrite(numLockPin, numLock);
              }
            }
          } else {
            //pressing numlock overrides mode mode
            if (r == 0 && c == 4) {
              scrollLock = false;
            }

            uint8_t key_code = KEY_CODE_MAP[r][c];

            if (pinValue == LOW) {
              bleKeyboard.press(key_code);
            } else if (pinValue == HIGH) {
              bleKeyboard.release(key_code);
            }
          }
        }
      }
    }
  }
  //combine all mouse inputs then move
  if (scrollLock) {
    moveCursor();
  }
}

void handleClick(int r, int c) {
  //LEFT CLICK
  if (r == 2 && c == 1) {
    if (pinValue == LOW) {
      bleKeyboard.click(MOUSE_LEFT);
    }
  }

  //RIGHT CLICK
  if (r == 2 && c == 2) {
    if (pinValue == LOW) {
      bleKeyboard.click(MOUSE_RIGHT);
    }
  }
}

void handleCursor(int r, int c) {
  if (pinValue == LOW) {
    if (r == 0 && c == 7)
      mY -= 1;
    if (r == 1 && c == 7)
      mY += 1;
    if (r == 1 && c == 2)
      mX -= 1;
    if (r == 1 && c == 4)
      mX += 1;
    if (r == 0 && c == 6) {
      mX -= 1;
      mY -= 1;
    }
    if (r == 1 && c == 0) {
      mX += 1;
      mY -= 1;
    }
    if (r == 1 && c == 6) {
      mX -= 1;
      mY += 1;
    }
    if (r == 2 && c == 0) {
      mX += 1;
      mY += 1;
    }
  }
}

void moveCursor() {
  unsigned long currentMillis = millis();
  if (currentMillis - mouseMillis > (mk_interval / (isShift() ? 3 : 1))) {
    bleKeyboard.move(mX * 10, mY * 10);
    mouseMillis = currentMillis;
  }

  mX = 0;
  mY = 0;
}

boolean isShift() {
  if (key[5][2] == LOW || key[6][5] == LOW) {
    return true;
  }
  return false;
}

boolean isCtrl() {
  if (key[11][4] == LOW)
    return true;
  return false;
}

void initKeyArray() {
  for (int r = 0; r < ROWS ; r++) {
    for (int c = 0; c < COLS; c++) {
      key[r][c] = HIGH;
      skey[r][c] = HIGH;
    }
  }
}

void printArray() {
  for (int r = 0; r < ROWS ; r++) {
    for (int c = 0; c < COLS; c++) {
      Serial.print(key[r][c]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.println("x");
}

void printInput(int r, int c, int val) {
#if TEST_MODE
  Serial.print("key R:");
  Serial.print(r);
  Serial.print(" C:");
  Serial.print(c);
  Serial.print(" Value ");
  Serial.println(val);
#endif
}
