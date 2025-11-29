#include "pcf_stepper_motor.hpp"

PCFStepperMotor::PCFStepperMotor(
    i2c_inst_t *i2c,
    uint8_t address, 
    uint8_t pin1, 
    uint8_t pin2, 
    uint8_t pin3, 
    uint8_t pin4
) :
    i2c(i2c),
    address(address),
    output_pins{pin1, pin2, pin3, pin4} 
{
}


///// ----- PCF8574 ----- /////
PCF8574StepperMotor::PCF8574StepperMotor(
    i2c_inst_t *i2c,
    uint8_t address, 
    uint8_t pin1, 
    uint8_t pin2, 
    uint8_t pin3, 
    uint8_t pin4
) : PCFStepperMotor(i2c, address, pin1, pin2, pin3, pin4) {
}

bool PCF8574StepperMotor::init()
{
    for(int i=0; i<4; i++) {
        if(output_pins[i] > 7) {
            return false; // Invalid pin for PCF8574
        }
    }
    applyOutput(0);
    return PICO_OK;
}

void PCF8574StepperMotor::applyOutput(uint8_t output) {
    uint8_t outputState = 0x00;

    // TODO: This might be inverted - 
    // outputPins = pin1, pin2, pin3, pin4
    // output = pin4, pin3, pin2, pin1

    // Set the required pins based on the output
    for(int i=0; i<4; i++) {
        if(output & (1 << i)) {
            outputState |= (1 << output_pins[i]);
        }
    }

    // Send the output state to the PCF8574
    int ret = i2c_write_blocking(i2c, address, &outputState, 1, false);
    
    // TODO: Delete after
    if (ret < 0) {
        // send_to_print_safe("[PCF8574StepperMotor]: Write failed — no device or NACK\n");
    } else {
        send_to_print_safe("Write succeeded\n");
    } 
}


///// ----- PCF8575 ----- /////
PCF8575StepperMotor::PCF8575StepperMotor(
    i2c_inst_t *i2c,
    uint8_t address, 
    uint8_t pin1, 
    uint8_t pin2, 
    uint8_t pin3, 
    uint8_t pin4
) : PCFStepperMotor(i2c, address, pin1, pin2, pin3, pin4) {
}

bool PCF8575StepperMotor::init()
{
    for(int i=0; i<4; i++) {
        if(output_pins[i] > 15) {
            return false; // Invalid pin for PCF8575
        }
    }
    applyOutput(0);
    return PICO_OK;
}

void PCF8575StepperMotor::applyOutput(uint8_t output) {
    uint16_t outputState = 0x0000;

    // TODO: This might be inverted - 
    // outputPins = pin1, pin2, pin3, pin4
    // output = pin4, pin3, pin2, pin1

    // Set the required pins based on the output
    for(int i=0; i<4; i++) {
        if(output & (1 << i)) {
            outputState |= (1 << output_pins[i]);
        }
    }

    uint8_t data[2] = {
        static_cast<uint8_t>(outputState & 0xFF),
        static_cast<uint8_t>(outputState >> 8)
    };
    
    // Send the output state to the PCF8574
    int ret = i2c_write_blocking(i2c, address, data, 2, false);

    // TODO: Delete after
    if (ret < 0) {
        // send_to_print_safe("[PCF8575StepperMotor]: Write failed — no device or NACK\n");
    } else {
        send_to_print_safe("Write succeeded\n");
    } 
}