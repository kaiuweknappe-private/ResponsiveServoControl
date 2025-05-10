
#include "ServoControler.h"
#include <Arduino.h>

// Define global instance of ServoControler
ServoControler servoControler;

ServoControler::ServoControler() : _servoCount(0), _currentServo(0) {

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

bool ServoControler::attach(uint8_t pin) {
  if (_servoCount >= MAX_SERVOS)
    return false;

  _pins[_servoCount] = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  _pulseWidths[_servoCount] = DEFAULT_PULSE_WIDTH; 
  _servoCount++;
  return true;
}

bool ServoControler::detach(uint8_t pin) {
  for (uint8_t i = 0; i < _servoCount; i++) {
    if (_pins[i] == pin) {
      // Shift servos down
      for (uint8_t j = i; j < _servoCount - 1; j++) {
        _pins[j] = _pins[j + 1];
        _pulseWidths[j] = _pulseWidths[j + 1];
      }
      _servoCount--;
      return true;
    }
  }
  return false;
}

bool ServoControler::write(uint8_t pin, uint8_t angle) {
  if (angle < 0 || angle > 180)
    return false;

  uint16_t pulseWidth = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  return writeMicroseconds(pin, pulseWidth);
}

