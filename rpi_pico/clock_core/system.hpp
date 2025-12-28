#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "pico/stdlib.h"

#include "tusb.h"
#include <bsp/board_api.h>
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#include "../clock_services/usb.hpp"
#include "../clock_services/wifi.hpp"

#include "hardware/i2c.h"

#include "../clock_drivers/on_board_led.hpp"
#include "../clock_drivers/led.hpp"
#include "../clock_drivers/gpio_stepper_motor.hpp"
#include "../clock_drivers/pcf_stepper_motor.hpp"
#include "../clock_drivers/real_time_clock.hpp"

#include <cmath>


#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

#define I2C_PORT i2c1
#define I2C_SCL_PIN 15
#define I2C_SDA_PIN 14
#define I2C_BAUDRATE 100000

#define MINUTE_STEPS_PER_REV 8152
#define HOUR_STEPS_PER_REV 4076

struct ClockState {
    uint32_t curr_minute_steps; //0-8151
    uint32_t curr_hour_steps; //0-4075

    uint8_t curr_minute; //0-59
    uint16_t curr_hour; //0-720 (Increment every minute by 1, reset after 12 hours)

    uint8_t curr_date_ones; //0-9
    uint8_t curr_date_tens; //0-3
    uint8_t curr_month; //0-11
};

class System {
public:
    System();

    OnBoardLED onboard_led;
    LED led_1A;
    GPIOStepperMotor hour_motor;
    GPIOStepperMotor minute_motor;
    PCF8574StepperMotor hour_minute_motor;
    PCF8575StepperMotor date_motor;
    RealTimeClock rtc;

    bool init();
    void run();
    void move_minutes(uint8_t minutes);
    void move_hours(uint16_t hours);

    ClockState state_{};
};

void process_event_queue(System* clock_system);

#endif // SYSTEM_HPP