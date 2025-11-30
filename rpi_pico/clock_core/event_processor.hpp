#ifndef EVENT_PROCESSOR_HPP
#define EVENT_PROCESSOR_HPP

#include "../clock_services/usb.hpp"
#include "pico/bootrom.h"
#include <string>
#include <string_view>

// From PC we send inputs in the format: [{TYPE}] {COMPONENT}: {COMMAND}
struct UserInput {
    std::string_view type;
    std::string_view comp;
    std::string_view cmd;
};

#endif // EVENT_PROCESSOR_HPP