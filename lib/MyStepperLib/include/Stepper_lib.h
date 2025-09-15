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
    //      reduction = motor_rev / output_rev  (e.g. 50:1 gearbox => 50.0f)
    StepperMotor(EncoderIT &encoder, TimPWM &pwm, DigitalOut &direction,
                 float stepsPerMotorRev = 200.0f, float reduction = 1.0f);
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

    void setDeadband(int32_t steps) { deadbandSteps = (steps < 0 ? 0 : steps); }

    // degrees-based API (absolute / relative in degrees at the OUTPUT shaft)
    void setTargetDegrees(float degrees);
    void moveByDegrees(float deltaDegrees);

    // configuration helpers
    void setReduction(float r)           { reduction = (r > 0.f ? r : 1.f); }
    void setStepsPerMotorRev(float spr)  { stepsPerMotorRev = (spr > 0.f ? spr : 200.f); }

private:
    // conversion helpers
    inline float stepsPerOutputRev() const { return stepsPerMotorRev * reduction; }
    inline int32_t degreesToSteps(float deg) const {
        // round to nearest step
        const float steps = (deg / 360.0f) * stepsPerOutputRev();
        return static_cast<int32_t>( (steps >= 0.f) ? (steps + 0.5f) : (steps - 0.5f) );
    }

    EncoderIT &encoder;      // Reference to an externally created encoder
    TimPWM &pwm;             // Reference to an externally created PWM timer
    DigitalOut &direction;   // Reference to an externally created digital output

    int32_t targetPosition;  // Desired position in steps
    uint16_t speed;          // Stepping speed (steps per second)
    bool isPwmRunning;       // Flag to track if PWM is already active

    int32_t deadbandSteps = 200;   // default: ±2 steps deadband (tweak to taste)

    // NEW: kinematics
    float    stepsPerMotorRev;   // microstepped steps/rev at motor
    float    reduction;          // motor_rev / output_rev
};

#pragma once
#include <stdint.h>
#include "TIM_lib.h"
#include "GPIO_lib.h"

/**
 * @brief Open-loop stepper motor controller (no encoder).
 *
 * Generates step pulses via TimPWM and keeps a software "virtual position".
 * It counts emitted pulses using TimPWM::consumePeriods() to stop exactly
 * at the commanded target.
 */
class OpenLoopStepper {
public:
    /**
     * @param pwmTimer  TimPWM instance configured to output STEP pulses
     * @param dirPin    DigitalOut for the DIR pin (GPIO high = forward)
     */
    OpenLoopStepper(TimPWM &pwmTimer, DigitalOut &dirPin,
                    float stepsPerMotorRev = 200.0f, float reduction = 1.0f);

    /** Set absolute target position in steps (relative to virtual 0 at startup) */
    void setTargetPosition(int32_t position);

    /** Relative move helper (delta in steps) */
    void moveBy(int32_t delta);

    /** Set constant speed in steps/second (must be > 0 to move) */
    void setSpeed(uint16_t stepsPerSecond);

    /** Returns current virtual position in steps */
    int32_t getCurrentPosition() const;

    /** Returns current target position in steps */
    int32_t getTargetPosition() const;

    /** Returns true while the motor is still moving toward its target */
    bool isBusy() const;

    /**
     * @brief Progress motion and handle completion.
     * Call this frequently from your main loop (e.g., every 1–5 ms).
     */
    void update();

    /** Immediately stop generating pulses (keeps the current virtual position) */
    void stop();

    // NEW: degrees-based API (absolute / relative at OUTPUT shaft)
    void setTargetDegrees(float degrees);
    void moveByDegrees(float deltaDegrees);

    // NEW: configuration helpers
    void setReduction(float r)           { reduction = (r > 0.f ? r : 1.f); }
    void setStepsPerMotorRev(float spr)  { stepsPerMotorRev = (spr > 0.f ? spr : 200.f); }

private:
    inline float stepsPerOutputRev() const { return stepsPerMotorRev * reduction; }
    inline int32_t degreesToSteps(float deg) const {
        const float steps = (deg / 360.0f) * stepsPerOutputRev();
        return static_cast<int32_t>( (steps >= 0.f) ? (steps + 0.5f) : (steps - 0.5f) );
    }

    TimPWM     &pwm;
    DigitalOut &direction;

    int32_t   virtualPosition = 0;   // software-tracked position (steps)
    int32_t   targetPosition  = 0;   // desired position (steps)
    uint16_t  speed           = 0;   // steps/second
    bool      running         = false;
    bool      dirPositive     = true;
    
    // NEW: kinematics
    float     stepsPerMotorRev;
    float     reduction;
};


#endif // /* INC_STEPPER_LIB_H_ */