#ifndef GPIO_STEPPER_MOTOR_HPP
#define GPIO_STEPPER_MOTOR_HPP

#include "stepper_motor.hpp"

class GPIOStepperMotor : public StepperMotor {
public:
    GPIOStepperMotor(uint pin1, uint pin2, uint pin3, uint pin4);
    bool init() override;
private:
    uint pins[4];
    void applyOutput(uint8_t output) override;
};

#endif // GPIO_STEPPER_MOTOR_HPP