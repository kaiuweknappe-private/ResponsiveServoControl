
#include "ServoControllerESP32.h"


void setup() {
  servoController.attach(5);
  servoController.attach(18);
  servoController.attach(19);
  servoController.start();
  
}

void sweep();
uint16_t position = 0;
int direction = 1;

void loop() {
  //sweep();
  
  servoController.write(9, 0);
  servoController.write(10, 0);
  delay(1000);
  servoController.write(9, 180);
  servoController.write(10, 180);
  delay(1000);
  
}


void sweep() {
  servoController.write(9, position);

  position += direction;

  if (position >= 180 || position <= 0) {
    direction = -direction;
  }

  delay(10);
}
