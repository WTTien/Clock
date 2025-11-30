#include "event_processor.hpp"
#include "./system.hpp"

static inline std::string_view trim(std::string_view sv)
{
    // Remove leading and trailing spaces or tabs
    while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t'))
        sv.remove_prefix(1);
    while (!sv.empty() && (sv.back() == ' ' || sv.back() == '\t'))
        sv.remove_suffix(1);
    return sv;
}

bool parse_user_cmd(std::string_view s, UserInput& out)
{
    // Remove CR/LF if needed
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
        s.remove_suffix(1);

    // Expect "[" at start
    if (s.empty() || s.front() != '[')
        return false;
    s.remove_prefix(1);

    // Find closing ']'
    size_t rb = s.find(']');
    if (rb == std::string_view::npos)
        return false;
    //out.type = {TYPE}
    out.type = trim(s.substr(0, rb));
    s.remove_prefix(rb + 1);

    // Skip space(s)
    s = trim(s);

    // Find colon
    size_t colon = s.find(':');
    if (colon == std::string_view::npos)
        return false;
    //out.comp = {COMPONENT}
    out.comp = trim(s.substr(0, colon));

    // Move past ":", trim right side
    s.remove_prefix(colon + 1);
    // out.cmd = {COMMAND}
    out.cmd = trim(s);

    return true;
}

void process_event_queue(System* clock_system)
{
    char event_msg[EVENT_MSG_SIZE];
    if (pop_string_from_event_queue(event_msg)) {
        
        std::string_view msg(event_msg);
        UserInput user_input;

        if(parse_user_cmd(msg, user_input)) {
            if (user_input.type == "LED") {
                if (user_input.comp == "1A") {
                    if (user_input.cmd == "ON") {
                        send_to_print_safe("LED 1A turned ON.\n");
                        clock_system->onboard_led.set_led(true);
                    } 
                    else if (user_input.cmd == "OFF") {
                        send_to_print_safe("LED 1A turned OFF.\n");
                        clock_system->onboard_led.set_led(false);
                    } 
                    else {
                        send_to_print_safe("Unknown command for LED 1A!\n");
                    }
                } else {
                    send_to_print_safe("Unknown component for LED type!\n");
                }
            }
            else if (user_input.type == "POWER") {
                if (user_input.comp == "PICO") {
                    if (user_input.cmd == "REBOOT") {
                        send_to_print_safe("Rebooting to BOOTSEL.\n");
                        sleep_ms(100);
                        reset_usb_boot(0, 0);
                    }
                }
            }
            else {
                send_to_print_safe("Unknown user input!\n");
            }
        } else {
            send_to_print_safe("Failed to parse command!\n");
        }
    }
}