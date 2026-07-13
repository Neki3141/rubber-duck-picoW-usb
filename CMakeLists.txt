#from now I using alt h to open terminal but I accidently press shift tab and it delete my terminal I alt h again now it split to 3 part Generated Cmake Pico project file

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
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rubber_usb C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(rubber_usb main.cpp key_translate.cpp key_sent.cpp)

pico_set_program_name(rubber_usb "rubber_usb")
pico_set_program_version(rubber_usb "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(rubber_usb 0)
pico_enable_stdio_usb(rubber_usb 1)

# Add the standard library to the build
target_link_libraries(rubber_usb
        pico_stdlib
        tinyusb_device
        hardware_gpio)

target_compile_definitions(rubber_usb PRIVATE
    CFG_TUD_ENABLED=1
    CFG_TUD_HID=1
    CFG_TUD_CDC=1
    CFG_TUD_CDC_EP_BUFSIZE=64
    CFG_TUD_CDC_RX_BUFSIZE=256
    CFG_TUD_CDC_TX_BUFSIZE=256
    CFG_TUSB_RHPORT0_MODE=OPT_MODE_DEVICE
)

# Add the standard include files to the build
target_include_directories(rubber_usb PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(rubber_usb 
        
        )

pico_add_extra_outputs(rubber_usb)

