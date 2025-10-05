#include "stepper_motor.hpp"

void StepperMotor::setDirection(bool dir) {
    direction = dir;
}

void StepperMotor::setStepDelay(uint32_t delay) {
    step_delay_ms = delay;
}

void StepperMotor::step(int steps){
    for (int i = 0; i < steps; i++) {
        singleStep();
        sleep_ms(step_delay_ms);
    }
    disable();
}

// TODO: This might need improvement?
void StepperMotor::disable() {
    applyOutput(0); 
}

void StepperMotor::singleStep() {
    if (direction) {
        current_step = (current_step + 1) % 8;
    } else {
        current_step = (current_step + 7) % 8; // Equivalent to -1 mod 8
    }
    applyOutput(step_sequence[current_step]);
}