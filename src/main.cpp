
#include "ServoControllerESP32.h"


void setup() {
  servoController.attach(5);
  servoController.attach(18);
  servoController.attach(19);
  servoController.start();

  while (1);
  //delay(3000); // ob man das brauch weiß ich nicht, aber ja.. 
}

// void sweep();
// uint16_t position = 0;
// int direction = 1;

void loop() {
  //sweep();
  
  
  
}


// void sweep() {
//   servoController.write(9, position);

//   position += direction;

//   if (position >= 180 || position <= 0) {
//     direction = -direction;
//   }

//   delay(10);
// }
