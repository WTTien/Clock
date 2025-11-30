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
  cyw43_arch_enable_sta_mode();
  int rc = cyw43_arch_wifi_connect_timeout_ms(
      WIFI_SSID,
      WIFI_PASSWORD,
      CYW43_AUTH_WPA2_AES_PSK,
      60000
  );
  hard_assert(rc == PICO_OK);
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
  rc = this->onboard_led.init();
  hard_assert(rc == PICO_OK);

  // Jellybean LED
  rc = this->led_1A.init();
  hard_assert(rc == PICO_OK);

  // OLED Motor
  rc = this->OLED_motor.init();
  hard_assert(rc == PICO_OK);

  // hour_minute_motor
  rc = this->hour_minute_motor.init();
  hard_assert(rc == PICO_OK);

  // date_motor
  rc = this->date_motor.init();
  hard_assert(rc == PICO_OK);

  // Real Time Clock
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

    // this->OLED_motor.step(256); // Clockwise
    // this->hour_minute_motor.step(256); // Clockwise
    // this->date_motor.step(256); // Clockwise

    // send_to_print_safe("Hello World!\n");
    // display_wifi_status();
    process_event_queue(this);
    sleep_ms(50);
  }
}
