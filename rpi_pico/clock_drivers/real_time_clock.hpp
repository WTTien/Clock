#ifndef REAL_TIME_CLOCK_HPP
#define REAL_TIME_CLOCK_HPP

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "../clock_services/usb.hpp"

#define DS3231_REG_TIME 0x00 // Seconds register

struct DateTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
};

void rtc_interrupt_handler(uint gpio, uint32_t events);

class RealTimeClock {
public:
    RealTimeClock(i2c_inst_t *i2c, uint8_t address, uint8_t reset_pin, uint8_t int_pin);
    bool init();
    bool readTime(struct DateTime &dt);
    bool setTime(const DateTime &dt);
    bool readRegister(uint8_t reg, uint8_t &value, size_t length);
    bool writeRegister(uint8_t reg, const uint8_t *value, size_t length);


private:
    i2c_inst_t *i2c;
    uint8_t address;
    uint8_t reset_pin;
    uint8_t int_pin;

    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
};

#endif // REAL_TIME_CLOCK_HPP