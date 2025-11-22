#include "usb.hpp"

char usb_queue[USB_QUEUE_SIZE];
volatile uint32_t usb_q_head = 0;
volatile uint32_t usb_q_tail = 0;

static inline bool rb_push(char c) 
{
    uint32_t next = (usb_q_head + 1) % USB_QUEUE_SIZE;
    if (next == usb_q_tail) return false;  // buffer full

    usb_queue[usb_q_head] = c;
    __atomic_store_n(&usb_q_head, next, __ATOMIC_RELEASE);
    return true;
}

static inline bool rb_pop(char* c) 
{
    uint32_t tail = __atomic_load_n(&usb_q_tail, __ATOMIC_ACQUIRE);
    if (tail == usb_q_head) return false;  // buffer empty

    *c = usb_queue[tail];
    __atomic_store_n(&usb_q_tail, (tail + 1) % USB_QUEUE_SIZE, __ATOMIC_RELEASE);
    return true;
}

void send_to_print_safe(const char* s)
{
    while (*s) {
        rb_push(*s++);
    }
}

void usb_write()
{
    char c;
    while(tud_cdc_connected() && tud_cdc_write_available() && rb_pop(&c)) {
        tud_cdc_write_char(c);
    }
        
    tud_cdc_write_flush();
}

void tud_cdc_rx_cb(uint8_t itf)
{
    char buf[64];
    uint32_t n = tud_cdc_read(buf, sizeof(buf));
    buf[n]  = '\0';

    send_to_print_safe("Hello Hello\n");
}
