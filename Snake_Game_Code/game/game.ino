#include "JoystickControl.h"

const int xaxis = A0;
const int yaxis = A1;
const int sw = 2;    

void setup() {
  setupJoystick(A0, A1, 2);
}

void loop() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();  

  long interval = 500;  

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    get_Direction();
    
  }
}

void get_Direction() {
    int XY[2];
    get_xy(XY);
    int x = XY[0], y = XY[1];

    if (abs(x - 180) > move_range || abs(y - 180) > move_range) {
      if (abs(x - 180) > abs(y - 180)) {
        if (x > 180) {
          Serial.println("LEFT");
        }
        else {
          Serial.println("RIGHT");
        }
      }
      else {
        if (y > 180) {
          Serial.println("UP");
        }
        else {
          Serial.println("DOWN");
        }
      }
    }
}
