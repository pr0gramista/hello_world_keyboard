#define KEY_LAYER_DOWN 50920
#define KEY_MOUSE_MODE 50921
#define EMPTY 50922
#define PAD 50923
#define SPACE 32

int ROWS = 5;
int COLUMNS = 14;
int KEYS = 23;

int matrix[] = {
  KEY_ESC, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, KEY_BACKSPACE,
  KEY_TAB, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 92,
  KEY_CAPS_LOCK, PAD, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, 39, KEY_RETURN,
  KEY_LEFT_SHIFT, PAD, PAD, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, KEY_RIGHT_SHIFT,
  KEY_LEFT_CTRL, PAD, PAD, KEY_LEFT_GUI, KEY_LEFT_ALT, SPACE, SPACE, SPACE, SPACE, SPACE, KEY_RIGHT_ALT, KEY_LAYER_DOWN, EMPTY, KEY_MOUSE_MODE
};

int down[] = {
  96, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, KEY_BACKSPACE,
  126, KEY_HOME, KEY_UP_ARROW, KEY_END, 114, 116, 121, 117, 105, 111, 112, 91, 93, 92,
  KEY_CAPS_LOCK, PAD, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, 102, 103, 104, 106, 107, 108, 59, 39, KEY_RETURN,
  KEY_LEFT_SHIFT, PAD, PAD, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, KEY_RIGHT_SHIFT,
  KEY_LEFT_CTRL, PAD, PAD, KEY_LEFT_GUI, KEY_LEFT_ALT, SPACE, SPACE, SPACE, SPACE, SPACE, KEY_RIGHT_ALT, KEY_LAYER_DOWN, EMPTY, KEY_MOUSE_MODE
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

int old[75];

// pins in order from keyboard column 1 to 13
int rowsMapping[] = {19, 21, 22, 20, 23};
int columnMapping[] = {0, 1, 2, 3, 8, 5, 6, 7, 9, 15, 10, 14, 16, 17};

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World");

  for (int i = 0; i < COLUMNS; i++) {
    pinMode(columnMapping[i], INPUT_PULLUP);
  }
  
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowsMapping[i], OUTPUT);
    digitalWrite(rowsMapping[i], HIGH);
  }
}

void scan() {
  boolean hit = false;
  for (int i = 0; i < ROWS; i++) {
    selectRow(i);
    delayMicroseconds(50);
    for (int j = 0; j < COLUMNS; j++) {
      int result = digitalRead(columnMapping[j]);
      
      /*if (result == LOW) {
        Serial.print("Row ");
        Serial.print(i);
        Serial.print(" Pin ");
        Serial.println(columnMapping[j]);
        hit = true;
      }*/

      int m = i * COLUMNS + j;
      if (result == LOW && old[m] == HIGH) {
        old[m] = LOW;
        Serial.print("Pressed ");
        Serial.println(m);
        press(m);
      } else if (result == HIGH && old[m] == LOW) {
        old[m] = HIGH;
        Serial.print("Released ");
        Serial.println(m);
        release(m);
      }
    }
    unselectRow(i);
  }
  if (hit == false) {
    //Serial.println("No hit");
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
  Serial.println(key);
  
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
  } else if (key == 119 && mouse_mode) {
     move_y += 1;
     Serial.println("no kurwea");
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
  Serial.println(key);
  
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
