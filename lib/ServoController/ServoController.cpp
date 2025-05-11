
#include "ServoController.h"
#include <Arduino.h>

// Define global instance of ServoController
ServoController servoController;

ServoController::ServoController() : servoCount(0), currentIndex(0) {

  //Configure timer1:
  cli(); // disable interrupts while setting up

  // Reset Timer1 control registers
  TCCR1A = 0;
  TCCR1B = 0;

  // Set to CTC mode (Clear Timer on Compare Match)
  TCCR1B |= (1 << WGM12);

  // Prescaler 8 → 0.5 µs per tick
  TCCR1B |= (1 << CS11);

  // OCR1A = 40000 ticks → 20 ms
  OCR1A = 40000;

  // Enable COMPA and COMPB interrupts
  TIMSK1 |= (1 << OCIE1A); // start pulse
  TIMSK1 |= (1 << OCIE1B); // end pulse

  sei(); // enable global interrupts
}

void ServoController::reorder() {
  for (uint8_t i = 0; i < servoCount; i++) {
    order[i] = i;
  }

  for (uint8_t i = 0; i < servoCount - 1; i++) {
    for (uint8_t j = 0; j < servoCount - i - 1; j++) {
      if (servos[order[j]].pulseWidth > servos[order[j + 1]].pulseWidth) {
        uint8_t temp = order[j];
        order[j] = order[j + 1];
        order[j + 1] = temp;
      }
    }
  }
}

bool ServoController::attach(uint8_t pin) {
  if (servoCount >= MAX_SERVOS)
    return false;

  servos[servoCount] = {pin, DEFAULT_PULSE_WIDTH};
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  reorder();

  servoCount++;
  return true;
}

bool ServoController::detach(uint8_t pin) {
  for (uint8_t i = 0; i < servoCount; i++) {
    if (servos[i].pin == pin) {
      // Shift servos down
      for (uint8_t j = i; j < servoCount - 1; j++) {
        servos[j].pin = servos[j + 1].pin;
        servos[j].pulseWidth = servos[j + 1].pulseWidth;
      }
      servoCount--;
      reorder();
      return true;
    }
  }
  return false;
}

bool ServoController::write(uint8_t pin, uint8_t angle) {
  if (angle < 0 || angle > 180)
    return false;

  uint16_t pulseWidth = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  return writeMicroseconds(pin, pulseWidth);
}

bool ServoController::writeMicroseconds(uint8_t pin, uint16_t us) {
  if (us < MIN_PULSE_WIDTH || us > MAX_PULSE_WIDTH)
    return false;

  for (uint8_t i = 0; i < servoCount; i++) {
    if (servos[i].pin == pin) {
      servos[i].pulseWidth = us;
      reorder();
      return true;
    }
  }
  return false;
}

void ServoController::setAllHigh() {
  for (uint8_t i = 0; i < servoCount; i++) {
    digitalWrite(servos[i].pin, HIGH);
  }
}

void ServoController::setCurrentPinLow() {
  digitalWrite(servos[order[currentIndex]].pin, LOW);
}

// Timer1 interrupt service routine for starting the pulse
ISR(TIMER1_COMPA_vect) {
  if (servoController.servoCount <= 0)
    return;

  servoController.setAllHigh();
  
  servoController.currentIndex = 0;
  OCR1B = TCNT1 + (servoController.servos[servoController.order[0]].pulseWidth * 2);
}

// Timer1 interrupt service routine for ending the pulse
ISR(TIMER1_COMPB_vect) {
  servoController.setCurrentPinLow();
  servoController.currentIndex++;

  while (servoController.currentIndex < servoController.servoCount && )

  //set ocr1b for next pin
  uint16_t timeDiff = servos[_order[_currentIndex + 1]].pulseWidth - servos[_order[_currentIndex]].pulseWidth;
}

