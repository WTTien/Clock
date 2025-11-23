#include "event_processor.hpp"

static inline std::string_view trim(std::string_view sv)
{
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

    out.type = trim(s.substr(0, rb));
    s.remove_prefix(rb + 1);

    // Skip space(s)
    s = trim(s);

    // Find colon
    size_t colon = s.find(':');
    if (colon == std::string_view::npos)
        return false;

    out.comp = trim(s.substr(0, colon));

    // Move past ":", trim right side → this is command
    s.remove_prefix(colon + 1);
    out.cmd = trim(s);

    return true;
}

void process_event_queue()
{
    char event_msg[EVENT_MSG_SIZE];
    if (pop_string_from_event_queue(event_msg)) {
        
        std::string_view msg(event_msg);
        send_to_print_safe("Processing:\n");
        send_to_print_safe(std::string(msg).c_str());
        send_to_print_safe("\n");
        UserInput user_input;

        if(parse_user_cmd(msg, user_input)) {
            send_to_print_safe("Parsed command:\n");
            send_to_print_safe(" Type: ");
            send_to_print_safe(std::string(user_input.type).c_str());
            send_to_print_safe("\n Component: ");
            send_to_print_safe(std::string(user_input.comp).c_str());
            send_to_print_safe("\n Command: ");
            send_to_print_safe(std::string(user_input.cmd).c_str());
            send_to_print_safe("\n");

            if (user_input.type == "LED") {
                send_to_print_safe("Got LED type!\n");
                if (user_input.comp == "1A") {
                    send_to_print_safe("Got LED 1A component!\n");
                    if (user_input.cmd == "ON") {
                        send_to_print_safe("LED 1A turned ON\n");
                    } 
                    else if (user_input.cmd == "OFF") {
                        send_to_print_safe("LED 1A turned OFF\n");
                    } 
                    else {
                        send_to_print_safe("Unknown command for LED 1A\n");
                    }
                } else {
                    send_to_print_safe("Unknown component for LED type\n");
                }
            } else {
                send_to_print_safe("Unknown command type\n");
            }
        } else {
            send_to_print_safe("Failed to parse command!\n");
        }
        
        
        // event_msg_str.erase(std::remove_if(
        //     event_msg_str.begin(), event_msg_str.end(),
        //     [](unsigned char c){ return c == '\r' || c == '\n'; }), 
        //     event_msg_str.end());
        // send_to_print_safe("Processing event: ");
        // send_to_print_safe(event_msg);
        // send_to_print_safe("\n");
    }
}