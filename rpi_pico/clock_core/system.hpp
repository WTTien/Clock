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


#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

#define I2C_PORT i2c1
#define I2C_SCL_PIN 15
#define I2C_SDA_PIN 14
#define I2C_BAUDRATE 100000

class System {
public:
    System();

    OnBoardLED onboard_led;
    LED led_1A;
    GPIOStepperMotor OLED_motor;
    PCF8574StepperMotor hour_minute_motor;
    PCF8575StepperMotor date_motor;
    RealTimeClock rtc;

    bool init();
    void run();
};

void process_event_queue(System* clock_system);

#endif // SYSTEM_HPP