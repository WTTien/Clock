#include "on_board_led.hpp"
#include "led.hpp"

#include <stdio.h>

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

int main() {

    // This is to test USB serial input output
    stdio_init_all();
    char buf[128];
    int idx = 0;
    int c;

    printf("Starting programme!\n");

    OnBoardLED onboard_led;
    int rc = onboard_led.init();
    hard_assert(rc == PICO_OK);

    LED led_1A(6);
    rc = led_1A.init();
    hard_assert(rc == PICO_OK);
    
    idx = 0;
    while (true)
    {
        onboard_led.set_led(true);
        led_1A.set_led(true);
        sleep_ms(LED_DELAY_MS);
        onboard_led.set_led(false);
        led_1A.set_led(false);
        sleep_ms(LED_DELAY_MS);

        printf("Hello World!\n");

        idx = 0;
        while(true) {
            c = getchar_timeout_us(0);
            if (c == PICO_ERROR_TIMEOUT) break;
            if (c == '\n' || c == '\r') {
                buf[idx] = '\0';
                printf("You entered: %s\n", buf);
                break;
            } else {
                if (idx < sizeof(buf) - 1) {
                    buf[idx++] = (char)c;
                }
            }
            sleep_ms(1);
        }
    }
}