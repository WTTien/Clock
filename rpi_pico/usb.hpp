#ifndef USB_HPP
#define USB_HPP

#include "pico/stdlib.h"
#include "tusb.h"

void tud_cdc_rx_cb(uint8_t itf);

#endif // USB_HPP