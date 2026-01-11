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
      led_2A(7), // TODO: update!
      led_1B(8), // TODO: update!
      led_2B(9), // TODO: update!
      led_1C(10), // TODO: update!
      led_2C(11), // TODO: update!
      hour_motor(20, 21, 22, 26),
      minute_motor(16, 17, 18, 19),
      date_tenth_motor(
          i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
          0x20, // I2C address
          0, 1, 2, 3 // Pins on the expander
      ),
      date_ones_motor(
          i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
          0x20, // I2C address
          4, 5, 6, 7 // Pins on the expander
      ),
      month_motor(
          i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
          0x20, // I2C address
          8, 9, 10, 11 // Pins on the expander
      ),
      // date_motor(
      //     i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
      //     0x21, // I2C address
      //     0, 1, 2, 3 // Pins on the expander
      // ),
      rtc(
        i2c1, // TODO: Find a way to link to clock.cpp definition of I2C_PORT
        0x68, // I2C address
        12,   // Reset pin
        13    // Interrupt pin
      )
{
  state_.curr_minute_steps = 0;
  state_.curr_hour_steps = 0;
  state_.curr_minute = 0;
  state_.curr_hour = 0;
  state_.curr_date_ones = 0;
  state_.curr_date_tens = 0;
  state_.curr_month = 12;
}

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

  // WiFi and SNTP
#ifdef CYW43_WL_GPIO_LED_PIN
  connect_to_wifi();
  try_sync_system_time_sntp();
  disconnect_from_wifi();
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
  // rc = this->onboard_led.init();
  hard_assert(rc == PICO_OK);
  // this->onboard_led.set_led(true);

  // Jellybean LED
  rc = this->led_1A.init();
  hard_assert(rc == PICO_OK);

  rc = this->led_2A.init();
  hard_assert(rc == PICO_OK);

  rc = this->led_1B.init();
  hard_assert(rc == PICO_OK);

  rc = this->led_2B.init();
  hard_assert(rc == PICO_OK);

  rc = this->led_1C.init();
  hard_assert(rc == PICO_OK);

  rc = this->led_2C.init();
  hard_assert(rc == PICO_OK);
 
  // Hour Motor
  rc = this->hour_motor.init();
  hard_assert(rc == PICO_OK);
  this->hour_motor.setDirection(true);

  // Minute Motor
  rc = this->minute_motor.init();
  hard_assert(rc == PICO_OK);
  this->minute_motor.setDirection(false);

  // Date Tenth Motor
  rc = this->date_tenth_motor.init();
  hard_assert(rc == PICO_OK);
  this->date_tenth_motor.setDirection(false);

  // Date Ones Motor
  rc = this->date_ones_motor.init();
  hard_assert(rc == PICO_OK);
  this->date_ones_motor.setDirection(true);

  // Month Motor
  rc = this->month_motor.init();
  hard_assert(rc == PICO_OK);
  this->month_motor.setDirection(false);

  // // Date Motor
  // rc = this->date_motor.init();
  // hard_assert(rc == PICO_OK);
  // this->date_motor.setDirection(true);

  // Real Time Clock
  rc = this->rtc.init();
  hard_assert(rc == PICO_OK);

  send_to_print_safe("Starting programme!\n");

  /// TEST UTILITIES ///
  // this->test_minute = 0;
  // this->test_hour = 0;
  /// TEST UTILITIES ///

  return PICO_OK;
}

void System::run()
{
  while(true)
  {
    /// ONE DAY THESE WILL BE GONE ///
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
    /// ONE DAY THESE WILL BE GONE ///
    
    /// TEST UTILITIES ///
    // this->test_minute = (this->test_minute + 1) % 60;
    // if (this->test_minute == 0) {
    //     this->test_hour = (this->test_hour + 1) % 24;
    // }
    // this->set_test_minute_hour(this->test_minute, this->test_hour);
    // char event_msg[64] = "[TEST] RTC_INT: MINUTE_HOUR\n";
    // push_string_into_event_queue(event_msg);
    /// TEST UTILITIES ///

    process_event_queue(this);
    sleep_ms(1500);
  }
}

void System::set_to_minute(uint8_t minute)
{
  int8_t move_minute = (minute - (this->state_.curr_minute % 60) + 60) % 60;
  // snprintf(event_msg, sizeof(event_msg), "Moving minutes by %d\n", move_minute);
  // send_to_print_safe(event_msg);

  // 3 cases:
  // 1. Move 1 minute
  // 2. Move 15 minutes from 0 minute position to 15 minute position
  // 3. Move 14 minutes from 1 minute position to 15 minute position
  move_minute = move_minute % 60;

  uint32_t minute_steps = std::ceil((MINUTE_STEPS_PER_REV * move_minute) / 60.0f);

  uint32_t next_minute_step = (this->state_.curr_minute_steps + minute_steps) % MINUTE_STEPS_PER_REV;
  uint8_t next_minute = (this->state_.curr_minute + move_minute) % 60;

  if (next_minute_step >= (((MINUTE_STEPS_PER_REV * next_minute) / 60.0f) + 1.0f))
  {
    minute_steps = minute_steps - 1; // Adjusting minute steps to avoid overshoot
  }
  
  this->minute_motor.step(minute_steps);
  this->state_.curr_minute_steps = (this->state_.curr_minute_steps + minute_steps) % MINUTE_STEPS_PER_REV;
  this->state_.curr_minute = (this->state_.curr_minute + move_minute) % 60;
  // char state_msg[64];
  // snprintf(state_msg, sizeof(state_msg), "Current minute set to %d\n", this->state_.curr_minute);
  // send_to_print_safe(state_msg);
}

void System::set_to_hour(uint16_t hour_unit)
{
  int16_t move_hour = (hour_unit - (this->state_.curr_hour % 720) + 720) % 720;
  // snprintf(event_msg, sizeof(event_msg), "Moving hours by %d\n", move_hour);
  // send_to_print_safe(event_msg);

  move_hour = move_hour % 720;

  uint32_t hour_steps = std::ceil((HOUR_STEPS_PER_REV * move_hour) / 720.0);

  uint32_t next_hour_step = (this->state_.curr_hour_steps + hour_steps) % HOUR_STEPS_PER_REV;
  float next_hour = (this->state_.curr_hour + move_hour) % 720;

  if (next_hour_step >= (((HOUR_STEPS_PER_REV * next_hour) / 720.0f) + 1.0f))
  {
    hour_steps = hour_steps - 1; // Adjusting hour steps to avoid overshoot
  }

  this->hour_motor.step(hour_steps);
  this->state_.curr_hour_steps = (this->state_.curr_hour_steps + hour_steps) % HOUR_STEPS_PER_REV;
  this->state_.curr_hour = (this->state_.curr_hour + move_hour) % 720;
  // char state_msg[64];
  // snprintf(state_msg, sizeof(state_msg), "Current hour set to %d\n", this->state_.curr_hour);
  // send_to_print_safe(state_msg);
}

void System::set_to_date_tens(uint8_t date_tens)
{
  int8_t move_date_tens = date_tens - (this->state_.curr_date_tens % 4);
  // snprintf(event_msg, sizeof(event_msg), "Moving date tens by %d\n", move_date_tens);
  // send_to_print_safe(event_msg);

  move_date_tens = move_date_tens % 4; // Only 0-3 valid

  uint32_t date_tens_steps = move_date_tens * DATE_TENS_STEPS; // 1223 steps per tens increment

  this->date_tenth_motor.step(date_tens_steps);
  this->state_.curr_date_tens = (this->state_.curr_date_tens + move_date_tens) % 4;
  // char state_msg[64];
  // snprintf(state_msg, sizeof(state_msg), "Current date_tenth set to %d\n", this->state_.curr_date_tens);
  // send_to_print_safe(state_msg);
}

void System::set_to_date_ones(uint8_t date_ones)
{
  int8_t move_date_ones = date_ones - (this->state_.curr_date_ones % 10);
  // snprintf(event_msg, sizeof(event_msg), "Moving date ones by %d\n", move_date_ones);
  // send_to_print_safe(event_msg);

  move_date_ones = move_date_ones % 10; // Only 0-9 valid

  uint32_t date_ones_steps = move_date_ones * DATE_ONES_STEPS; // 1223 steps per ones increment

  this->date_ones_motor.step(date_ones_steps);
  this->state_.curr_date_ones = (this->state_.curr_date_ones + move_date_ones) % 10;
  // char state_msg[64];
  // snprintf(state_msg, sizeof(state_msg), "Current date_ones set to %d\n", this->state_.curr_date_ones);
  // send_to_print_safe(state_msg);
}

void System::set_to_month(uint8_t month)
{
  int8_t move_month = (month - (this->state_.curr_month % 12) + 12) % 12;
  // snprintf(event_msg, sizeof(event_msg), "Moving month by %d\n", move_month);
  // send_to_print_safe(event_msg);

  move_month = move_month % 12; // Only 0-11 valid

  uint32_t month_steps = move_month * MONTH_STEPS; // 682 steps per month increment

  this->month_motor.step(month_steps);
  this->state_.curr_month = (this->state_.curr_month + move_month) % 12;
  // char state_msg[64];
  // snprintf(state_msg, sizeof(state_msg), "Current month set to %d\n", this->state_.curr_month);
  // send_to_print_safe(state_msg);
}


/// TEST UTILITIES ///
void System::set_test_minute_hour(uint8_t minutes, uint8_t hours)
{
    this->test_minute = minutes;
    this->test_hour = hours;
}

uint8_t System::get_test_minute()
{
    return this->test_minute;
}

uint8_t System::get_test_hour()
{
    return this->test_hour;
}
/// TEST UTILITIES ///

bool System::try_sync_system_time_sntp()
{
  if(check_wifi_status())
  {
    //Reset system time
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    
    // Wait until time is valid
    absolute_time_t timeout = make_timeout_time_ms(30000); // 30 seconds timeout
    time_t now = 0;
    while ((now = time(NULL)) < 100000) {
      sleep_ms(200);  // small delay

      if (absolute_time_diff_us(get_absolute_time(), timeout) < 0) {
        // Timeout after 30 seconds
        send_to_print_safe("SNTP time sync timeout!\n");
        sntp_stop();
        return false;
      }
    }

    // Stop after synced
    sntp_stop();
    return true;
  }
  else {
    return false; // No WiFi connection
  }
}