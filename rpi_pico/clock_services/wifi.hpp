#ifndef WIFI_HPP
#define WIFI_HPP

#include "pico/cyw43_arch.h"
#include "./usb.hpp"

#include <optional>
#include <string>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

bool check_wifi_status();
bool connect_to_wifi(const std::optional<std::string>& ssid_opt = std::nullopt, const std::optional<std::string>& pwd_opt  = std::nullopt);
bool disconnect_from_wifi();
bool wifi_is_initialized();

static bool wifi_initialized = false;

#endif // WIFI_HPP