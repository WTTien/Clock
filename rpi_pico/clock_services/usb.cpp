#include "usb.hpp"

////////////////////////////////////////////////////////////////////////////
// First section : This is used when we send serial data FROM Pico TO PC. //
////////////////////////////////////////////////////////////////////////////
char usb_queue[USB_QUEUE_SIZE];
volatile uint32_t usb_q_head = 0;
volatile uint32_t usb_q_tail = 0;

static inline bool push_char_into_usb_queue(char c) 
{
    uint32_t next = (usb_q_head + 1) % USB_QUEUE_SIZE;
    if (next == usb_q_tail) return false;  // buffer full

    usb_queue[usb_q_head] = c;
    __atomic_store_n(&usb_q_head, next, __ATOMIC_RELEASE);
    return true;
}

static inline bool pop_char_from_usb_queue(char* c) 
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
        push_char_into_usb_queue(*s++);
    }
}

void usb_write()
{
    char c;
    while(tud_cdc_connected() && tud_cdc_write_available() && pop_char_from_usb_queue(&c)) {
        tud_cdc_write_char(c);
    }
        
    tud_cdc_write_flush();
}


/////////////////////////////////////////////////////////////////////////////
// Second section : This is used when we send serial data FROM PC To Pico. //
/////////////////////////////////////////////////////////////////////////////
char event_queue[EVENT_QUEUE_SIZE][EVENT_MSG_SIZE];
volatile uint32_t event_q_head = 0;
volatile uint32_t event_q_tail = 0;

bool push_string_into_event_queue(const char* msg)
{
    uint32_t head = __atomic_load_n(&event_q_head, __ATOMIC_RELAXED);
    uint32_t tail = __atomic_load_n(&event_q_tail, __ATOMIC_ACQUIRE);
    uint32_t next = (head + 1) % EVENT_QUEUE_SIZE;
    if (next == tail) return false;  // buffer full

    strncpy(event_queue[event_q_head], msg, EVENT_MSG_SIZE - 1);
    event_queue[event_q_head][EVENT_MSG_SIZE - 1] = '\0'; // Ensure null-termination

    __atomic_store_n(&event_q_head, next, __ATOMIC_RELEASE);
    return true;
}

bool pop_string_from_event_queue(char* out)
{
    uint32_t tail = __atomic_load_n(&event_q_tail, __ATOMIC_RELAXED);
    uint32_t head = __atomic_load_n(&event_q_head, __ATOMIC_ACQUIRE);
    if (tail == head) return false;  // buffer empty

    strncpy(out, event_queue[tail], EVENT_MSG_SIZE);
    __atomic_store_n(&event_q_tail, (tail + 1) % EVENT_QUEUE_SIZE, __ATOMIC_RELEASE);
    return true;
}

void tud_cdc_rx_cb(uint8_t itf)
{
    char buf[64];
    uint32_t n = tud_cdc_read(buf, sizeof(buf));
    buf[n]  = '\0';

    send_to_print_safe("Received: ");
    send_to_print_safe(buf);
    send_to_print_safe("\n");
    if (!push_string_into_event_queue(buf)) {
        send_to_print_safe("Event queue full, message dropped.\n");
    }
    else {
        // Notify the other core that new data is available
        multicore_fifo_push_blocking(1);
    }
}
