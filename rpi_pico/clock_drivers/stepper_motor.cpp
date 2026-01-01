#include "stepper_motor.hpp"

void StepperMotor::setDirection(bool dir) {
    direction = dir;
}

void StepperMotor::setStepDelay(uint32_t delay) {
    step_delay_ms = delay;
}

void StepperMotor::step(int steps) {

    if (steps == 0)
        return;

    const bool forward = (steps > 0);
    const int count = std::abs(steps);

    for (int i = 0; i < count; i++) {
        if (forward) {
            singleForwardStep();
        } else {
            singleBackwardStep();
        }
        sleep_ms(step_delay_ms);
    }
    
    disable();
}

// TODO: This might need improvement?
void StepperMotor::disable() {
    applyOutput(0); 
}

void StepperMotor::singleForwardStep() {
    if (direction) {
        current_step = (current_step + 1) % 8;
    } else {
        current_step = (current_step + 7) % 8; // Equivalent to -1 mod 8
    }
    applyOutput(step_sequence[current_step]);
}

void StepperMotor::singleBackwardStep() {
    if (direction) {
        current_step = (current_step + 7) % 8; // Equivalent to -1 mod 8
    } else {
        current_step = (current_step + 1) % 8; 
    }
    applyOutput(step_sequence[current_step]);
}