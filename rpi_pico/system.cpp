#include "system.hpp"

System::System()
    : onboard_led(),
      led_1A(6),
      OLED_motor(26, 22, 21, 20),
      hour_minute_motor(
          i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
          0x21, // I2C address
          0, 1, 2, 3 // Pins on the expander
      ),
      date_motor(
          i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
          0x20, // I2C address
          0, 1, 2, 3 // Pins on the expander
      ),
      rtc(
        i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
        0x68, // I2C address
        12,   // Reset pin
        13    // Interrupt pin
      )
{}