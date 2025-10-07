#ifndef PCF_STEPPER_MOTOR_HPP
#define PCF_STEPPER_MOTOR_HPP

#include "stepper_motor.hpp"
#include "hardware/i2c.h"

class PCFStepperMotor : public StepperMotor {
public:
    PCFStepperMotor(
        i2c_inst_t *i2c,
        uint8_t address, 
        uint8_t pin1, 
        uint8_t pin2, 
        uint8_t pin3, 
        uint8_t pin4
    );
protected:
    i2c_inst_t *i2c;
    uint8_t address;
    uint output_pins[4];
};


///// ----- PCF8574 ----- /////
class PCF8574StepperMotor : public PCFStepperMotor {
public:
    PCF8574StepperMotor(
        i2c_inst_t *i2c,
        uint8_t address, 
        uint8_t pin1, 
        uint8_t pin2, 
        uint8_t pin3, 
        uint8_t pin4
    );
    bool init() override;
private:
    uint8_t outputState = 0x00;
    void applyOutput(uint8_t output) override;
};


///// ----- PCF8575 ----- /////
class PCF8575StepperMotor : public PCFStepperMotor {
public:
    PCF8575StepperMotor(
        i2c_inst_t *i2c,
        uint8_t address, 
        uint8_t pin1, 
        uint8_t pin2, 
        uint8_t pin3, 
        uint8_t pin4
    );
    bool init() override;
private:
    uint16_t outputState = 0x0000;
    void applyOutput(uint8_t output) override;
};

#endif //PCF_STEPPER_MOTOR_HPP