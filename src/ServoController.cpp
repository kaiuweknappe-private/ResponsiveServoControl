
#include "ServoController.h"

// Define global instance of ServoController
ServoController servoController;

ServoController::ServoController() : servoCount(0), currentIndex(0) { cli(); }

void ServoController::start() {
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

  TIMSK1 |= (1 << OCIE1A); // start pulse
  TIMSK1 |= (1 << OCIE1B); //en/disabling this everytime in the isr's didnt work properly ..

  sei(); // enable interrupts
}

void ServoController::stop() {
  cli(); 
  TIMSK1 &= ~(1 << OCIE1A); // stop pulse
  TIMSK1 &= ~(1 << OCIE1B); // stop pulse
}

void ServoController::reorder() {
  cli();
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
  sei();
}

bool ServoController::attach(uint8_t pin, uint16_t pulseWidth) {
  if (servoCount >= MAX_SERVOS)
    return false;

  servos[servoCount] = {pin, pulseWidth};
  servoCount++;

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  reorder();

  return true;
}

bool ServoController::detach(uint8_t pin) {
  cli();
  for (uint8_t i = 0; i < servoCount; i++) {
    if (servos[i].pin == pin) {
      // Shift servos down
      for (uint8_t j = i; j < servoCount - 1; j++) {
        servos[j].pin = servos[j + 1].pin;
        servos[j].pulseWidth = servos[j + 1].pulseWidth;
      }
      servoCount--;
      reorder();
      sei();
      return true;
    }
  }
  sei();
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

bool ServoController::advanceIndex() {
  currentIndex++;
  if (currentIndex >= servoCount) {
    currentIndex = 0;
    return false;
  }
  return true;
}

ServoChannel ServoController::getServoChannelAt(uint8_t index) {
  if (index < servoCount) {
    return servos[order[index]];
  }
  return {0, 0};
}

// Timer1 interrupt service routine for starting the pulse
ISR(TIMER1_COMPA_vect) {
  if (servoController.servoCount <= 0)
    return;

  //digitalWrite(LED_BUILTIN, HIGH);
  servoController.setAllHigh();
  
  servoController.currentIndex = 0;
  OCR1B = TCNT1 + (servoController.getServoChannelAt(servoController.currentIndex).pulseWidth * 2);
}

// Timer1 interrupt service routine for ending the pulse
ISR(TIMER1_COMPB_vect) {
  uint16_t currentPulse = servoController.getServoChannelAt(servoController.currentIndex).pulseWidth;
  
  // Set the next pin to low aswell if the pulse width is the same
  while (servoController.getServoChannelAt(servoController.currentIndex).pulseWidth == currentPulse) {
    servoController.setCurrentPinLow();
    if(!servoController.advanceIndex()) {
      //digitalWrite(LED_BUILTIN, LOW);
      return; 
    }
  }
  
  // scedule next pulse end
  uint16_t timeToNextPulseEnd = servoController.getServoChannelAt(servoController.currentIndex).pulseWidth - currentPulse;
  OCR1B = TCNT1 + (timeToNextPulseEnd * 2);
}