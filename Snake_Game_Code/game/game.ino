#include "JoystickControl.h"
#include <LedControl.h>

#define PAUSE 0
#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4
#define STABLE 5 

#define ROWS 8
#define COLUMNS 8

const int xaxis = A0;
const int yaxis = A1;
const int sw = 2;

const long appleInterval = 1500;
const long moveInterval = 300;


volatile bool isPaused = false;

int userX = 0, userY = 0;
int length = 1;
int userDirection = STABLE;

LedControl lc = LedControl(11, 13, 10, 1);

struct leds {
  bool state;
  byte nextStep;
}; leds

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

void setLedStates() {
  for (int i = 0; i < ROWS; i++){
    for (int j = 0; j < COLUMNS; j++){
      ledState[i][j] = false;
    }
  }
}

void printLedStates() {
  for (int i = 0; i < ROWS; i++){
    for (int j = 0; j < COLUMNS; j++){
        Serial.print(ledState[i][j]);
        Serial.print(" ");
    }
    Serial.println("");
  }
}

void setup() {
  setupJoystick(A0, A1, sw);
  attachInterrupt(digitalPinToInterrupt(sw), handleSwitch, FALLING); // interrupt for handling switch pressing ;

    lc.shutdown(0, false);  // Turn on the MAX7219
    lc.setIntensity(0, 8);  // Set brightness (0 to 15)
    lc.clearDisplay(0);     // Clear the display


    setLedStates();

    ledState[0][4] = true;
    ledState[0][1] = true;
    lc.setLed(0, 0, 4, true);
    lc.setLed(0, 0, 1, true);

    lc.setLed(0, userY, userX, true);

    ledState[userY][userX] = true;

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
    if (!ledState[row][column])
      break;
  }
  lc.setLed(0, row, column, true);
}

void switchPoints(int userX0, int userY0, int userX1, int userY1) {
  lc.setLed(0, userY0, userX0, false);
  ledState[userY0][userX0] = false;
  
  ledState[userY1][userX1] = true;
  lc.setLed(0, userY1, userX1, true);
}

void go_right() {
  if (userDirection == LEFT) {
    return;
  }

  if (!isPoint(userY, userX + 1)) {
    if(userDirection == RIGHT) {
      for(int i= 0; i < length; i++) {
        switchPoints(userX - i, userY, userX - i + 1, userY);
      }
    }
  }
  else {
    length++;
  }
  userX++;
  userDirection = RIGHT;
}

void go_left() {
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
  userDirection = LEFT;
  // print_location(userY, userX);
}

void go_up() {

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
  userDirection = UP;
}

void go_down() {

  if (userDirection == UP) {
    return;
  }
  

  if (!isPoint(userY + 1, userX)) {
    if (userDirection == DOWN) {
      for(int i=0; i < length ; i++) {
        switchPoints(userX, userY - i, userX, userY - i + 1);
      }
    }
  }

  else {
    length++;
  }
  userY++;
  userDirection = DOWN;
}

bool isPoint(int userY, int userX) {
  return ledState[userY][userX];
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
