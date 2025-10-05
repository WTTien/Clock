/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdio.h>
#include "led.hpp"

LED::LED(int pin) : pin(pin) {}


bool LED::init() {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    return PICO_OK;
}

void LED::set_led(bool led_on) {
    gpio_put(pin, led_on);
}

int LED::get_pin() const {
    return pin;
}