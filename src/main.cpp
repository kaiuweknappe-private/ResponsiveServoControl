#include <Arduino.h>
#include <ServoController.h>


void setup() {

  servoController.attach(9);
  servoController.start();

}

void loop() {
  // pending tests of the library
}

