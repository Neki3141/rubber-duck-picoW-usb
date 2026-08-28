cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rubber_usb C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Shared source files
set(SOURCES
    main.cpp
    key_translate.cpp
    key_sent.cpp
)

# Shared TinyUSB configuration definitions
set(USB_DEFINITIONS
    CFG_TUD_ENABLED=1
    CFG_TUD_HID=1
    CFG_TUD_CDC=1
    CFG_TUD_CDC_EP_BUFSIZE=64
    CFG_TUD_CDC_RX_BUFSIZE=256
    CFG_TUD_CDC_TX_BUFSIZE=256
    CFG_TUSB_RHPORT0_MODE=OPT_MODE_DEVICE
)

# Shared library dependencies
set(PROJECT_LIBS
    pico_stdlib
    tinyusb_device
    hardware_gpio
)

# ==============================================================================
# 1. PRODUCTION TARGET (rubber_usb)
# ==============================================================================
add_executable(rubber_usb ${SOURCES})

pico_set_program_name(rubber_usb "rubber_usb")
pico_set_program_version(rubber_usb "0.1")

pico_enable_stdio_uart(rubber_usb 0)
pico_enable_stdio_usb(rubber_usb 1)

target_include_directories(rubber_usb PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(rubber_usb PRIVATE ${USB_DEFINITIONS})
target_link_libraries(rubber_usb ${PROJECT_LIBS})

pico_add_extra_outputs(rubber_usb)

# ==============================================================================
# 2. DEBUG TARGET (rubber_usb_debug)
# ==============================================================================
add_executable(rubber_usb_debug ${SOURCES})

pico_set_program_name(rubber_usb_debug "rubber_usb_debug")
pico_set_program_version(rubber_usb_debug "0.1")

pico_enable_stdio_uart(rubber_usb_debug 0)
pico_enable_stdio_usb(rubber_usb_debug 1)

target_include_directories(rubber_usb_debug PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Pass DEBUG_BUILD=1 macro along with TinyUSB definitions
target_compile_definitions(rubber_usb_debug PRIVATE 
    ${USB_DEFINITIONS} 
    DEBUG_BUILD=1
)

target_link_libraries(rubber_usb_debug ${PROJECT_LIBS})

pico_add_extra_outputs(rubber_usb_debug)
