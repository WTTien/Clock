#include "real_time_clock.hpp"

RealTimeClock::RealTimeClock(
    i2c_inst_t *i2c, 
    uint8_t address, 
    uint8_t reset_pin, 
    uint8_t int_pin
) :
    i2c(i2c), 
    address(address), 
    reset_pin(reset_pin), 
    int_pin(int_pin) 
{
}

bool RealTimeClock::init() 
{
    // Initialize I2C
    // Assuming I2C is already initialized outside this class

    // Initialize reset pin
    gpio_init(reset_pin);
    gpio_set_dir(reset_pin, GPIO_OUT);
    gpio_put(reset_pin, 1); // Keep RTC out of reset

    // Initialize interrupt pin
    gpio_init(int_pin);
    gpio_set_dir(int_pin, GPIO_IN);
    gpio_pull_up(int_pin); // Enable pull-up resistor

    // Optionally test communication
    uint8_t reg = DS3231_REG_TIME;
    if (i2c_write_blocking(i2c, address, &reg, 1, true) < 0)
        return false;

    return PICO_OK;
}

bool RealTimeClock::readTime(DateTime &dt) 
{
    uint8_t reg = DS3231_REG_TIME;
    uint8_t data[7]; // sec, min, hr, day, date, month, year

    if (i2c_write_blocking(i2c, address, &reg, 1, true) < 0)
        return false;

    if (i2c_read_blocking(i2c, address, data, 7, false) < 0)
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

bool RealTimeClock::setTime(const DateTime &dt) 
{
    uint8_t data[8];
    data[0] = DS3231_REG_TIME; // Start writing at the time register
    data[1] = decToBcd(dt.second);
    data[2] = decToBcd(dt.minute);
    data[3] = decToBcd(dt.hour); // 24-hour mode
    data[4] = decToBcd(dt.day);  // Day of the week
    data[5] = decToBcd(dt.date);
    data[6] = decToBcd(dt.month);
    data[7] = decToBcd(dt.year - 2000); // Store only the last 2 digits of the year

    // Write the data to the DS3231
    if (i2c_write_blocking(i2c, address, data, 8, false) < 0) {
        return false; // Write failed
    }

    return true; // Write successful
}

uint8_t RealTimeClock::bcdToDec(uint8_t val) 
{
    return (val / 16 * 10) + (val % 16);
}

uint8_t RealTimeClock::decToBcd(uint8_t val) 
{
    return (val / 10 * 16) + (val % 10);
}