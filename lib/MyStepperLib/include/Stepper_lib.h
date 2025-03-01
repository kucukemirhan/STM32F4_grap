#ifndef INC_STEPPER_LIB_H_
#define INC_STEPPER_LIB_H_

#include "Encoder_lib.h"
#include "TIM_lib.h"
#include "GPIO_lib.h"

// This version of the StepperMotor class accepts pre-constructed objects.
// It uses:
// - EncoderIT for position feedback,
// - TimPWM for generating step pulses,
// - DigitalOut for controlling the motor direction.
class StepperMotor {
public:
    // Constructor accepts references to already configured objects.
    StepperMotor(EncoderIT &encoder, TimPWM &pwm, DigitalOut &direction);
    ~StepperMotor();

    // Set the target position (in steps).
    void setTargetPosition(int32_t position);

    // Set the stepping speed (in steps per second).
    void setSpeed(uint16_t stepsPerSecond);

    // Call periodically to update motor control.
    void update();

    // Get the current position.
    int32_t getCurrentPosition();

    // Stop the motor movement.
    void stop();

private:
    EncoderIT &encoder;      // Reference to an externally created encoder
    TimPWM &pwm;             // Reference to an externally created PWM timer
    DigitalOut &direction;   // Reference to an externally created digital output

    int32_t targetPosition;  // Desired position in steps
    uint16_t speed;          // Stepping speed (steps per second)
    bool isPwmRunning;       // Flag to track if PWM is already active
};

#endif // /* INC_STEPPER_LIB_H_ */