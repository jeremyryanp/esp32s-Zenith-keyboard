#define KEY_PRT_SC 0xCE

#define KP_DIV '\334' //220 Keypad /
#define KP_MUL '\335' //221 Keypad *
#define KP_MINUS '\336' //222 Keypad -
#define KP_PLUS '\337' //223 Keypad +
#define KP_ENTER '\340' //224 Keypad ENTER
#define KP_1 '\341' //225 Keypad 1 and End
#define KP_2 '\342' //226 Keypad 2 and Down Arrow
#define KP_3 '\343' //227 Keypad 3 and PageDn
#define KP_4 '\344' //228 Keypad 4 and Left Arrow
#define KP_5 '\345' //229 Keypad 5
#define KP_6 '\346' //230 Keypad 6 and Right Arrow
#define KP_7 '\347' //231 Keypad 7 and Home
#define KP_8 '\350' //232 Keypad 8 and Up Arrow
#define KP_9 '\351' //233 Keypad 9 and PageUp
#define KP_0 '\352' //234 Keypad 0 and Insert
#define KP_RADIX '\353' //235 Keypad . and Delete

#define KEY_BACKSLASH '\134'
#define KEY_NUMLOCK 136 + 83
#define KEY_SCROLLLOCK 0x47
#define BLNK 0x00

const uint8_t KEY_CODE_MAP[12][8] = {
  {KEY_F7, KEY_F8,  KEY_F9,  KEY_F10, KEY_NUMLOCK, KEY_SCROLLLOCK, KP_7, KP_8},
  {KP_9, KP_MINUS, KP_4, KP_5, KP_6, KP_PLUS, KP_1, KP_2},
  {KP_3, KP_0, KP_RADIX, KP_ENTER, BLNK, BLNK, BLNK, BLNK},
  {BLNK, BLNK, BLNK, BLNK, BLNK, BLNK, BLNK, BLNK},
  {'f', 'g', 'h', 'j', 'k', 'l', ';', '\''},//the last one is ' but idk if this is correct
  {'`', KEY_LEFT_SHIFT , KEY_BACKSLASH, 'z', 'x', 'c', 'v', 'b'}, //134 should be
  {'n', 'm', ',', '.', '/', KEY_RIGHT_SHIFT, KEY_PRT_SC , KEY_LEFT_ALT},
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
