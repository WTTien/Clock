#include "clock_core/system.hpp"

int main() {
    System system;
    if (!system.init()) {
        return -1;
    }
    system.run();
    return 0;
}

// int main() {
//     cyw43_arch_enable_sta_mode();
    
//     rc = cyw43_arch_wifi_connect_timeout_ms(
//         WIFI_SSID,
//         WIFI_PASSWORD,
//         CYW43_AUTH_WPA2_AES_PSK,
//         60000
//     );
//     hard_assert(rc == PICO_OK);
    
//     while (true)
//     {
//         int status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
//         switch (status) {
//             case CYW43_LINK_JOIN:
//             case CYW43_LINK_NOIP:
//             case CYW43_LINK_UP:
//                 // send_to_print_safe("[WiFi] Connected to WiFi network.\n");
//                 printf("[WiFi] Connected to WiFi network.\n");
//                 break;
//             default:
//                 // send_to_print_safe("[WiFi] Disconnected.\n");
//                 printf("[WiFi] Disconnected.\n");
//                 break;
//         }
//     }
// }