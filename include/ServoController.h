
#ifndef ServoController_h
#define ServoController_h


class ServoController{
    public:
    ServoController();
    bool attach(uint8_t pin);
    bool detach(uint8_t pin);
    bool write(uint8_t pin, uint8_t angle);
    bool writeMicroseconds(uint8_t pin, uint16_t us);
    
    static const uint8_t MAX_SERVOS = 8;

    static const uint16_t MIN_PULSE_WIDTH = 500; // 0.5 ms
    static const uint16_t MAX_PULSE_WIDTH = 2500; // 2.5 ms
    static const uint16_t DEFAULT_PULSE_WIDTH = 1500; // 1.5 ms
    
    private:
    uint8_t _pins[MAX_SERVOS];
    uint16_t _pulseWidths[MAX_SERVOS];
    uint8_t _servoCount;
    
    volatile uint8_t _currentServo;
    
};

// Declare global instance of ServoControler
extern ServoController servoController;

#endif // ServoControllr_h