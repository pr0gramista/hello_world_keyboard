#define KEY_LAYER_DOWN 50920
#define KEY_MOUSE_MODE 50921
#define EMPTY 50922

int ROWS = 5;
int COLUMNS = 13;
int KEYS = 65;

int matrix[] = {
  KEY_ESC, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, KEY_BACKSPACE, KEY_BACKSPACE,
  39, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93,
  KEY_TAB, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, KEY_RETURN, KEY_RETURN,
  EMPTY, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, KEY_UP_ARROW, KEY_RIGHT_SHIFT,
  KEY_MOUSE_MODE, KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_LAYER_DOWN, KEY_SPACE, KEY_SPACE, KEY_LEFT_SHIFT, KEY_RIGHT_ALT, EMPTY, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW
};

int down[] = {
  KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_DELETE, KEY_DELETE,
  EMPTY, KEY_F11, KEY_F12, 101, 114, 116, 121, 117, 105, KEY_HOME, KEY_PAGE_UP, KEY_INSERT, 93,
  KEY_TAB, 97, 115, 100, 102, 103, 104, 106, 107, KEY_END, KEY_PAGE_DOWN, KEY_RETURN, KEY_RETURN,
  KEY_LEFT_SHIFT, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, KEY_UP_ARROW, KEY_RIGHT_SHIFT,
  KEY_MOUSE_MODE, KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_LAYER_DOWN, KEY_SPACE, KEY_SPACE, KEY_LEFT_SHIFT, KEY_RIGHT_ALT, EMPTY, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW
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
float sensitivity = 4;
float wheel = 0;

int old[65];

// pins in order from keyboard column 1 to 13
int rowsMapping[] = {19, 18, 17, 16, 15};
int columnMapping[] = {12, 11, 9, 10, 8, 7, 6, 5, 4, 3, 2, 1, 14};

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World");

  for (int i = 0; i < ROWS; i++) {
    pinMode(rowsMapping[i], INPUT_PULLUP);
  }

  // Set pinmode for columns
  for (int i = 0; i < COLUMNS; i++) {
    pinMode(columnMapping[i], OUTPUT);
    digitalWrite(columnMapping[i], HIGH);
  }
  Keyboard.begin();
}

void scan() {
  for (int i = 0; i < COLUMNS; i++) {
    selectColumn(i);
    delayMicroseconds(50);
    for (int j = 0; j < ROWS; j++) {
      int result = digitalRead(rowsMapping[j]);

      int m = j * COLUMNS + i;
      if (result == LOW && old[m] == HIGH) {
        old[m] = LOW;
        press(m);
      } else if (result == HIGH && old[m] == LOW) {
        old[m] = HIGH;
        release(m);
      }
    }
    unselectColumn(i);
  }
}

void loop() {
  scan();

  if (mouse_mode) {
   mouse(); 
  }
  delay(1);
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
  
  if (key == KEY_BACKSPACE) {
    if (!backspace_pressed) {
      backspace_pressed = true;
      Keyboard.press(key); 
    }
  } else if (key == KEY_SPACE) {
  } else if (key == KEY_RETURN) {
    if (!return_pressed) {
      return_pressed = true;
      Keyboard.press(key); 
    }
  } else if (key == KEY_LAYER_DOWN) {
    layer_down = true;
  } else if (key == KEY_MOUSE_MODE) {
    Mouse.begin();
    mouse_mode = true;
    move_x = 0;
    move_y = 0;
    wheel = 0;
  } else if (key == KEY_UP_ARROW && mouse_mode) {
     move_y += 1;
  } else if (key == KEY_DOWN_ARROW && mouse_mode) {
     move_y -= 1;
  } else if (key == KEY_RIGHT_ARROW && mouse_mode) {
     move_x += 1;
  } else if (key == KEY_LEFT_ARROW && mouse_mode) {
     move_x -= 1;
  } else if (key == 46 && mouse_mode) {
     wheel += 1;
  } else if (key == EMPTY && mouse_mode) {
     wheel -= 1;
  } else if (key == 47 && mouse_mode) {
    Mouse.press(MOUSE_LEFT);
  } else if (key == KEY_RIGHT_SHIFT && mouse_mode) {
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
  
  if (key == KEY_BACKSPACE) {
    if (backspace_pressed) {
      backspace_pressed = false;
      Keyboard.release(key); 
    }
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
    layer_down = false;
  } else if (key == KEY_MOUSE_MODE) {
    Mouse.end();
    mouse_mode = false;
  } else if (key == KEY_UP_ARROW && mouse_mode) {
     move_y -= 1;
  } else if (key == KEY_DOWN_ARROW && mouse_mode) {
     move_y += 1;
  } else if (key == KEY_RIGHT_ARROW && mouse_mode) {
     move_x -= 1;
  } else if (key == KEY_LEFT_ARROW && mouse_mode) {
     move_x += 1;
  } else if (key == 46 && mouse_mode) {
     wheel -= 1;
  } else if (key == EMPTY && mouse_mode) {
     wheel += 1;
  } else if (key == 47 && mouse_mode) {
    Mouse.release(MOUSE_LEFT);
  } else if (key == KEY_RIGHT_SHIFT && mouse_mode) {
    Mouse.release(MOUSE_RIGHT);
  } else {
    Keyboard.release(key);
  }
}

/* row: 0 to 12 */
void selectColumn(int column) {
  digitalWrite(columnMapping[column], LOW);
}

void unselectColumn(int column) {
  digitalWrite(columnMapping[column], HIGH);
}



