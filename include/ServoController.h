
#ifndef ServoController_h
#define ServoController_h
#include <stdint.h>
#include <Arduino.h>

struct ServoChannel{
    uint8_t pin;
    uint16_t pulseWidth;
};

class ServoController{
    public:
    static const uint8_t MAX_SERVOS = 8;
    //Todo: Finding the right pulse width for a servo can be tricky, manufacturer sad 1000 - 2000 us but that was only like 70 isch degrees.. 
    static const uint16_t MIN_PULSE_WIDTH = 500;
    static const uint16_t MAX_PULSE_WIDTH = 2500; 
    
    ServoController();
    void start();
    void stop();
    bool attach(uint8_t pin, uint16_t pulseWidth = 1500);
    bool detach(uint8_t pin);
    bool write(uint8_t pin, uint8_t angle);
    bool writeMicroseconds(uint8_t pin, uint16_t us);
    
    ServoChannel servos[MAX_SERVOS];
    uint8_t servoCount;
    
    uint8_t order[MAX_SERVOS];
    volatile uint8_t currentIndex;
    ServoChannel getServoChannelAt(uint8_t index);
    bool advanceIndex(); 

    void setAllHigh();
    void setCurrentPinLow();
    
    private:
    void reorder();
    
};

// Declare global instance of ServoControler
extern ServoController servoController;

#endif // ServoController_h