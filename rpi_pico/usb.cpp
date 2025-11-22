#include "usb.hpp"

char usb_queue[USB_QUEUE_SIZE];
volatile uint32_t usb_q_head = 0;
volatile uint32_t usb_q_tail = 0;
mutex_t usb_queue_mutex;

void send_to_print_safe(const char* str)
{
    if (!str) return;

    mutex_enter_blocking(&usb_queue_mutex);

    for (int i = 0; str[i] != '\0'; i++) {
        uint32_t next = (usb_q_head + 1) % USB_QUEUE_SIZE;
        if (next == usb_q_tail) break;  // buffer full, drop
        usb_queue[usb_q_head] = str[i];
        usb_q_head = next;
    }

    mutex_exit(&usb_queue_mutex);
}

void usb_write()
{
    // Drain queue → write to USB
    mutex_enter_blocking(&usb_queue_mutex);

    while (usb_q_tail != usb_q_head && tud_cdc_available() >= 0) {
        uint8_t c = usb_queue[usb_q_tail];
        usb_q_tail = (usb_q_tail + 1) % USB_QUEUE_SIZE;

        tud_cdc_write_char(c);
    }

    mutex_exit(&usb_queue_mutex);
    
    tud_cdc_write_flush();
}

void tud_cdc_rx_cb(uint8_t itf)
{
    char buf[64];
    uint32_t n = tud_cdc_read(buf, sizeof(buf));
    buf[n]  = '\0';

    send_to_print_safe("Hello Hello\n");
}
