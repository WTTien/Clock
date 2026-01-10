# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(clock C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(clock
    ${CMAKE_CURRENT_LIST_DIR}/clock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_drivers/on_board_led.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_drivers/led.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_drivers/stepper_motor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_drivers/gpio_stepper_motor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_drivers/pcf_stepper_motor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_drivers/real_time_clock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_services/usb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_services/wifi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_core/event_processor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_core/system.cpp
    ${CMAKE_CURRENT_LIST_DIR}/clock_services/config/usb_device.c
)

target_include_directories(clock PRIVATE ${CMAKE_CURRENT_LIST_DIR}/clock_services/config)

# pull in common dependencies
target_link_libraries(clock pico_stdlib pico_multicore hardware_i2c tinyusb_device tinyusb_board)


if (PICO_CYW43_SUPPORTED)
    target_link_libraries(clock pico_cyw43_arch_lwip_threadsafe_background pico_lwip_sntp)
endif()

pico_enable_stdio_usb(clock 1)
pico_enable_stdio_uart(clock 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(clock)

# add url via pico_set_program_url
