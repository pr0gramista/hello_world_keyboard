#include <Rotary.h>

#define KEY_LAYER_DOWN 50920
#define KEY_MOUSE_MODE 50921
#define KEY_MACRO 50922
#define EMPTY 50922
#define PAD 50923
#define SPACE 32
#define BUTTON_CLEAN_SIZE 10
#define BUTTON_CLEAN_EXCEPTIONS_SIZE 5

#define DEBUG 0

int ROWS = 5;
int COLUMNS = 14;
int KEYS = 23;

int button_pressed[BUTTON_CLEAN_SIZE];
int button_clean_exceptions[BUTTON_CLEAN_EXCEPTIONS_SIZE] = {KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_LEFT_SHIFT, KEY_RIGHT_ALT}; // Remember to change BUTTON_CLEAN_EXCEPTIONS_SIZE

int matrix[] = {
  KEY_ESC, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, KEY_BACKSPACE,
  KEY_TAB, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 92,
  KEY_CAPS_LOCK, PAD, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, 39, KEY_RETURN,
  KEY_LEFT_SHIFT, PAD, PAD, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, KEY_RIGHT_SHIFT,
  KEY_LEFT_CTRL, PAD, PAD, KEY_LEFT_GUI, KEY_LEFT_ALT, SPACE, SPACE, SPACE, SPACE, SPACE, KEY_RIGHT_ALT, KEY_LAYER_DOWN, KEY_MACRO, KEY_MOUSE_MODE
};

int down[] = {
  96, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_BACKSPACE,
  126, KEY_HOME, KEY_UP_ARROW, KEY_END, 114, KEYPAD_7, KEYPAD_8, KEYPAD_9, KEY_NUM_LOCK, 111, KEY_PAUSE, KEY_SCROLL_LOCK, KEY_PRINTSCREEN, KEY_DELETE,
  KEY_CAPS_LOCK, PAD, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, 102, KEYPAD_4, KEYPAD_5, KEYPAD_6, KEY_INSERT, 108, 59, 39, KEY_RETURN,
  KEY_LEFT_SHIFT, PAD, PAD, 122, 120, 99, 118, KEYPAD_1, KEYPAD_2, KEYPAD_3, KEYPAD_0, 46, 47, KEY_RIGHT_SHIFT,
  KEY_LEFT_CTRL, PAD, PAD, KEY_LEFT_GUI, KEY_LEFT_ALT, SPACE, SPACE, SPACE, SPACE, SPACE, KEY_RIGHT_ALT, KEY_LAYER_DOWN, KEY_MACRO, KEY_MOUSE_MODE
};

boolean layer_down = false;

boolean backspace_pressed = false;
boolean space_pressed = false;
boolean return_pressed = false;

boolean mouse_mode = false;
unsigned long mouse_timer = 0;
const long mouse_interval = 10; 

float move_x = 0;
float move_y = 0;
float sensitivity = 2;
float wheel = 0;

// Joystick
int new_x = 0;
int old_x = 0;
int count_down_x = 5;

// Macro
boolean macro = false;

int old[75];

// pins in order from keyboard column 1 to 13
int rowsMapping[] = {19, 21, 22, 20, 23};
int columnMapping[] = {0, 1, 2, 3, 8, 5, 6, 7, 9, 15, 10, 14, 16, 17};

// Encoder
Rotary r = Rotary(11, 12);
boolean r_vertical = false;
boolean r_button = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World");

  // Keyboard setup
  for (int i = 0; i < COLUMNS; i++) {
    pinMode(columnMapping[i], INPUT_PULLUP);
  }
  
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowsMapping[i], OUTPUT);
    digitalWrite(rowsMapping[i], HIGH);
  }

  // Joystick setup
  Joystick.begin();
  pinMode(18, INPUT);
  pinMode(4, INPUT_PULLUP);
}

void scan() {
  for (int i = 0; i < ROWS; i++) {
    selectRow(i);
    delayMicroseconds(50);
    for (int j = 0; j < COLUMNS; j++) {
      int result = digitalRead(columnMapping[j]);

      int m = i * COLUMNS + j;
      if (result == LOW && old[m] == HIGH) {
        old[m] = LOW;
        #if DEBUG 
        Serial.print("Pressed ");
        Serial.println(m);
        #endif
        press(m);
      } else if (result == HIGH && old[m] == LOW) {
        old[m] = HIGH;
        #if DEBUG
        Serial.print("Released ");
        Serial.println(m);
        #endif
        release(m);
      }
    }
    unselectRow(i);
  }
}

/* 
 * Add button to release after it's unreachable
 */
void add_button_to_clean(int key) {
  // We want modifier keys to be untouchable by this mechanism
  for (int i = 0; i < BUTTON_CLEAN_EXCEPTIONS_SIZE; i++) {
    if (button_clean_exceptions[i] == key) {
      return;
    }
  }
  
  for (int i = 0; i < BUTTON_CLEAN_SIZE; i++) {
    if (button_pressed[i] == 0) {
      button_pressed[i] = key;
      break;
    }
  }
}

/* 
 * Remove button from being release after it's unreachable
 */
void remove_button_from_clean(int key) {
  for (int i = 0; i < BUTTON_CLEAN_SIZE; i++) {
    if (button_pressed[i] == key) {
      button_pressed[i] = 0;
      break;
    }
  }
}

/* 
 * Release buttons that were added to be released after
 * they are unreachable
 */
void clean_buttons() {
  for (int i = 0; i < BUTTON_CLEAN_SIZE; i++) {
    if (button_pressed[i] != 0) {
      Keyboard.release(button_pressed[i]);
      button_pressed[i] = 0;
    }
  }
}

void loop() {
  scan();

  if (mouse_mode) {
   mouse(); 
  }
  delay(1);

  int x_axis = 1024 - analogRead(18);
  if (x_axis == new_x) {
    count_down_x = count_down_x - 1;
    if (count_down_x < 0) {
      old_x = new_x;
    }
  } else {
    new_x = x_axis;
  }
  // Serial.println(x_axis);
  Joystick.X(old_x);
  sensitivity = old_x / 64;

  //Encoder
  //Button
  int state = digitalRead(4);
  if (state != r_button) {
    r_button = state;
    #if DEBUG
    Serial.println("Encoder button");
    #endif
    if (state == LOW) {
      r_vertical = true;
    } else {
      r_vertical = false;
    }
  }
  
  int result = r.process();
  if (result) {
    if (result == DIR_CW) {
      if (r_vertical == false) {
        Keyboard.press(KEY_RIGHT_ARROW);
        Keyboard.release(KEY_RIGHT_ARROW);
      }
      else {
        Keyboard.press(KEY_UP_ARROW);
        Keyboard.release(KEY_UP_ARROW);
      }
    } else {
      if (r_vertical == false) {
        Keyboard.press(KEY_LEFT_ARROW);
        Keyboard.release(KEY_LEFT_ARROW);
      }
      else {
        Keyboard.press(KEY_DOWN_ARROW);
        Keyboard.release(KEY_DOWN_ARROW);
      }
    }
    #if DEBUG
    Serial.println(result == DIR_CW ? "Right" : "Left");
    #endif
  }
}

void mouse () {
  unsigned long currentMillis = millis();

  if (currentMillis - mouse_timer >= mouse_interval) {
    mouse_timer = currentMillis;
    Mouse.move(move_x * sensitivity, move_y * sensitivity * -1, wheel);
  }
}

void press(int index) {
  int key = 0;
  if (layer_down == false) {
    key = matrix[index];
  } else {
    key = down[index];
  }

  if (macro) {
    if(index == 41) {
      Keyboard.press(MODIFIERKEY_RIGHT_CTRL);
      Keyboard.press(KEY_SCROLL_LOCK);
      delay(10);
      Keyboard.release(KEY_SCROLL_LOCK);
      Keyboard.release(MODIFIERKEY_RIGHT_CTRL);
    } else if(index == 27) {
      Keyboard.press(MODIFIERKEY_RIGHT_CTRL);
      Keyboard.press(MODIFIERKEY_RIGHT_SHIFT);
      Keyboard.press(KEY_SCROLL_LOCK);
      delay(10);
      Keyboard.release(KEY_SCROLL_LOCK);
      Keyboard.release(MODIFIERKEY_RIGHT_SHIFT);
      Keyboard.release(MODIFIERKEY_RIGHT_CTRL);
    } else {
      Serial.print("press,");
      Serial.println(index);
      return;
    }
  } else {
    add_button_to_clean(key);
  }
  
  if (key == KEY_BACKSPACE) {
    if (!backspace_pressed) {
      backspace_pressed = true;
      Keyboard.press(key); 
    }
  } else if (key == KEY_MACRO) {
    clean_buttons();
    macro = true;
  } else if (key == KEY_SPACE) {
  } else if (key == KEY_RETURN) {
    if (!return_pressed) {
      return_pressed = true;
      Keyboard.press(key); 
    }
  } else if (key == KEY_LAYER_DOWN) {
    clean_buttons();
    layer_down = true;
  } else if (key == KEY_MOUSE_MODE) {
    Mouse.begin();
    mouse_mode = true;
    move_x = 0;
    move_y = 0;
    wheel = 0;
  } else if (key == 119 && mouse_mode) {
     move_y += 1;
  } else if (key == 115 && mouse_mode) {
     move_y -= 1;
  } else if (key == 100 && mouse_mode) {
     move_x += 1;
  } else if (key == 97 && mouse_mode) {
     move_x -= 1;
  } else if (key == 51 && mouse_mode) {
     wheel += 1;
  } else if (key == 50 && mouse_mode) {
     wheel -= 1;
  } else if (key == 113 && mouse_mode) {
    Mouse.press(MOUSE_LEFT);
  } else if (key == 101 && mouse_mode) {
    Mouse.press(MOUSE_RIGHT);
  } else {
    Keyboard.press(key);
  }
}
  
void release(int index) {
  int key = 0;
  if (layer_down == false) {
    key = matrix[index];
  } else {
    key = down[index];
  }

  if (macro) {
    if (key == KEY_MACRO) {
      macro = false;
      return;
    } else if(key == 41 || key == 27) {
      return;
    } else {
      Serial.print("release,");
      Serial.println(index);
      return;
    }
  } else {
    remove_button_from_clean(key);
  }
  
  if (key == KEY_BACKSPACE) {
    if (backspace_pressed) {
      backspace_pressed = false;
      Keyboard.release(key); 
    }
  } else if (key == KEY_MACRO) {
    clean_buttons();
    macro = false;
  } else if (key == KEY_SPACE) {
    if (space_pressed) {
      space_pressed = false;
      Keyboard.release(key); 
    }
  } else if (key == KEY_RETURN) {
    if (return_pressed) {
      return_pressed = false;
      Keyboard.release(key); 
    }
  } else if (key == KEY_LAYER_DOWN) {
    clean_buttons();
    layer_down = false;
  } else if (key == KEY_MOUSE_MODE) {
    Mouse.end();
    mouse_mode = false;
  } else if (key == 119 && mouse_mode) {
     move_y -= 1;
  } else if (key == 115 && mouse_mode) {
     move_y += 1;
  } else if (key == 100 && mouse_mode) {
     move_x -= 1;
  } else if (key == 97 && mouse_mode) {
     move_x += 1;
  } else if (key == 51 && mouse_mode) {
     wheel -= 1;
  } else if (key == 50 && mouse_mode) {
     wheel += 1;
  } else if (key == 113 && mouse_mode) {
    Mouse.release(MOUSE_LEFT);
  } else if (key == 101 && mouse_mode) {
    Mouse.release(MOUSE_RIGHT);
  } else {
    Keyboard.release(key);
  }
}

void selectRow(int row) {
  digitalWrite(rowsMapping[row], LOW);
}

void unselectRow(int row) {
  digitalWrite(rowsMapping[row], HIGH);
}
