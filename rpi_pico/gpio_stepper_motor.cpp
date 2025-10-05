#include "gpio_stepper_motor.hpp"

GPIOStepperMotor::GPIOStepperMotor(uint pin1, uint pin2, uint pin3, uint pin4) {
    pins[0] = pin1;
    pins[1] = pin2;
    pins[2] = pin3;
    pins[3] = pin4;
}

bool GPIOStepperMotor::init() {
    for (int i = 0; i < 4; i++) {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_OUT);
        gpio_put(pins[i], 0);
    }
    return PICO_OK;
}

void GPIOStepperMotor::applyOutput(uint8_t output) {
    for (int i = 0; i < 4; i++) {
        gpio_put(pins[i], (output >> i) & 0x01);
    }
}