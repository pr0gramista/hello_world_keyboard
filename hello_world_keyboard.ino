int ROWS = 5;
int COLUMNS = 13;
int KEYS = 65;

int matrix[] = {
  KEY_ESC, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, KEY_BACKSPACE, KEY_BACKSPACE,
  KEY_ESC, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93,
  KEY_TAB, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, KEY_RETURN, KEY_RETURN,
  KEY_LEFT_SHIFT, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, KEY_RIGHT_SHIFT, KEY_RIGHT_SHIFT,
  96, 96,KEY_LEFT_ALT, KEY_LEFT_CTRL, KEY_F12, KEY_SPACE, KEY_SPACE, KEY_F12, KEY_RIGHT_ALT, KEY_LEFT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW
};

boolean backspace_pressed = false;
boolean space_pressed = false;
boolean return_pressed = false;

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
        press(matrix[m]);
      } else if (result == HIGH && old[m] == LOW) {
        old[m] = HIGH;
        release(matrix[m]);
      }
    }
    unselectColumn(i);
  }
}

void loop() {
  scan();
  delay(1);
}

void press(int key) {
  if (key == KEY_BACKSPACE) {
    if (!backspace_pressed) {
      backspace_pressed = true;
      Keyboard.press(key); 
    }
  } else if (key == KEY_SPACE) {
    if (!space_pressed) {
      space_pressed = true;
      Keyboard.press(key);
    }
  } else if (key == KEY_RETURN) {
    if (!return_pressed) {
      return_pressed = true;
      Keyboard.press(key); 
    }
  } else {
    Keyboard.press(key);
  }
}

void release(int key) {
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



