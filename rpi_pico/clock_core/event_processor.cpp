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

bool parse_datetime(const char* str, DateTime& dt)
{
    uint8_t hour, minute, second;
    uint8_t day, date, month, year;

    int matched = sscanf(
        str,
        "%d-%d-%d_%d_%d-%d-%d",
        &hour,
        &minute,
        &second,
        &day,
        &date,
        &month,
        &year
    );

    if (matched != 7) {
        return false; // format error
    }

    // Optional: range validation
    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;
    if (second < 0 || second > 59) return false;
    if (day < 0 || day > 6) return false;
    if (date < 1 || date > 31) return false;
    if (month < 1 || month > 12) return false;
    if (year < 2000 || year > 3000) return false;

    dt.hour   = hour;
    dt.minute = minute;
    dt.second = second;
    dt.day    = day;
    dt.date   = date;
    dt.month  = month;
    dt.year   = year;

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
                            snprintf(time_msg, sizeof(time_msg), "Time: %02d:%02d:%02d, Date: %02d/%02d/%04d, Day: %d\n",
                                        dt.hour, dt.minute, dt.second, dt.date, dt.month, dt.year, dt.day);
                            send_to_print_safe(time_msg);
                        } else {
                            send_to_print_safe("Failed to read time from RTC!\n");
                        }
                    }
                    else {
                        send_to_print_safe("I got [RTC] READ but I cant understand data/command given :(\n");
                    }
                }
                else if (user_input.comp == "SET_MANUAL") {
                    DateTime dt;
                    std::string cmd_str(user_input.cmd);
                    if (parse_datetime(cmd_str.c_str(), dt)) {
                        if (clock_system->rtc.setTime(dt)) {
                            send_to_print_safe("Manual time written to RTC:\n");
                        } else {
                            send_to_print_safe("Failed to set manual time to RTC!\n");
                        }
                    }
                    else {
                        send_to_print_safe("I got [RTC] SET MANUAL but I cant understand data/command given :(\n");
                    }
                }
                else if (user_input.comp == "SET_NTP") {
                    if (user_input.cmd == "NOW") {
                        if (clock_system->rtc.setTimeFromNTP()) {
                            send_to_print_safe("RTC time set from NTP successfully.\n");
                        } else {
                            send_to_print_safe("Failed to set RTC time from NTP!\n");
                        }
                    }
                    else {
                        send_to_print_safe("I got [RTC] SET NTP but I cant understand data/command given :(\n");
                    }
                }
                else if (user_input.comp == "CHECK_SYS_TIME") {
                    if (user_input.cmd == "NOW") {
                        time_t now = time(NULL);
                        now += 8 * 3600;  // Adjust for timezone (e.g., UTC+8)

                        struct tm *tm = gmtime(&now);

                        char sys_time_msg[64];
                        snprintf(sys_time_msg, sizeof(sys_time_msg), "System Time: %02d:%02d:%02d, Date: %02d/%02d/%04d, Weekday: %d\n",
                                    tm->tm_hour, tm->tm_min, tm->tm_sec,
                                    tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_wday);
                        send_to_print_safe(sys_time_msg);
                    }
                    else {
                        send_to_print_safe("I got [RTC] CHECK SYS TIME but I cant understand data/command given :(\n");
                    }
                }
                else if (user_input.comp == "SYNC_SYS_TIME") {
                    if (user_input.cmd == "NOW") {
                        if (clock_system->try_sync_system_time_sntp()) {
                            send_to_print_safe("Pico system time synced from NTP successfully.\n");
                        } else {
                            send_to_print_safe("Failed to sync Pico system time from NTP!\n");
                        }
                    }
                    else {
                        send_to_print_safe("I got [RTC] SYNC SYS TIME but I cant understand data/command given :(\n");
                    }
                }
                
                // NOTE: MAIN OPERATION!
                else if (user_input.comp == "INT") {
                    if (user_input.cmd == "NOW") {
                        if (!clock_system->debug_mode) {
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
                    else {
                        send_to_print_safe("I got [RTC] INT but I cant understand data/command given :(\n");
                    }
                }
            
                else {
                    send_to_print_safe("I got some instruction on [RTC] but not too sure what is it about :(\n");
                }
            }

            else if (user_input.type == "WIFI") {
                if (user_input.comp == "STATUS") {
                    if (user_input.cmd == "NOW") {
                        if (check_wifi_status()) {
                            send_to_print_safe("WiFi is connected.\n");
                        } else {
                            send_to_print_safe("WiFi is disconnected.\n");
                        }
                    }
                    else {
                        send_to_print_safe("I got [WIFI] STATUS but I cant understand data/command given :(\n");
                    }
                }
                else if (user_input.comp == "CONNECT") {
                    if (user_input.cmd == "DEFAULT") { // No SSID/PASSWORD provided, use default
                        if (connect_to_wifi()) {
                            send_to_print_safe("Connected to WiFi successfully.\n");
                        } else {
                            send_to_print_safe("Failed to connect to WiFi!\n");
                        }
                    }
                    else if (user_input.cmd.find('_') != std::string_view::npos) { // SSID and PASSWORD provided
                        size_t underscore_pos = user_input.cmd.find('_');
                        std::string ssid(user_input.cmd.substr(0, underscore_pos));
                        std::string password(user_input.cmd.substr(underscore_pos + 1));
                        if (connect_to_wifi(ssid, password)) {
                            send_to_print_safe("Connected to WiFi successfully.\n");
                        } else {
                            send_to_print_safe("Failed to connect to WiFi!\n");
                        }
                    }
                    else {
                        send_to_print_safe("I got [WIFI] CONNECT but I cant understand data/command given :(\n");
                    }
                }
                else if (user_input.comp == "DISCONNECT") {
                    if (user_input.cmd == "NOW") {
                        if (disconnect_from_wifi()) {
                            send_to_print_safe("Disconnected from WiFi successfully.\n");
                        } else {
                            send_to_print_safe("Failed to disconnect from WiFi!\n");
                        }
                    }
                    else {
                        send_to_print_safe("I got [WIFI] DISCONNECT but I cant understand data/command given :(\n");
                    }
                }
                else {
                    send_to_print_safe("I got some instruction on [WIFI] but not too sure what is it about :(\n");
                }
            }

            else if (user_input.type == "DEBUG") {
                if (user_input.comp == "MODE") {
                    if (user_input.cmd == "ON") {
                        clock_system->debug_mode = true;
                        send_to_print_safe("Debug mode enabled.\n");
                    }
                    else if (user_input.cmd == "OFF") {
                        clock_system->debug_mode = false;
                        send_to_print_safe("Debug mode disabled.\n");
                    }
                    else {
                        send_to_print_safe("Unknown command for DEBUG MODE!\n");
                    }
                }
                else {
                    send_to_print_safe("Unknown component for DEBUG type!\n");
                }
            }

            else if (user_input.type == "TEST") {
                if (user_input.comp == "RTC_INT") {
                    if (user_input.cmd == "START") {
                        if (!clock_system->test_rtc_int_start) {
                            clock_system->test_rtc_int_start = true;
                            clock_system->set_test_time(0, 0, 0, 0);
                            add_repeating_timer_ms(1000, System::static_test_rtc_int_timer_callback, nullptr, &clock_system->test_rtc_int_timer);
                            send_to_print_safe("RTC Interrupt TEST started.\n");
                        }
                        else {
                            uint8_t min_now = clock_system->get_test_minute();
                            uint8_t hour_now = clock_system->get_test_hour();
                            uint8_t date_now = clock_system->get_test_date();
                            uint8_t month_now = clock_system->get_test_month();

                            snprintf(event_msg, sizeof(event_msg), "RTC Interrupt TEST: Time %02d:%02d, Date %02d-%02d\n",
                                        hour_now, min_now, date_now, month_now);
                            send_to_print_safe(event_msg);

                            clock_system->set_to_minute(min_now);
                            
                            uint8_t hour_adjusted = hour_now % 12; // Convert to 12-hour format
                            uint16_t hour_unit = (hour_adjusted * 60) + min_now;
                            clock_system->set_to_hour(hour_unit);

                            clock_system->set_to_date_tens(date_now / 10);

                            clock_system->set_to_date_ones(date_now % 10);

                            clock_system->set_to_month(month_now);

                            send_to_print_safe("---TEST---\n");    
                        }
                    }
                }

                else if (user_input.cmd == "STOP") {
                    if (clock_system->test_rtc_int_start) {
                        clock_system->test_rtc_int_start = false;
                        clock_system->set_test_time(0, 0, 0, 0);
                        cancel_repeating_timer(&clock_system->test_rtc_int_timer);
                        send_to_print_safe("RTC Interrupt TEST stopped.\n");
                    }
                }

                else {
                    send_to_print_safe("I got [TEST] RTC_INT but not too sure what it is about :(\n");
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