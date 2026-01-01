#ifndef STEPPER_MOTOR_HPP
#define STEPPER_MOTOR_HPP

#include "pico/stdlib.h"
#include <cstdlib>

class StepperMotor {
public:
    virtual ~StepperMotor() = default;
    void setDirection(bool dir);
    void setStepDelay(uint32_t delay);
    void step(int steps);
    virtual void disable();
    virtual bool init() = 0;

protected:
    static constexpr uint8_t step_sequence[8] = {
        0b0001, 0b0011, 0b0010, 0b0110,
        0b0100, 0b1100, 0b1000, 0b1001
    };
    int current_step = 0;
    // TODO: Verify following
    bool direction = true; // true for CW, false for CCW
    uint32_t step_delay_ms = 5; // Default delay between steps
    virtual void applyOutput(uint8_t output) = 0;
    void singleForwardStep();
    void singleBackwardStep();
};

#endif // STEPPER_MOTOR_HPP