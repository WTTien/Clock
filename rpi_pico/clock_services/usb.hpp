#ifndef USB_HPP
#define USB_HPP

#include "pico/stdlib.h"
#include "tusb.h"


// First section : This is used when we send serial data FROM Pico TO PC.
#define USB_QUEUE_SIZE 4096
extern char usb_queue[USB_QUEUE_SIZE];
extern volatile uint32_t usb_q_head;
extern volatile uint32_t usb_q_tail;
// send_to_print_safe and usb_write function is used to synchronise USB serial printing across multiple cores.
// Aims to have all USB operation handled in Core 1.
void send_to_print_safe(const char* str); //This function is used to set what to print over USB serial. The string (from Core 0 and Core 1) is added to usb_queue.
void usb_write(); //This function writes data from usb_queue to USB serial. Only calls in Core 1 main.

// Second section : This is used when we receive serial data FROM PC TO Pico.
#define EVENT_MSG_SIZE 128
#define EVENT_QUEUE_SIZE 32
extern char event_queue[EVENT_QUEUE_SIZE][EVENT_MSG_SIZE];
extern volatile uint32_t event_q_head;
extern volatile uint32_t event_q_tail;
// We will have a event processing function in other parts of the codebase, there is quite a few process to be done.
bool pop_string_from_event_queue(char* out); //This function is used to get the next event string from event_queue.
void tud_cdc_rx_cb(uint8_t itf);

bool push_string_into_event_queue(const char* msg);

#endif // USB_HPP