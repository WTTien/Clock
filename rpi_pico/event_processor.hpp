#include "usb.hpp"
#include "led.hpp" 
#include <string>
#include <string_view>

// From PC we send inputs in the format: [{TYPE}] {COMPONENT}: {COMMAND}
struct UserInput {
    std::string_view type;
    std::string_view comp;
    std::string_view cmd;
};

void process_event_queue();