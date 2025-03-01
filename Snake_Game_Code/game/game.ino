#include <math.h>

const int xaxis = A0;
const int yaxis = A1;
const int sw = 2;

const int move_range = 20; 

void setup() {
  pinMode(xaxis, INPUT);
  pinMode(yaxis, INPUT);
  pinMode(sw, INPUT);

  Serial.begin(9600);

}

void loop() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();  

  long interval = 500;  

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    int XY[2];  
    get_xy(XY); 
    int x = XY[0], y = XY[1];

    if(abs(x - 180) > move_range || abs(y - 180 ) > move_range) {
        if(abs(x - 180 ) > abs(y - 180 )) {
          if(x > 180) {
            Serial.println("LEFT");
          }
          else {
            Serial.println("RIGHT");
          }
        }

        else {
          if(y > 180) {
            Serial.println("UP");
          }
          else {
            Serial.println("DOWN");
          }
        }
    }



    // Serial.print("Y: ");
    // Serial.print(XY[1]);
    // Serial.print("  X: ");
    // Serial.println(XY[0]);
  }
}

void get_xy(int *XY) {
  int X = analogRead(xaxis);  
  int Y = analogRead(yaxis);  

  XY[0] = map(X, 0, 1023, 0, 360);
  XY[1] = map(Y, 0, 1023, 0, 360);

}

