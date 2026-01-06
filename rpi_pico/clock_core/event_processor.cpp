#include "event_processor.hpp"
#include "./system.hpp"
#include <string>
#include <stdexcept>


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
        // send_to_print_safe("Processing event: ");
        // send_to_print_safe(event_msg);
        std::string_view msg(event_msg);
        UserInput user_input;

        if(parse_user_cmd(msg, user_input)) {
            if (user_input.type == "LED") {

                if (user_input.cmd == "ON") {
                    if (user_input.comp == "1A") {
                        send_to_print_safe("LED 1A turned ON.\n");
                        clock_system->onboard_led.set_led(true);
                        clock_system->led_1A.set_led(true);
                    }
                    else if (user_input.comp == "1B") {
                        send_to_print_safe("LED 1B turned ON.\n");
                        clock_system->led_1B.set_led(true);
                    }
                    else if (user_input.comp == "2A") {
                        send_to_print_safe("LED 2A turned ON.\n");
                        clock_system->led_2A.set_led(true);
                    }
                    else if (user_input.comp == "2B") {
                        send_to_print_safe("LED 2B turned ON.\n");
                        clock_system->led_2B.set_led(true);
                    }
                    else if (user_input.comp == "1C") {
                        send_to_print_safe("LED 1C turned ON.\n");
                        clock_system->led_1C.set_led(true);
                    }
                    else if (user_input.comp == "2C") {
                        send_to_print_safe("LED 2C turned ON.\n");
                        clock_system->led_2C.set_led(true);
                    }
                    else {
                        send_to_print_safe("Unknown component for LED type!\n");
                    }
                }
                
                else if (user_input.cmd == "OFF") {
                    if (user_input.comp == "1A") {
                        send_to_print_safe("LED 1A turned OFF.\n");
                        clock_system->onboard_led.set_led(false);
                        clock_system->led_1A.set_led(false);
                    }
                    else if (user_input.comp == "1B") {
                        send_to_print_safe("LED 1B turned OFF.\n");
                        clock_system->led_1B.set_led(false);
                    }
                    else if (user_input.comp == "2A") {
                        send_to_print_safe("LED 2A turned OFF.\n");
                        clock_system->led_2A.set_led(false);
                    }
                    else if (user_input.comp == "2B") {
                        send_to_print_safe("LED 2B turned OFF.\n");
                        clock_system->led_2B.set_led(false);
                    }
                    else if (user_input.comp == "1C") {
                        send_to_print_safe("LED 1C turned OFF.\n");
                        clock_system->led_1C.set_led(false);
                    }
                    else if (user_input.comp == "2C") {
                        send_to_print_safe("LED 2C turned OFF.\n");
                        clock_system->led_2C.set_led(false);
                    }
                    else {
                        send_to_print_safe("Unknown component for LED type!\n");
                    }
                }

                else {
                    send_to_print_safe("Unknown command for LED type!\n");
                }
            }

            else if (user_input.type == "MOTOR"){
                int steps;
                if (steps = std::stoi(std::string(user_input.cmd))) {

                    char spin_msg[64];

                    // A minute spins minute_motor about 135-136 steps
                    if (user_input.comp == "MINUTE") {
                        snprintf(spin_msg, sizeof(spin_msg), "Spinning minute for %d steps", steps);
                        send_to_print_safe(spin_msg);
                        clock_system->minute_motor.step(steps);
                    }
                    // An hour spins hour_motor about 339-340 steps, a minute spins hour_motor about 5-6 steps
                    else if (user_input.comp == "HOUR") {
                        snprintf(spin_msg, sizeof(spin_msg), "Spinning hour for %d steps", steps);
                        send_to_print_safe(spin_msg);
                        clock_system->hour_motor.step(steps);
                    }
                    // A date tenth increase spins date_tenth_motor about 1223 steps
                    else if (user_input.comp == "DAYTENTH") {
                        snprintf(spin_msg, sizeof(spin_msg), "Spinning day tenths for %d steps", steps);
                        send_to_print_safe(spin_msg);
                        clock_system->date_tenth_motor.step(steps);
                    }
                    // A date ones increase spins date_ones_motor about 1223 steps
                    else if (user_input.comp == "DAYONES") {
                        snprintf(spin_msg, sizeof(spin_msg), "Spinning day ones for %d steps", steps);
                        send_to_print_safe(spin_msg);
                        clock_system->date_ones_motor.step(steps);
                    }
                    // A month increase spins month_motor about 682 steps
                    else if (user_input.comp == "MONTH") {
                        snprintf(spin_msg, sizeof(spin_msg), "Spinning month for %d steps", steps);
                        send_to_print_safe(spin_msg);
                        clock_system->month_motor.step(steps);
                    }
                    else {
                        send_to_print_safe("Unknown component for MOTOR type!\n");
                    }
                }
                else {
                    send_to_print_safe("Unknown command for MOTOR type!\n");
                }
            }

            else if (user_input.type == "RTC") {
                if (user_input.comp == "READ") {
                    if (user_input.cmd == "NOW") {
                        DateTime dt;
                        if(clock_system->rtc.readTime(dt)) {
                            char time_msg[64];
                            snprintf(time_msg, sizeof(time_msg), "Time: %02d:%02d:%02d, Date: %02d/%02d/%04d\n",
                                        dt.hour, dt.minute, dt.second, dt.date, dt.month, dt.year);
                            send_to_print_safe(time_msg);
                        } else {
                            send_to_print_safe("Failed to read time from RTC!\n");
                        }
                    }
                }
                else if (user_input.comp == "SET") {
                    if (user_input.cmd == "NOW") {
                        DateTime dt;
                        dt.second = 0;  // Set seconds to 0
                        dt.minute = 3;  // Set minutes to 2
                        dt.hour   = 1;  // Set hours to 1 (1:00 AM)
                        dt.day    = 6;   //  Set day of the week to 6 (Friday)
                        dt.date   = 26;  //  Set date to the 25th
                        dt.month  = 12;  // Set month to December
                        dt.year   = 2025; // Set year to 2025
                        if(clock_system->rtc.setTime(dt)) {
                            send_to_print_safe("Time written to RTC:\n");
                        } else {
                            send_to_print_safe("Failed to set time from RTC!\n");
                        }
                    }
                }
                
                // NOTE: MAIN OPERATION!
                else if (user_input.comp == "INT") {
                    if (user_input.cmd == "NOW") {
                        DateTime dt;
                        clock_system->rtc.readTime(dt);
                        
                        snprintf(event_msg, sizeof(event_msg), "RTC Interrupt: Time %02d:%02d:%02d, Date %02d/%02d/%04d\n",
                                    dt.hour, dt.minute, dt.second, dt.date, dt.month, dt.year);
                        send_to_print_safe(event_msg);

                        clock_system->set_to_minute(dt.minute);
                        
                        uint8_t hour_adjusted = dt.hour % 12; // Convert to 12-hour format
                        uint16_t hour_unit = (hour_adjusted * 60) + dt.minute;
                        clock_system->set_to_hour(hour_unit);

                        clock_system->set_to_date_tens(dt.date / 10);

                        clock_system->set_to_date_ones(dt.date % 10);
                        
                        clock_system->set_to_month(dt.month);
                    }
                }
            }

            else if (user_input.type == "TEST") {
                if (user_input.comp == "RTC_INT") {
                    if (user_input.cmd == "MINUTE_HOUR") {
                        uint8_t min_now = clock_system->get_test_minute();
                        uint8_t hour_now = clock_system->get_test_hour();

                        snprintf(event_msg, sizeof(event_msg), "RTC Interrupt TEST: Time %02d:%02d\n",
                                    hour_now, min_now);
                        send_to_print_safe(event_msg);

                        clock_system->set_to_minute(min_now);
                        
                        uint8_t hour_adjusted = hour_now % 12; // Convert to 12-hour format
                        uint16_t hour_unit = (hour_adjusted * 60) + min_now;
                        clock_system->set_to_hour(hour_unit);

                        send_to_print_safe("---TEST---\n");
                    }
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