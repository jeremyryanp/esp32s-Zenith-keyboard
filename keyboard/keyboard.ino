#include <BleMKeyboard.h>

BleMKeyboard bleKeyboard("Zenith Keyboard", "Penntech customs", 100);

#define TEST_MODE false
#define capsLockPin 23
#define numLockPin 15
#define addr0 17
#define addr1 16
#define demuxChip 19
#define demuxBank 18
#define ROWS 12
#define COLS 8
int readPin[] = {36, 39, 34, 35, 32, 33, 25, 26};

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
boolean initLocks = true;
byte pinValue;

unsigned long previousMillis = 0;
unsigned long mouseMillis = 0;
long interval = 500;
long mouseInterval = 50;
int mouseFactor = 10;

const uint8_t KEY_CODE_MAP[12][8] = {
  { KEY_F7,  KEY_F8,  KEY_F9,  KEY_F10, 136 + 83, 0x47, '\347', '\350'},
  {'\351', '\336', '\344', '\345', '\346', '\337', '\341', '\342'},
  {'\343', '\340', '\353', '\340', 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {'f', 'g', 'h', 'j', 'k', 'l', ';', '\''},//the last one is ' but idk if this is correct
  {'`', KEY_LEFT_SHIFT , '\134', 'z', 'x', 'c', 'v', 'b'}, //134 should be
  {'n', 'm', '<', '>', '?', KEY_RIGHT_SHIFT, 0x46, KEY_LEFT_ALT},
  {' ' , KEY_CAPS_LOCK , KEY_F1 , KEY_F2,  KEY_F3 , KEY_F4 , KEY_F5, KEY_F6},
  {KEY_ESC, '1', '2', '3', '4', '5', '6', '7'},
  {'8', '9', '0', '-', '=', KEY_BACKSPACE, KEY_TAB, 'q'},
  {'w', 'e', 'r', 't', 'y', 'u', 'i', 'o'},
  {'p', '{', '}', KEY_RETURN, KEY_LEFT_CTRL, 'a', 's', 'd'}
};
const int SCROLL_MAP[12][8] = {
  {0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

void setup() {
  Serial.begin(115200);

  pinMode(capsLockPin, OUTPUT);
  pinMode(numLockPin, OUTPUT);
  pinMode(addr0, OUTPUT);
  pinMode(addr1, OUTPUT);
  pinMode(demuxChip, OUTPUT);
  pinMode(demuxBank, OUTPUT);

  for (int i = 0; i < sizeof(readPin) / sizeof(readPin[0]); i++) {
    //    pinMode(readPin[i], INPUT);
    pinMode(readPin[i], INPUT_PULLUP);
  }

  initArray();

  bleKeyboard.begin();

}

void loop() {
  if (bleKeyboard.isConnected()) {
    if (initLocks)
      initializeConnection();

    readArray();
  } else {
    initLocks = true;
  }


  unsigned long currentMillis = millis();
  if (scrollLock) {
    if (currentMillis - previousMillis > interval) {
      digitalWrite(numLockPin, scrollFlash);
      scrollFlash = !scrollFlash;
      previousMillis = currentMillis;
    }
  }
}

void initializeConnection() {
  Serial.println("------------------------------");
  Serial.println("connected");
  Serial.println();
  Serial.println("------------------------------");

  initLocks = false;
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
          //MOVE MOUSE
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
                digitalWrite(numLockPin, LOW);
              }
            }
          } else {
            if (pinValue == LOW) {
              bleKeyboard.press(KEY_CODE_MAP[r][c]);
            } else if (pinValue == HIGH) {
              bleKeyboard.release(KEY_CODE_MAP[r][c]);
            }
          }
        }
      }
    }
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
      moveMouse(0, -1);
    if (r == 1 && c == 7)
      moveMouse(0, 1);
    if (r == 1 && c == 2)
      moveMouse(-1, 0);
    if (r == 1 && c == 4)
      moveMouse(1, 0);

    if (r == 0 && c == 6)
      moveMouse(-1, -1);
    if (r == 1 && c == 0)
      moveMouse(1, -1);
    if (r == 1 && c == 6)
      moveMouse(-1, 1);
    if (r == 2 && c == 0)
      moveMouse(1, 1);
  }
}

void moveMouse(int x, int y) {
  unsigned long currentMillis = millis();
  if (currentMillis - mouseMillis > (mouseInterval / (isShift() ? 3 : 1))) {
    bleKeyboard.move(x * mouseFactor, y * mouseFactor);
    mouseMillis = currentMillis;
  }
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

void initArray() {
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
