#ifndef DS3231_HPP
#define DS3231_HPP

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <cstdint>

struct DateTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
};

class DS3231 {
public:
    DS3231(i2c_inst_t *i2c_port, uint8_t address);

    bool begin();
    bool readTime(DateTime &dt);

private:
    i2c_inst_t *i2c_port_;
    uint8_t address_;

    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
};

#endif // DS3231_HPP

//cpp

#include "ds3231.hpp"
#include <cstdio>

#define DS3231_REG_TIME 0x00  // Seconds register

DS3231::DS3231(i2c_inst_t *i2c_port, uint8_t address)
    : i2c_port_(i2c_port), address_(address) {}

bool DS3231::begin() {
    // Optionally test communication
    uint8_t reg = DS3231_REG_TIME;
    if (i2c_write_blocking(i2c_port_, address_, &reg, 1, true) < 0)
        return false;
    return true;
}

bool DS3231::readTime(DateTime &dt) {
    uint8_t reg = DS3231_REG_TIME;
    uint8_t data[7]; // sec, min, hr, day, date, month, year

    if (i2c_write_blocking(i2c_port_, address_, &reg, 1, true) < 0)
        return false;

    if (i2c_read_blocking(i2c_port_, address_, data, 7, false) < 0)
        return false;

    dt.second = bcdToDec(data[0] & 0x7F);
    dt.minute = bcdToDec(data[1]);
    dt.hour   = bcdToDec(data[2] & 0x3F); // 24-hour mode assumed
    dt.day    = bcdToDec(data[3]);
    dt.date   = bcdToDec(data[4]);
    dt.month  = bcdToDec(data[5] & 0x1F);
    dt.year   = 2000 + bcdToDec(data[6]);

    return true;
}

uint8_t DS3231::bcdToDec(uint8_t val) {
    return (val / 16 * 10) + (val % 16);
}

uint8_t DS3231::decToBcd(uint8_t val) {
    return (val / 10 * 16) + (val % 10);
}


//clock.cpp
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ds3231.hpp"
#include <cstdio>

int main() {
    stdio_init_all();

    // Initialize I2C on pins 14 (SDA) and 15 (SCL)
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);

    DS3231 rtc(i2c1, 0x68);

    if (!rtc.begin()) {
        printf("DS3231 not found!\n");
        return 1;
    }

    while (true) {
        DateTime now;
        if (rtc.readTime(now)) {
            printf("Time: %02u:%02u:%02u %02u/%02u/%u\n",
                now.hour, now.minute, now.second,
                now.date, now.month, now.year);
        } else {
            printf("Failed to read time.\n");
        }
        sleep_ms(1000);
    }
}

