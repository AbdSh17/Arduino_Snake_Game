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
// =============================== DEFINE ===============================

// =============================== CONST ===============================
const int xaxis = A0;
const int yaxis = A1;
const int sw = 2;

const long appleInterval = 1500;
const long moveInterval = 300;
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
byte get_user_direction();
bool isPoint(int, int);
void go_down();
void go_up();
void go_left();
void go_right();
void switchPoints(int, int, int, int);
void set_apple();


// =============================== DECLARATOIN ===============================


typedef struct leds *led;

struct leds {
  led next;
  led previous;
  bool state;
  byte nextStep;
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
      ledState[i][j].state = false;
      ledState[i][j].nextStep = STABLE;
      ledState[i][j].next = nullptr;
      ledState[i][j].previous = nullptr;
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

    ledState[3][0].state = true;
    ledState[5][0].state = true;
    lc.setLed(0, 5, 0, true);
    lc.setLed(0, 3, 0, true);

    ledState[0][3].state = true;
    ledState[0][5].state = true;
    lc.setLed(0, 0, 3, true);
    lc.setLed(0, 0, 5, true);

    lc.setLed(0, userY, userX, true);
    ledState[userY][userX].state = true;

}

void loop() {

  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();   

  if (currentMillis - previousMillis >= moveInterval) {
    previousMillis = currentMillis;
    // set_apple();
    int analogDirection = get_Direction();

    if(analogDirection == RIGHT) {
      go_right();
    }

    else if (analogDirection == LEFT) {
      go_left();
    }

    else if (analogDirection == UP) {
      go_up();
    }

    else if (analogDirection == DOWN) {
      go_down();
    }
  }
}

void set_apple() {
  static int row = 1, column = 0;

  lc.setLed(0, row, column, false);

  static bool wait = true;
  wait = !wait;
  if (wait) {
    return;
  }

  while (1) {
    row = random(0, 8);
    column = random(0, 8);
    if (!ledState[row][column].state)
      break;
  }
  lc.setLed(0, row, column, true);
}

void switchPoints(int userX0, int userY0, int userX1, int userY1) {
  lc.setLed(0, userY0, userX0, false);
  ledState[userY0][userX0].state = false;
  
  lc.setLed(0, userY1, userX1, true);
  ledState[userY1][userX1].state = true;

  ledState[userY1][userX1].previousLoc = ledState[userY0][userX0].previousLoc;
  // ledState[userY0][userX0].previousLoc = 0;

  if (ledState[userY0][userX0].previous) {
    ledState[userY0][userX0].next = &ledState[userY1][userX1];
    ledState[userY1][userX1].previous = &ledState[userY0][userX0];
  }
  else {
    ledState[userY0][userX0].next = nullptr;
    ledState[userY1][userX1].previous = nullptr;
  }
}

/*
RIGHT: switchPoints(userX - i, userY, userX - i + 1, userY);
LEFT: switchPoints(userX + i, userY, userX + i - 1, userY);
UP: switchPoints(userX, userY + i, userX, userY + i - 1);
DOWN: switchPoints(userX, userY - i, userX, userY - i + 1);
*/


void go_right() {
  userDirection = get_user_direction();
  if (userDirection == STABLE) {
    userDirection = RIGHT;
  }
  if (userDirection == LEFT) {
    return;
  }

  if (!isPoint(userY, userX + 1)) {
    switchPoints(userX, userY, userX + 1, userY);
    int tempY = userY, tempX = userX;
    userX++;
    if (ledState[userY][userX].previousLoc) {
      ledState[userY][userX].previousLoc = LEFT;
    }

    for(int i= 1; i < length; i++) {
      if (ledState[tempY][tempX].previousLoc == LEFT) {
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
    }
      // ledState[userY][userX].previous->nextStep = RIGHT;
      if(ledState[userY][userX].previous) {
        Serial.println(ledState[userY][userX].previous->next->state);
      }
    }
  else {
    userX++;
    ledState[userY][userX - 1].state = true;
    ledState[userY][userX].state = true;
    ledState[userY][userX].previousLoc = LEFT;

    ledState[userY][userX].previous = &ledState[userY][userX - 1];
    ledState[userY][userX - 1].next = &ledState[userY][userX];

    length++;
  }

  ledState[userY][userX].nextStep = RIGHT;
  print_location(userY, userX);
  printLedStates();
}

void go_left() {
  userDirection = get_user_direction();
  if (userDirection == RIGHT) {
    return;
  }
  if (!isPoint(userY, userX - 1)) {
    if (userDirection == LEFT) {
      for(int i=0; i < length; i++) {
        switchPoints(userX + i, userY, userX + i - 1, userY);
      }
    }
    
  }
  else {
    length++;
  }
  userX--;
  ledState[userY][userX].nextStep = LEFT;
  // print_location(userY, userX);
}

void go_up() {
  userDirection = get_user_direction();
  if (userDirection == DOWN) {
    return;
  }
  
  if (!isPoint(userY - 1, userX)) {
    if (userDirection == UP){
      for(int i=0 ; i < length; i++) {
        switchPoints(userX, userY + i, userX, userY + i - 1);
      }
    }

  }

  else {
    length++;
  }
  userY--;
  ledState[userY][userX].nextStep = UP;
}

void go_down() {
  byte userDirection = get_user_direction();

  if (userDirection == STABLE) {
    userDirection = DOWN;
  }

  if (userDirection == UP) {
    return;
  }
  
  if (!isPoint(userY + 1, userX)) {
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

      else if (ledState[tempY][tempX].previousLoc == UP) {
        switchPoints(tempX, tempY - 1, tempX, tempY);
        if (ledState[tempY][tempX].previousLoc) {
          ledState[tempY][tempX].previousLoc = UP;
        }
        tempY--;
      }
    }

      if(ledState[userY][userX].previous) {
        Serial.println(ledState[userY][userX].previous->next->state);
      }
    }
  else {
    userY++;
    ledState[userY - 1][userX].state = true;
    ledState[userY][userX].state = true;
    ledState[userY][userX].previousLoc = UP;

    ledState[userY][userX].previous = &ledState[userY - 1][userX];
    ledState[userY - 1][userX].next = &ledState[userY][userX];

    length++;
  }
  ledState[userY][userX].nextStep = DOWN;
  print_location(userY, userX);
  printLedStates();
}

bool isPoint(int userY, int userX) {
  return ledState[userY][userX].state;
}

byte get_user_direction() {
  return userDirection = ledState[userY][userX].nextStep;
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
