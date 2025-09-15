#include "Stepper_lib.h"

StepperMotor::StepperMotor(EncoderIT &enc, TimPWM &pwmTimer, DigitalOut &dir,
                           float stepsPerMotorRev_, float reduction_)
    : encoder(enc),
      pwm(pwmTimer),
      direction(dir),
      targetPosition(0),
      speed(0),
      isPwmRunning(false),
      stepsPerMotorRev(stepsPerMotorRev_ > 0.f ? stepsPerMotorRev_ : 200.f),
      reduction(reduction_ > 0.f ? reduction_ : 1.0f)
{
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
    // Positive error => need to move forward; Negative => move reverse.
    const int32_t error = targetPosition - getCurrentPosition();

    if (error > deadbandSteps) {
        // Move forward
        direction.write(GPIO_PIN_SET);
        pwm.setFrequency(speed);
        if (!isPwmRunning) {
            pwm.start();
            isPwmRunning = true;
        }
    }
    else if (error < -deadbandSteps) {
        // Move reverse
        direction.write(GPIO_PIN_RESET);
        pwm.setFrequency(speed);
        if (!isPwmRunning) {
            pwm.start();
            isPwmRunning = true;
        }
    }
    else {
        // Inside deadband: stop PWM if running
        if (isPwmRunning) {
            pwm.stop();
            isPwmRunning = false;
        }
        // (Optional) You could also “snap” to target here if you maintain a software position.
    }
}


void StepperMotor::stop() {
    if (isPwmRunning) {
        pwm.stop();
        isPwmRunning = false;
    }
}

void StepperMotor::setTargetDegrees(float degrees) {
    setTargetPosition(degreesToSteps(degrees));
}

void StepperMotor::moveByDegrees(float deltaDegrees) {
    setTargetPosition(targetPosition + degreesToSteps(deltaDegrees));
}

// ===== OpenLoopStepper =====
OpenLoopStepper::OpenLoopStepper(TimPWM &pwmTimer, DigitalOut &dirPin,
                                 float stepsPerMotorRev_, float reduction_)
: pwm(pwmTimer), direction(dirPin),
  stepsPerMotorRev(stepsPerMotorRev_ > 0.f ? stepsPerMotorRev_ : 200.f),
  reduction(reduction_ > 0.f ? reduction_ : 1.0f)
{
    // default DIR: forward
    direction.write(GPIO_PIN_SET);
}

void OpenLoopStepper::setTargetPosition(int32_t position)
{
    targetPosition = position;
}

void OpenLoopStepper::moveBy(int32_t delta)
{
    setTargetPosition(targetPosition + delta);
}

void OpenLoopStepper::setSpeed(uint16_t stepsPerSecond)
{
    speed = stepsPerSecond;
    if (speed > 0) {
        pwm.setFrequency(speed);
    } else {
        // no speed => stop PWM if it was running
        stop();
    }
}

int32_t OpenLoopStepper::getCurrentPosition() const
{
    return virtualPosition;
}

int32_t OpenLoopStepper::getTargetPosition() const
{
    return targetPosition;
}

bool OpenLoopStepper::isBusy() const
{
    return running;
}

void OpenLoopStepper::update()
{
    // 0) Integrate emitted pulses into software position
    if (running) {
        const uint32_t pulses = pwm.consumePeriods();
        if (pulses) {
            virtualPosition += dirPositive ? (int32_t)pulses : -(int32_t)pulses;
        }
    }

    // 1) Deadbanded position error (tune deadband as needed)
    constexpr int32_t deadbandSteps = 100; // replace with a member if you prefer
    const int32_t error = targetPosition - virtualPosition;

    if (error > deadbandSteps) {
        // Move forward
        dirPositive = true;
        direction.write(GPIO_PIN_SET);

        if (speed == 0) {            // safety: no speed → don't run PWM
            if (running) { pwm.stop(); running = false; }
            return;
        }

        pwm.setFrequency(speed);
        if (!running) {
            pwm.start();
            running = true;
        }
    }
    else if (error < -deadbandSteps) {
        // Move reverse
        dirPositive = false;
        direction.write(GPIO_PIN_RESET);

        if (speed == 0) {
            if (running) { pwm.stop(); running = false; }
            return;
        }

        pwm.setFrequency(speed);
        if (!running) {
            pwm.start();
            running = true;
        }
    }
    else {
        // Inside deadband: stop PWM if running
        if (running) {
            pwm.stop();
            running = false;
        }
    }
}


void OpenLoopStepper::stop()
{
    if (running) {
        pwm.stop();
        running = false;
    }
}

void OpenLoopStepper::setTargetDegrees(float degrees)
{
    setTargetPosition(degreesToSteps(degrees));
}

void OpenLoopStepper::moveByDegrees(float deltaDegrees)
{
    moveBy(degreesToSteps(deltaDegrees));
}