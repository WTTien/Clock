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
    gpio_set_irq_enabled_with_callback(int_pin, GPIO_IRQ_EDGE_FALL, true, &rtc_interrupt_handler);

    // Set Minute Interrupt
    uint8_t data[4];
    data[0] = 0x0B; // Alarm 2 register address (minutes, hours, day/date)
    data[1] = 0x80; // Mask minutes (A2M2 = 1, match every minute)
    data[2] = 0x80; // Mask hours (A2M3 = 1, match every hour)
    data[3] = 0x80; // Mask day/date (A2M4 = 1, match every day)

    // Write to Alarm 2 registers
    if (i2c_write_blocking(i2c, address, data, 4, false) < 0) {
        return false; // Write failed
    }

    // Enable Alarm 2 interrupt
    uint8_t control_reg = 0x0E; // Control register address
    uint8_t control_val;
    // Read the current control register value
    if (!this->readRegister(control_reg, control_val, 1)) {
        return false; // Read failed
    }
    // Set the A2IE (Alarm 2 Interrupt Enable) bit
    control_val |= 0x06;
    // Write back the updated control register value
    if (!this->writeRegister(control_reg, &control_val, 1)) {
        return false; // Write failed
    }

    uint8_t status_reg = 0x0F; // Status register address
    uint8_t status_val;
    // Read the current status register value
    if (!this->readRegister(status_reg, status_val, 1)) {
        return false; // Read failed
    }
    // Clear A2F (Alarm 2 Flag) bit
    status_val &= ~0x02;
    // Write back the updated status register value
    if (!this->writeRegister(status_reg, &status_val, 1)) {
        return false; // Write failed
    }

    // // Optionally test communication
    // uint8_t reg = DS3231_REG_TIME;
    // if (i2c_write_blocking(i2c, address, &reg, 1, true) < 0)
    //     return false;

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

bool RealTimeClock::readRegister(uint8_t reg, uint8_t &value, size_t length) 
{
    if (i2c_write_blocking(i2c, address, &reg, 1, true) < 0)
        return false;

    if (i2c_read_blocking(i2c, address, &value, length, false) < 0)
        return false;

    return true;
}

bool RealTimeClock::writeRegister(uint8_t reg, const uint8_t *value, size_t length) 
{
    uint8_t data[length+1];
    data[0] = reg;

    for (size_t i = 0; i < length; i++) {
        data[i + 1] = value[i];
    }

    if (i2c_write_blocking(i2c, address, data, length + 1, false) < 0)
        return false;

    return true;
}

bool RealTimeClock::setTimeFromNTP() {
    time_t now = time(NULL);
    now += 8 * 3600;  // Adjust for timezone (e.g., UTC+8)
    
    struct tm *tm = gmtime(&now);

    DateTime dt;
    dt.second = tm->tm_sec;
    dt.minute = tm->tm_min;
    dt.hour   = tm->tm_hour;
    dt.day    = tm->tm_wday + 1; // tm_wday: 0-6, DS3231: 1-7
    dt.date   = tm->tm_mday;
    dt.month  = tm->tm_mon + 1;  // tm_mon: 0-11, DS3231: 1-12
    dt.year   = tm->tm_year + 1900;

    return setTime(dt);
}

void rtc_interrupt_handler(uint gpio, uint32_t events) {
    // Handle RTC interrupt (e.g., read time, clear alarm flag, etc.)
    send_to_print_safe("RTC interrupt triggered: Logging motor spin event.\n");
    char event_msg[64] = "[RTC] INT: NOW\n";
    push_string_into_event_queue(event_msg);

    // Clear the RTC alarm flag
    uint8_t status_reg = 0x0F; // Status register address
    uint8_t status_val;

    // Read the current status register value
    if (i2c_write_blocking(i2c1, 0x68, &status_reg, 1, true) >= 0 &&
        i2c_read_blocking(i2c1, 0x68, &status_val, 1, false) >= 0) {
        // Clear the A2F (Alarm 2 Flag) bit
        status_val &= ~0x02;

        // Write back the updated status register value
        uint8_t data[2] = {status_reg, status_val};
        i2c_write_blocking(i2c1, 0x68, data, 2, false);
    }
}