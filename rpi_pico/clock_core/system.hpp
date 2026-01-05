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

#define DATE_TENS_STEPS 1223
#define DATE_ONES_STEPS 1223
#define MONTH_STEPS 682

struct ClockState {
    uint32_t curr_minute_steps; //0-8151
    uint32_t curr_hour_steps; //0-4075

    uint8_t curr_minute; //0-59
    uint16_t curr_hour; //0-719 (Increment every minute by 1, reset after 12 hours)

    uint8_t curr_date_ones; //0-9
    uint8_t curr_date_tens; //0-3
    uint8_t curr_month; //1-12
};

class System {
public:
    System();

    OnBoardLED onboard_led;
    LED led_1A;
    GPIOStepperMotor hour_motor;
    GPIOStepperMotor minute_motor;
    PCF8575StepperMotor date_tenth_motor;
    PCF8575StepperMotor date_ones_motor;
    PCF8575StepperMotor month_motor;
    // PCF8574StepperMotor date_motor;
    RealTimeClock rtc;

    bool init();
    void run();
    void move_minutes(uint8_t minutes);
    void move_hours(uint16_t hours);
    void move_date_tens(int8_t tens);
    void move_date_ones(int8_t ones);
    void move_months(int8_t months);

    ClockState state_{};
    
    
    /// TEST UTILITIES ///
    uint8_t test_minute = 0;
    uint8_t test_hour = 0;
    void set_test_minute_hour(uint8_t minutes, uint8_t hours);
    uint8_t get_test_minute();
    uint8_t get_test_hour();
    /// TEST UTILITIES ///

};

void process_event_queue(System* clock_system);

#endif // SYSTEM_HPP