#ifndef LED_HPP
#define LED_HPP

#include "pico/stdlib.h"

class LED {
public:
    LED(uint pin);
    bool init();
    void set_led(bool led_on);
    int get_pin() const;

private:
    int pin;
};

#endif // LED_HPP