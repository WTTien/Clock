#ifndef USB_HPP
#define USB_HPP

#include "pico/stdlib.h"
#include "tusb.h"

#define USB_QUEUE_SIZE 4096
extern char usb_queue[USB_QUEUE_SIZE];
extern volatile uint32_t usb_q_head;
extern volatile uint32_t usb_q_tail;

// send_to_print_safe and usb_write function is used to synchronise USB serial printing across multiple cores.
// Aims to have all USB operation handled in Core 1.
void send_to_print_safe(const char* str); //This function is used to set what to print over USB serial. The string is added to usb_queue.
void usb_write(); //This function writes data from usb_queue to USB serial. Only calls in Core 1 main.

// extern mutex_t print_mutex;
// void safe_print(const char* str); //This function is used to print a string over USB serial in a thread-safe manner.

void tud_cdc_rx_cb(uint8_t itf);

#endif // USB_HPP