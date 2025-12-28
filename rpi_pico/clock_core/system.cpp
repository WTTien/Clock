#include "system.hpp"

// Core 1 main function to handle USB tasks
void core1_main() {
    while (true) {
        tud_task();
        usb_write();
    }
}

System::System()
    : onboard_led(),
      led_1A(6),
      hour_motor(19, 18, 17, 16),
      minute_motor(26, 22, 21, 20),
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

bool System::init()
{
  ///////////////////////////////////
  // Clock_Services Initialisation //
  ///////////////////////////////////

  // USB
  // Initialize TinyUSB stack
  board_init();
  tusb_init();
  // Launch core 1 to handle USB in the background
  multicore_launch_core1(core1_main);

  // WiFi
#ifdef CYW43_WL_GPIO_LED_PIN
  cyw43_arch_init();
  // cyw43_arch_enable_sta_mode();
  // int rc = cyw43_arch_wifi_connect_timeout_ms(
  //     WIFI_SSID,
  //     WIFI_PASSWORD,
  //     CYW43_AUTH_WPA2_AES_PSK,
  //     60000
  // );
  // hard_assert(rc == PICO_OK);
#endif

  //////////////////////////////////////////////
  // PICO Hardware peripherals initialisation //
  //////////////////////////////////////////////

  // I2C bus
  i2c_init(I2C_PORT, I2C_BAUDRATE);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SCL_PIN);
  gpio_pull_up(I2C_SDA_PIN);
  
  
  //////////////////////////////////
  // Clock_Drivers initialisation //
  //////////////////////////////////
  
  // Onboard LED
  int rc = this->onboard_led.init();
  hard_assert(rc == PICO_OK);
  // this->onboard_led.set_led(true);

  // Jellybean LED
  rc = this->led_1A.init();
  hard_assert(rc == PICO_OK);
 
  // Hour Motor
  rc = this->hour_motor.init();
  hard_assert(rc == PICO_OK);

  // Minute Motor
  rc = this->minute_motor.init();
  hard_assert(rc == PICO_OK);

  // hour_minute_motor
  rc = this->hour_minute_motor.init();
  hard_assert(rc == PICO_OK);

  // date_motor
  rc = this->date_motor.init();
  hard_assert(rc == PICO_OK);

  // // Real Time Clock
  rc = this->rtc.init();
  hard_assert(rc == PICO_OK);

  send_to_print_safe("Starting programme!\n");

  return PICO_OK;
}

void System::run()
{
  while(true)
  {
    // this->onboard_led.set_led(true);
    // this->led_1A.set_led(true);
    // sleep_ms(LED_DELAY_MS);
    // this->onboard_led.set_led(false);
    // this->led_1A.set_led(false);
    // sleep_ms(LED_DELAY_MS);

    // this->hour_motor.step(256); // Clockwise
    // this->hour_minute_motor.step(256); // Clockwise
    // this->date_motor.step(256); // Clockwise

    // send_to_print_safe("Hello World!\n");
    // display_wifi_status();
    process_event_queue(this);
    sleep_ms(50);

    // uint8_t control_reg = 0x0E; // Control register address
    // uint8_t control_val;
    // uint8_t status_reg = 0x0F; // Status register address
    // uint8_t status_val;

    // i2c_write_blocking(i2c1, 0x68, &control_reg, 1, true);
    // i2c_read_blocking(i2c1, 0x68, &control_val, 1, false);

    // i2c_write_blocking(i2c1, 0x68, &status_reg, 1, true);
    // i2c_read_blocking(i2c1, 0x68, &status_val, 1, false);

    // char control_reg_print[64];
    // snprintf(control_reg_print, sizeof(control_reg_print), "Control Register (0x0E): 0x%02X\n", control_val);
    // send_to_print_safe(control_reg_print);

    // char status_reg_print[64];
    // snprintf(status_reg_print, sizeof(status_reg_print), "Status Register (0x0F): 0x%02X\n", status_val);
    // send_to_print_safe(status_reg_print);
  }
}

void System::move_minutes(uint8_t minutes)
{

  // 3 cases:
  // 1. Move 1 minute
  // 2. Move 15 minutes from 0 minute position to 15 minute position
  // 3. Move 14 minutes from 1 minute position to 15 minute position
   
  minutes = minutes % 60;

  uint32_t minute_steps = std::ceil((MINUTE_STEPS_PER_REV * minutes) / 60.0f);

  uint32_t next_minute_step = (this->state_.curr_minute_steps + minute_steps) % MINUTE_STEPS_PER_REV;
  uint8_t next_minute = (this->state_.curr_minute + minutes) % 60;

  if (next_minute_step >= (((MINUTE_STEPS_PER_REV * next_minute) / 60.0f) + 1.0f))
  {
    minute_steps = minute_steps - 1; // Adjusting minute steps to avoid overshoot
  }
  
  this->minute_motor.step(minute_steps);
  this->state_.curr_minute_steps = (this->state_.curr_minute_steps + minute_steps) % MINUTE_STEPS_PER_REV;
  this->state_.curr_minute = this->state_.curr_minute + minutes;
}

void System::move_hours(uint16_t hour_units)
{
  hour_units = hour_units % 720;

  uint32_t hour_steps = std::ceil((HOUR_STEPS_PER_REV * hour_units) / 720.0);

  uint32_t next_hour_step = (this->state_.curr_hour_steps + hour_steps) % HOUR_STEPS_PER_REV;
  float next_hour = (this->state_.curr_hour + hour_units) % 720;

  if (next_hour_step >= (((HOUR_STEPS_PER_REV * next_hour) / 720.0f) + 1.0f))
  {
    hour_steps = hour_steps - 1; // Adjusting hour steps to avoid overshoot
  }

  this->hour_motor.step(hour_steps);
  this->state_.curr_hour_steps = (this->state_.curr_hour_steps + hour_steps) % HOUR_STEPS_PER_REV;
  this->state_.curr_hour = this->state_.curr_hour + hour_units;
}