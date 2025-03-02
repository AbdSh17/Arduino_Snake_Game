#include "JoystickControl.h"

const int xaxis = A0;
const int yaxis = A1;
const int sw = 2;

volatile bool isPaused = false;

#define PAUSE 0
#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4


void handleSwitch() {
  isPaused = !isPaused;
}

void setup() {
  setupJoystick(A0, A1, sw);
  attachInterrupt(digitalPinToInterrupt(sw), handleSwitch, FALLING);
}

void loop() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();  

  long interval = 500;  

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int direction = get_Direction();
    
  }
}

int get_Direction() {
    int XY[2];
    get_xy(XY);
    int x = XY[0], y = XY[1];

    if (isPaused){
      Serial.println("PAUSE");
      return PAUSE;
    }

    else if (abs(x - 180) > move_range || abs(y - 180) > move_range) {
      if (abs(x - 180) > abs(y - 180)) {
        if (x > 180) {
          Serial.println("LEFT");
          return LEFT;
        }
        else {
          Serial.println("RIGHT");
          return RIGHT;
        }
      }
      else {
        if (y > 180) {
          Serial.println("UP");
          return UP;
        }
        else {
          Serial.println("DOWN");
          return DOWN;
        }
      }
    }
}
