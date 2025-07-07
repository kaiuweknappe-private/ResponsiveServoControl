#ifndef ServoControllerESP32_h
#define ServoControllerESP32_h

#include <stdint.h>
#include <Arduino.h>

struct ServoChannel {
    uint8_t pin;
    uint16_t pulseWidth; // in microseconds
};

class ServoController {
public:
    static const uint8_t MAX_SERVOS = 8;
    static const uint16_t MIN_PULSE_WIDTH = 1000;
    static const uint16_t MAX_PULSE_WIDTH = 2000;
    static const uint16_t FRAME_LENGTH_US = 20000;

    ServoController();
    void start();
    void stop();
    bool attach(uint8_t pin, uint16_t pulseWidth = 1500);
    bool detach(uint8_t pin);
    bool write(uint8_t pin, uint8_t angle); // 0–180
    bool writeMicroseconds(uint8_t pin, uint16_t us);

private:
    ServoChannel servos[MAX_SERVOS];
    uint8_t servoCount;
    uint8_t order[MAX_SERVOS];
    volatile uint8_t currentIndex;

    hw_timer_t* timer;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

    void reorder();
    void beginFrame();
    void scheduleNextLow();
    static void IRAM_ATTR onTimerStatic();
    void IRAM_ATTR onTimer();
};

extern ServoController servoController;

#endif // ServoControllerESP32_h
