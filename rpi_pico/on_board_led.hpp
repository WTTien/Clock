/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ON_BOARD_LED_HPP
#define ON_BOARD_LED_HPP

#include "pico/stdlib.h"

class OnBoardLED {
public:
    OnBoardLED();
    bool init();
    void set_led(bool led_on);
};

#endif // ON_BOARD_LED_HPP