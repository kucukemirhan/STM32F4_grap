#include "Stepper_lib.h"

StepperMotor::StepperMotor(EncoderIT &enc, TimPWM &pwmTimer, DigitalOut &dir)
    : encoder(enc),
      pwm(pwmTimer),
      direction(dir),
      targetPosition(0),
      speed(0),
      isPwmRunning(false)
{
    // Assume the injected objects are pre-configured.
}

StepperMotor::~StepperMotor() {
    stop();
}

void StepperMotor::setTargetPosition(int32_t position) {
    targetPosition = position;
}

void StepperMotor::setSpeed(uint16_t stepsPerSecond) {
    speed = stepsPerSecond;
    pwm.setFrequency(stepsPerSecond);
}

int32_t StepperMotor::getCurrentPosition() {
    return encoder.read();
}

void StepperMotor::update() {
    if (!isPwmRunning) return;
    
    int32_t currentPosition = getCurrentPosition();

    int32_t error = targetPosition - currentPosition;
    if (std::abs(error) <= 10) {
        this->stop();
        return;
    }

    uint8_t dir = (error >= 0) ? 1 : 0;
    direction.write(dir ? GPIO_PIN_RESET : GPIO_PIN_SET);

    pwm.setFrequency(speed);
    this->start();
}

void StepperMotor::start() {
    if (!isPwmRunning) {
        pwm.start();
        isPwmRunning = true;
    }
}


void StepperMotor::stop() {
    if (isPwmRunning) {
        pwm.stop();
        isPwmRunning = false;
    }
}
