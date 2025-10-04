/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdio.h>

const int LED_GPIO[] = {6, 7, 8, 9, 10, 11};
const int NUM_LEDS = sizeof(LED_GPIO) / sizeof(LED_GPIO[0]);

// Perform initialisation
int led_init(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_init(LED_GPIO[i]);
        gpio_set_dir(LED_GPIO[i], GPIO_OUT);
    }
    return PICO_OK;
}

// Turn the led on or off
void set_led(int pin, bool led_on) {
    gpio_put(pin, led_on);
}

// Next work : Modify this to have a LED class, and create an object for each LED.
// THey can retrieve pin number and do set_led