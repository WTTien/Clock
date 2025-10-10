#include "on_board_led.hpp"
#include "led.hpp"
#include "gpio_stepper_motor.hpp"
#include "pcf_stepper_motor.hpp"
#include "real_time_clock.hpp"

#include "hardware/i2c.h"

#include <stdio.h>

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

#define I2C_PORT i2c1
#define I2C_SCL_PIN 15
#define I2C_SDA_PIN 14
#define I2C_BAUDRATE 100000

int main() {

    // This is to test USB serial input output
    stdio_init_all();
    char buf[128];
    int idx = 0;
    int c;

    printf("Starting programme!\n");

    // For PICO hardware peripherals initialization
    // Initialise I2C bus
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL_PIN);
    gpio_pull_up(I2C_SDA_PIN);

    // For system component initialization
    OnBoardLED onboard_led;
    int rc = onboard_led.init();
    hard_assert(rc == PICO_OK);

    LED led_1A(6);
    rc = led_1A.init();
    hard_assert(rc == PICO_OK);

    GPIOStepperMotor OLED_Motor(26, 22, 21, 20);
    rc = OLED_Motor.init();
    hard_assert(rc == PICO_OK);

    PCF8574StepperMotor hour_minute_motor(
        I2C_PORT,
        0x21, // I2C address
        0, 1, 2, 3 // Pins on the expander
    );
    rc = hour_minute_motor.init();
    hard_assert(rc == PICO_OK);

    PCF8575StepperMotor date_motor(
        I2C_PORT,
        0x20, // I2C address
        0, 1, 2, 3 // Pins on the expander
    );
    rc = date_motor.init();
    hard_assert(rc == PICO_OK);

    RealTimeClock rtc(
        I2C_PORT,
        0x68, // I2C address
        12,   // Reset pin
        13    // Interrupt pin
    );
    rc = date_motor.init();
    hard_assert(rc == PICO_OK);

    while (true)
    {
        onboard_led.set_led(true);
        led_1A.set_led(true);
        sleep_ms(LED_DELAY_MS);
        onboard_led.set_led(false);
        led_1A.set_led(false);
        sleep_ms(LED_DELAY_MS);

        OLED_Motor.step(256); // Clockwise
        hour_minute_motor.step(256); // Clockwise
        date_motor.step(256); // Clockwise

        printf("Hello World!\n");

        idx = 0;
        while(true) {
            c = getchar_timeout_us(0);
            if(c != PICO_ERROR_TIMEOUT)
            {
                if (c == '\n' || c == '\r') {
                    buf[idx] = '\0';
                    printf("You entered: %s\n", buf);
                    break;
                }
                if (idx < sizeof(buf) - 1) {
                    buf[idx++] = (char)c;
                }
            }
            sleep_ms(1);
        }
    }
}