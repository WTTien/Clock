#include "wifi.hpp"

bool check_wifi_status()
{
    if (!wifi_initialized) {
        return false; // WiFi not initialized
    }

    int status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    switch (status) {
        case CYW43_LINK_JOIN:
        case CYW43_LINK_NOIP:
        case CYW43_LINK_UP:
            return true;
            // send_to_print_safe("[WiFi] Connected to WiFi network.\n");
            // break;
        default:
            return false;
            // send_to_print_safe("[WiFi] Disconnected.\n");
            // break;
    }
}

bool connect_to_wifi(
    const std::optional<std::string>& ssid_opt,
    const std::optional<std::string>& pwd_opt
)
{
    if (!wifi_initialized) {
        if (cyw43_arch_init() != PICO_OK) {
            return false;
        }
        cyw43_arch_enable_sta_mode();
        wifi_initialized = true;
    }
    const char* ssid = ssid_opt ? ssid_opt->c_str() : WIFI_SSID;
    const char* password = pwd_opt ? pwd_opt->c_str() : WIFI_PASSWORD;
    int rc = cyw43_arch_wifi_connect_timeout_ms(
        ssid,
        password,
        CYW43_AUTH_WPA2_AES_PSK,
        60000
    );

    if(rc == PICO_OK) {
        return true;
    }
    return false;
}

bool disconnect_from_wifi()
{
    if(!wifi_initialized) {
        return true; // Already disconnected
    }
    cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
    cyw43_arch_deinit();
    wifi_initialized = false;
    return true;
}

bool wifi_is_initialized()
{
    return wifi_initialized;
} // Is this needed??