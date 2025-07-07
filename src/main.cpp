#include <Arduino.h>
#include "ServoController.h"

void setup() {

  // HIER DARF KEIN DELAY SEIN
  
  servoController.attach(8, 1500); // funktioniert nur mit non pwm pins for some reason 
  //delay(40); MUSS AUCH NICHT SEIEN ANSCHEINEEND 
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
