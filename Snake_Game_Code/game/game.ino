// =============================== INCLUDE ===============================
#include "JoystickControl.h"
#include <LedControl.h>
// =============================== INCLUDE ===============================

// =============================== DEFINE ===============================
#define PAUSE 0
#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4
#define STABLE 5 

#define ROWS 8
#define COLUMNS 8

const byte apple = 1;
const byte snake = 2;
const byte none = 0;
// =============================== DEFINE ===============================

// =============================== CONST ===============================
const int xaxis = A0;
const int yaxis = A1;
const int sw = 2;

const long appleInterval = 8000;
const long moveInterval = 1;
// =============================== CONST ===============================

// =============================== GLOBAL ===============================

volatile bool isPaused = false;

int userX = 0, userY = 0;
int length = 1;
byte userDirection = STABLE;

LedControl lc = LedControl(11, 13, 10, 1);
// =============================== GLOBAL ===============================

// =============================== DECLARATOIN ===============================
int get_Direction();
void print_location(int, int);
bool isPoint(int, int);
bool go_down();
bool go_up();
bool go_left();
bool go_right();
void switchPoints(int, int, int, int);
void set_apple();
void printGameOver();


// =============================== DECLARATOIN ===============================


typedef struct leds *led;

struct leds {
  byte state;
  byte previousLoc;
}; 

leds ledState[ROWS][COLUMNS];

// Switch interrupt
void handleSwitch() {
  // ========== For handle bouncing ==========
  delay(10);
  static bool toggle = false;
  // =========================================
  if(!digitalRead(sw) && !toggle) {
    isPaused = !isPaused;
  }
  toggle = !toggle;
}

// void makeEmptyLeds() {
//   led l = (led)malloc(sizeof(struct node));
//   l->next = null;
//   l->previous = null;
//   l->state = true;
//   l->
// }

void setLedStates() {
  for (int i = 0; i < ROWS; i++){
    for (int j = 0; j < COLUMNS; j++){
      ledState[i][j].state = none;
      ledState[i][j].previousLoc = 0;
    }
  }
}

void printLedStates() {
  Serial.println("=================================");
  for (int i = 0; i < ROWS; i++){
    for (int j = 0; j < COLUMNS; j++){ // nextStep
        Serial.print(ledState[i][j].previousLoc);
        Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("=================================\n");
}

void setup() {
  setupJoystick(A0, A1, sw);
  attachInterrupt(digitalPinToInterrupt(sw), handleSwitch, FALLING); // interrupt for handling switch pressing ;

  lc.shutdown(0, false);  // Turn on the MAX7219
  lc.setIntensity(0, 8);  // Set brightness (0 to 15)
  lc.clearDisplay(0);     // Clear the display


  setLedStates();

    // ledState[2][0].state = true;
    // ledState[4][0].state = true;
    // ledState[5][5].state = true;
    // lc.setLed(0, 5, 0, true);
    // lc.setLed(0, 3, 0, true);
    // lc.setLed(0, 5, 5, true);

    // ledState[0][2].state = true;
    // ledState[0][4].state = true;

    // lc.setLed(0, 0, 2, true);
    // lc.setLed(0, 0, 4, true);

    lc.setLed(0, userY, userX, true);
    ledState[userY][userX].state = snake;

}

void restart() {
  clearMatrix();
  setLedStates();
  userX = userY = 0;
  lc.setLed(0, userY, userX, true);
  ledState[userY][userX].state = snake;
}

void loop() {

  static unsigned long previousMillisApple = 0;
  static unsigned long previousMillisMove = 0;

  unsigned long currentMillis = millis();  


  if (currentMillis - previousMillisApple >= appleInterval) {
    previousMillisApple = currentMillis;
    set_apple();
  }

  if (currentMillis - previousMillisMove >= moveInterval) {
    previousMillisMove = currentMillis;
    
    int joystickDirection = get_Direction();
    static int lastDirection = joystickDirection;

    if (joystickDirection == STABLE) {
      joystickDirection = lastDirection;

    }

    bool lost = false;

    if(joystickDirection == RIGHT) {
      lastDirection = RIGHT;
      lost = go_right();
    }

    else if (joystickDirection == LEFT) {
      lastDirection = LEFT;
      lost = go_left();
    }

    else if (joystickDirection == UP) {
      lastDirection = UP;
      lost = go_up();
    }

    else if (joystickDirection == DOWN) {
      lastDirection = DOWN;
      lost = go_down();
    }

    Serial.print("DIRECTION:   ");
    Serial.println(joystickDirection);

    if (lost) {
      printGameOver();
      restart();
      joystickDirection = lastDirection = STABLE;
    }
  }
}

void set_apple() {
  static int row = 1, column = 0;

  while (1) {
    row = random(0, 8);
    column = random(0, 8);
    if (!ledState[row][column].state)
      break;
  }
  lc.setLed(0, row, column, true);
  ledState[row][column].state = apple;
}

void switchPoints(int userX0, int userY0, int userX1, int userY1) {
  lc.setLed(0, userY0, userX0, false);
  ledState[userY0][userX0].state = none;
  
  lc.setLed(0, userY1, userX1, true);
  ledState[userY1][userX1].state = snake;

  ledState[userY1][userX1].previousLoc = ledState[userY0][userX0].previousLoc;

}

bool go_right() {

  if (userX == 7) {
    return true;
  }

  byte state = getState(userY, userX + 1);

  if (!state) {
    switchPoints(userX, userY, userX + 1, userY);
    int tempY = userY, tempX = userX;
    userX++;
    if (ledState[userY][userX].previousLoc) {
      ledState[userY][userX].previousLoc = LEFT;
    }

    for(int i= 1; i < length; i++) {

      if (!ledState[tempY][tempX].previousLoc) {
        ledState[tempY][tempX].state = none;
      }

      if (ledState[tempY][tempX].previousLoc == RIGHT) {
        switchPoints(tempX + 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = RIGHT;
        }
        tempX++;
      }

      else if (ledState[tempY][tempX].previousLoc == LEFT) {
        switchPoints(tempX - 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = LEFT;
        }
        tempX--;
      }

      else if (ledState[tempY][tempX].previousLoc == UP) {
        switchPoints(tempX, tempY - 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = UP;
        }
        tempY--;
      }

      else if(ledState[tempY][tempX].previousLoc == DOWN) {
        switchPoints(tempX, tempY + 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = DOWN;
        }
        tempY++;
      }
    }

    }
  else if (state == apple) {
    Serial.print("\nfwefegegvewgewbgewgew");
    userX++;
    ledState[userY][userX - 1].state = snake;
    ledState[userY][userX].state = snake;
    ledState[userY][userX].previousLoc = LEFT;

    length++;
  }

  else {
    return true;
  }

  print_location(userY, userX);
  printLedStates();
  return false;
}

bool go_left() {

  if (userX == 0) {
    return true;
  }

  byte state = getState(userY, userX - 1);

  if (!state) {
    switchPoints(userX, userY, userX - 1, userY);
    int tempY = userY, tempX = userX;
    userX--;
    if (ledState[userY][userX].previousLoc) {
      ledState[userY][userX].previousLoc = RIGHT;
    }

    for(int i= 1; i < length; i++) {

      if (ledState[tempY][tempX].previousLoc == RIGHT) {
        switchPoints(tempX + 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = RIGHT;
        }
        tempX++;
      }

      else if (ledState[tempY][tempX].previousLoc == LEFT) {
        switchPoints(tempX - 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = LEFT;
        }
        tempX--;
      }

      else if (ledState[tempY][tempX].previousLoc == UP) {
        switchPoints(tempX, tempY - 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = UP;
        }
        tempY--;
      }

      else if(ledState[tempY][tempX].previousLoc == DOWN) {
        switchPoints(tempX, tempY + 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = DOWN;
        }
        tempY++;
      }
    }

    }
  else if (state == apple) {
    Serial.print("\nfwefegegvewgewbgewgew");
    userX--;
    ledState[userY][userX + 1].state = snake;
    ledState[userY][userX].state = snake;
    ledState[userY][userX].previousLoc = RIGHT;

    length++;
  }

  else {
    return true;
  }

  print_location(userY, userX);
  printLedStates();
  return false;
}

bool go_up() {

  if (userY == 0) {
    return true;
  }

  byte state = getState(userY - 1, userX);

  if (!state) {
    switchPoints(userX, userY, userX, userY - 1);
    int tempY = userY, tempX = userX;
    userY--;
    if (ledState[userY][userX].previousLoc) {
      ledState[userY][userX].previousLoc = DOWN;
    }

    for(int i= 1; i < length; i++) {
      if (ledState[tempY][tempX].previousLoc == LEFT) {
        switchPoints(tempX - 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = LEFT;
        }
        tempX--;
      }

      else if (ledState[tempY][tempX].previousLoc == RIGHT) {
        switchPoints(tempX + 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = RIGHT;
        }
        tempX++;
      }

      else if (ledState[tempY][tempX].previousLoc == UP) {
        switchPoints(tempX, tempY - 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = UP;
        }
        tempY--;
      }

      else if(ledState[tempY][tempX].previousLoc == DOWN) {
        switchPoints(tempX, tempY + 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = DOWN;
        }
        tempY++;
      }
    }

    }
  else if (state == apple){
    Serial.print("\nfwefegegvewgewbgewgew");
    userY--;
    ledState[userY + 1][userX].state = snake;
    ledState[userY][userX].state = snake;
    ledState[userY][userX].previousLoc = DOWN;

    length++;
  }

  else {
    return true;
  }

  print_location(userY, userX);
  printLedStates();
  return false;
}

bool go_down() {
  if (userY == 7) {
    return true;
  }

  byte state = getState(userY + 1, userX);

  if (!state) {
    switchPoints(userX, userY, userX, userY + 1);
    int tempY = userY, tempX = userX;
    userY++;
    if (ledState[userY][userX].previousLoc) {
      ledState[userY][userX].previousLoc = UP;
    }

    for(int i= 1; i < length; i++) {
      if (ledState[tempY][tempX].previousLoc == LEFT) {
        switchPoints(tempX - 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = LEFT;
        }
        tempX--;
      }

      else if (ledState[tempY][tempX].previousLoc == RIGHT) {
        switchPoints(tempX + 1, tempY, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = RIGHT;
        }
        tempX++;
      }

      else if (ledState[tempY][tempX].previousLoc == UP) {
        switchPoints(tempX, tempY - 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = UP;
        }
        tempY--;
      }

      else if(ledState[tempY][tempX].previousLoc == DOWN) {
        switchPoints(tempX, tempY + 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = DOWN;
        }
        tempY++;
      }
    }

  }

  else if (state == apple){
    Serial.print("\nfwefegegvewgewbgewgew");
    userY++;
    ledState[userY - 1][userX].state = snake;
    ledState[userY][userX].state = snake;
    ledState[userY][userX].previousLoc = UP;

    length++;
  }

  else {
    return true;
  }

  print_location(userY, userX);
  printLedStates();
  return false;
}

byte getState(int userY, int userX) {
  return ledState[userY][userX].state;
}

void printGameOver() {
  // Clear the LED matrix
  clearMatrix();

  // Define a simple "Game Over" pattern (each row is an 8-bit representation)
  byte G[8] = {
    B11111100,  // Row 1
    B10000000,  // Row 2
    B10000000,  // Row 3
    B10011110,  // Row 4
    B10000010,  // Row 5
    B10000010,  // Row 6
    B10000010,  // Row 7
    B11111100   // Row 8
  };

  byte A[8] = {
    B01111100,  // Row 1
    B10000010,  // Row 2
    B10000010,  // Row 3
    B11111110,  // Row 4
    B10000010,  // Row 5
    B10000010,  // Row 6
    B10000010,  // Row 7
    B10000010   // Row 8
  };

  byte M[8] = {
    B10000010,  // Row 1
    B11000110,  // Row 2
    B10101010,  // Row 3
    B10010010,  // Row 4
    B10000010,  // Row 5
    B10000010,  // Row 6
    B10000010,  // Row 7
    B10000010   // Row 8
  };

  byte E[8] = {
    B11111100,  // Row 1
    B10000000,  // Row 2
    B10000000,  // Row 3
    B11111100,  // Row 4
    B10000000,  // Row 5
    B10000000,  // Row 6
    B10000000,  // Row 7
    B11111100   // Row 8
  };

  byte O[8] = {
    B01111100,  // Row 1
    B10000010,  // Row 2
    B10000010,  // Row 3
    B10000010,  // Row 4
    B10000010,  // Row 5
    B10000010,  // Row 6
    B10000010,  // Row 7
    B01111100   // Row 8
  };

  byte V[8] = {
    B10000010,  // Row 1
    B10000010,  // Row 2
    B10000010,  // Row 3
    B10000010,  // Row 4
    B10000010,  // Row 5
    B10000010,  // Row 6
    B01000100,  // Row 7
    B00101000   // Row 8
  };

  byte R[8] = {
    B11111100,  // Row 1
    B10000010,  // Row 2
    B10000010,  // Row 3
    B11111100,  // Row 4
    B10010000,  // Row 5
    B10001000,  // Row 6
    B10000100,  // Row 7
    B10000010   // Row 8
  };

  clearMatrix();

  // Display "GAME OVER" letter by letter
  displayLetter(G, 0);  // 'G'
  delay(500);  // Wait for half a second
  displayLetter(A, 1);  // 'A'
  delay(500);
  displayLetter(M, 2);  // 'M'
  delay(500);
  displayLetter(E, 3);  // 'E'
  delay(500);
  displayLetter(O, 4);  // 'O'
  delay(500);
  displayLetter(V, 5);  // 'V'
  delay(500);
  displayLetter(E, 6);  // 'E'
  delay(500);
  displayLetter(R, 7);  // 'R'
  delay(2000);  // Keep the message for a while
}

void displayLetter(byte letter[8], int matrixColumn) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, letter[i]);  // Display the letter
  }
}

void clearMatrix() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            lc.setLed(0, row, col, false);
        }
    }
}


void print_location(int userY, int userX) {
  Serial.print("( ");
  Serial.print(userY);
  Serial.print(", ");
  Serial.print(userX);
  Serial.println(" )");
}

// function that return the current joystick state
int get_Direction() {
    int XY[2]; // array holding X, Y axis
    get_xy(XY);
    int x = XY[0], y = XY[1];

    if (isPaused){
      // Serial.println("PAUSE");
      return PAUSE;
    }

    else if (abs(x - 180) > move_range || abs(y - 180) > move_range) {
      if (abs(x - 180) > abs(y - 180)) {
        if (x > 180) {
          // Serial.println("LEFT");
          return LEFT;
        }
        else {
          // Serial.println("RIGHT");
          return RIGHT;
        }
      }
      else {
        if (y > 180) {
          // Serial.println("UP");
          return UP;
        }
        else {
          // Serial.println("DOWN");
          return DOWN;
        }
      }
    }
    else {
      return STABLE;
    }
}
