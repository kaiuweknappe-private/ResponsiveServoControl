#include "ServoControllerESP32.h"

ServoController servoController;

ServoController::ServoController() : servoCount(0), currentIndex(0), timer(nullptr) {}

bool ServoController::attach(uint8_t pin, uint16_t pulseWidth = 1500) {
    if (servoCount >= MAX_SERVOS) return false;

    for (uint8_t i = 0; i < servoCount; i++)
        if (servos[i].pin == pin) return false;

    servos[servoCount].pin = pin;
    servos[servoCount].pulseWidth = pulseWidth;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    servoCount++;
    reorder(pin);
    return true;
}

bool ServoController::detach(uint8_t pin) {
    for (uint8_t i = 0; i < servoCount; i++) {
        if (servos[i].pin == pin) {
            for (uint8_t j = i; j < servoCount - 1; j++)
                servos[j] = servos[j + 1];
            servoCount--;
            return true;
        }
    }
    return false;
}

bool ServoController::write(uint8_t pin, uint8_t angle) {
    if (angle > 180) return false;
    uint16_t us = MIN_PULSE_WIDTH + ((uint32_t)(MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) * angle) / 180;
    return writeMicroseconds(pin, us);
}

bool ServoController::writeMicroseconds(uint8_t pin, uint16_t us) {
    if (us < MIN_PULSE_WIDTH || us > MAX_PULSE_WIDTH) return false;
    for (uint8_t i = 0; i < servoCount; i++) {
        if (servos[i].pin == pin) {
            servos[i].pulseWidth = us;
            reorder(pin);
            return true;
        }
    }
    return false;
}

void ServoController::start() {
    if (!timer) {
        timer = timerBegin(0, 80, true); // 1us ticks
        timerAttachInterrupt(timer, &ServoController::onTimerStatic, true);
    }
    beginFrame();
}

void ServoController::stop() {
    if (timer) {
        timerEnd(timer);
        timer = nullptr;
    }
}

void ServoController::reorder(uint8_t pin) {
    // 1. Find the index of the modified servo in servos[]
    int modifiedIndex = -1;
    for (uint8_t i = 0; i < servoCount; i++) {
        if (servos[i].pin == pin) {
            modifiedIndex = i;
            break;
        }
    }
    if (modifiedIndex == -1) return; // not found

    // 2. Find current position in order[]
    int currentOrderPos = -1;
    for (uint8_t i = 0; i < servoCount; i++) {
        if (order[i] == modifiedIndex) {
            currentOrderPos = i;
            break;
        }
    }
    if (currentOrderPos == -1) return;

    // 3. Remove from order[]
    for (uint8_t i = currentOrderPos; i < servoCount - 1; i++) {
        order[i] = order[i + 1];
    }
    uint8_t newLength = servoCount - 1;

    // 4. Find new position using binary search
    uint16_t targetPulse = servos[modifiedIndex].pulseWidth;
    uint8_t insertPos = 0;
    while (insertPos < newLength &&
           servos[order[insertPos]].pulseWidth < targetPulse) {
        insertPos++;
    }

    // 5. Shift elements to make room
    for (uint8_t i = newLength; i > insertPos; i--) {
        order[i] = order[i - 1];
    }

    // 6. Insert modifiedIndex back
    order[insertPos] = modifiedIndex;
}

void ServoController::beginFrame() {
    portENTER_CRITICAL(&mux);
    for (uint8_t i = 0; i < servoCount; i++) {
        digitalWrite(servos[i].pin, HIGH);
    }
    timerWrite(timer, 0);
    currentIndex = 0;

    if (servoCount > 0) {
        timerAlarmWrite(timer, servos[order[0]].pulseWidth, false);
        timerAlarmEnable(timer);
    } else {
        timerAlarmWrite(timer, FRAME_LENGTH_US, false);
        timerAlarmEnable(timer);
    }
    portEXIT_CRITICAL(&mux);
}

void ServoController::scheduleNextLow() {
    if (currentIndex >= servoCount) return;

    uint16_t targetTime = servos[order[currentIndex]].pulseWidth;

    while (currentIndex < servoCount &&
           servos[order[currentIndex]].pulseWidth == targetTime) {
        digitalWrite(servos[order[currentIndex]].pin, LOW);
        currentIndex++;
    }

    if (currentIndex < servoCount) {
        uint16_t nextTime = servos[order[currentIndex]].pulseWidth;
        timerAlarmWrite(timer, nextTime, false);
        timerAlarmEnable(timer);
    } else {
        timerAlarmWrite(timer, FRAME_LENGTH_US, false);
        timerAlarmEnable(timer);
    }
}

void IRAM_ATTR ServoController::onTimerStatic() {
    servoController.onTimer();
}

void IRAM_ATTR ServoController::onTimer() {
    portENTER_CRITICAL_ISR(&mux);
    scheduleNextLow();
    portEXIT_CRITICAL_ISR(&mux);
}
