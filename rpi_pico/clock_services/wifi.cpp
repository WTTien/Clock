#include "wifi.hpp"

void display_wifi_status()
{
    int status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    switch (status) {
        case CYW43_LINK_JOIN:
        case CYW43_LINK_NOIP:
        case CYW43_LINK_UP:
            send_to_print_safe("[WiFi] Connected to WiFi network.\n");
            break;
        default:
            send_to_print_safe("[WiFi] Disconnected.\n");
            break;
    }
}