#include "system.hpp"
#include "hardware/i2c.h"

#include "tusb.h"
#include <bsp/board_api.h>
#include <stdio.h>

#include "pico/multicore.h"

#include "event_processor.hpp"

// Read time thruogh WiFi
#include "pico/cyw43_arch.h"
#include <time.h>

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

#define I2C_PORT i2c1
#define I2C_SCL_PIN 15
#define I2C_SDA_PIN 14
#define I2C_BAUDRATE 100000

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

void core1_main() {
    while (true) {
        tud_task();
        usb_write();
    }
}

int main() {

    // Initialize TinyUSB stack
    board_init();
    tusb_init();
    
    // This is for USB serial input output
    stdio_init_all();    

    // Launch core 1 to handle USB in the background
    multicore_launch_core1(core1_main);

    // For PICO hardware peripherals initialization
    // Initialise I2C bus
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL_PIN);
    gpio_pull_up(I2C_SDA_PIN);

    // For system component initialization
    System clock_system;

    // Onboard LED
    int rc = clock_system.onboard_led.init();
    hard_assert(rc == PICO_OK);

    // Jellybean LED
    rc = clock_system.led_1A.init();
    hard_assert(rc == PICO_OK);

    // OLED Motor
    rc = clock_system.OLED_motor.init();
    hard_assert(rc == PICO_OK);

    // hour_minute_motor
    rc = clock_system.hour_minute_motor.init();
    hard_assert(rc == PICO_OK);

    // date_motor
    rc = clock_system.date_motor.init();
    hard_assert(rc == PICO_OK);

    // Real Time Clock
    rc = clock_system.rtc.init();
    hard_assert(rc == PICO_OK);

    cyw43_arch_enable_sta_mode();
    
    rc = cyw43_arch_wifi_connect_timeout_ms(
        WIFI_SSID,
        WIFI_PASSWORD,
        CYW43_AUTH_WPA2_AES_PSK,
        60000
    );
    hard_assert(rc == PICO_OK);

    send_to_print_safe("Starting programme!\n");
    
    while (true)
    {
        // clock_system.onboard_led.set_led(false);
        // clock_system.led_1A.set_led(true);
        // sleep_ms(LED_DELAY_MS);
        // clock_system.onboard_led.set_led(true);
        // clock_system.led_1A.set_led(false);
        sleep_ms(LED_DELAY_MS);

        int status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
        switch (status) {
            case CYW43_LINK_JOIN:
            case CYW43_LINK_NOIP:
            case CYW43_LINK_UP:
                // send_to_print_safe("[WiFi] Connected to WiFi network.\n");
                printf("[WiFi] Connected to WiFi network.\n");
                break;
            default:
                // send_to_print_safe("[WiFi] Disconnected.\n");
                printf("[WiFi] Disconnected.\n");
                break;
        }

        clock_system.OLED_motor.step(256); // Clockwise
        // clock_system.hour_minute_motor.step(256); // Clockwise
        // clock_system.date_motor.step(256); // Clockwise

        send_to_print_safe("Hello World!\n");
        process_event_queue(clock_system);
    }
}