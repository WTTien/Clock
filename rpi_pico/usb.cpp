#include "usb.hpp"

void tud_cdc_rx_cb(uint8_t itf)
{
    char buf[64];
    uint32_t n = tud_cdc_read(buf, sizeof(buf));
    buf[n]  = '\0';

    printf("Hello Hello\n");
}
