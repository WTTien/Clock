#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "on_board_led.hpp"
#include "led.hpp"
#include "gpio_stepper_motor.hpp"
#include "pcf_stepper_motor.hpp"
#include "real_time_clock.hpp"
#include "usb.hpp"

class System {
public:
    System();

    OnBoardLED onboard_led;
    LED led_1A;
    GPIOStepperMotor OLED_motor;
    PCF8574StepperMotor hour_minute_motor;
    PCF8575StepperMotor date_motor;
    RealTimeClock rtc;
};

#endif // SYSTEM_HPP