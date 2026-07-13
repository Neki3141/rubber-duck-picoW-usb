#include <string>
#include <string.h>
#include "tusb.h"

#include <pico/stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include <stdio.h>

#include "class/hid/hid_device.h"
#include "tusb.h"
// system timer to keep track of the 4-second intervals

#include "key_sent.hpp"
#include "script.hpp"
#define BUTTON_PIN 15

static uint32_t start_ms = 0;
static uint32_t last_log_ms = 0;
static bool key_pressed = false;
static uint16_t _tud_string_desc[32];

void send_key_task(void);

void input_task() {
    KeySent keySent;
    for (const std::string& command : COMMANDS) {
        // Here you would send the command to the host, e.g., via HID or CDC
        // For demonstration, we just print it to the console
        keySent.processInput(command); 
        sleep_ms(500); // Small delay between commands to ensure they are processed
    }
    tud_hid_keyboard_report(0, 0, NULL);
}

int main()
{
    tusb_init();
    // 1. Wait until the host PC physically recognizes the USB device
    while (!tud_mounted()) {
        tud_task(); 
    }

    // 2. CRITICAL: Give the Host OS driver time to initialize (e.g., Windows/Linux setup window)
    // We loop tud_task() for a brief moment so the USB stack stays alive during the handshake.
    uint32_t startup_delay = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - startup_delay < 500) {
        tud_task();
    }
    tud_task(); // Device task must be called continuously in the main loop
    input_task();
}


// Button setup for testing the input_task function
// Only for debugging purposes, not for production use
// int main()
// {
//     stdio_init_all();
//     gpio_init(BUTTON_PIN);
//     gpio_set_dir(BUTTON_PIN, GPIO_IN);
//     gpio_pull_up(BUTTON_PIN); // Enable pull-up resistor for the button
//     bool button_pressed = false;
//
//     tusb_init();
//     while (!tud_mounted()) {
//         tud_task(); // Keeps the USB handshake processing alive
//     }
//     while (1) {
//         tud_task(); // Device task must be called continuously in the main loop
//         if (gpio_get(BUTTON_PIN) == 0) { // Button is pressed (active low)
//             if (!button_pressed) {
//                 input_task();
//                 button_pressed = true;
//             }
//         } else {
//             button_pressed = false; // Button released
//         }
//     }
// }

// Debug function to send a key press every 4 seconds
void send_key_task(void)
{
    if (!tud_hid_ready())
        return;

    uint32_t current_time_ms = to_ms_since_boot(get_absolute_time());
    uint8_t modifier = KEYBOARD_MODIFIER_LEFTSHIFT; 

    // 1. Every 4 seconds, press the 'a' key
    if (!key_pressed && (current_time_ms - start_ms >= 4000))
    {
        uint8_t keycode[6] = {HID_KEY_C, 0, 0, 0, 0, 0};
        tud_hid_keyboard_report(0, modifier, keycode);

        key_pressed = true;
        start_ms = current_time_ms;
    }

    // 2. Release the key after 50ms
    if (key_pressed && (current_time_ms - start_ms >= 50))
    {
        tud_hid_keyboard_report(0, 0, NULL);

        key_pressed = false;
        start_ms = current_time_ms;
    }
}

// --------------------------------------------------------------------+
// TinyUSB Mandatory Device Descriptor Callbacks
// --------------------------------------------------------------------+

// 1. Device Descriptor
tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCafe,  // Dummy Vendor ID
    .idProduct = 0x4004, // Dummy Product ID
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01};

uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&desc_device;
}

// 2. HID Report Descriptor (tells the host this is a keyboard)
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return desc_hid_report;
}

// 3. Configuration Descriptor
enum
{
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT 0x02
#define EPNUM_CDC_IN 0x82
#define EPNUM_HID 0x83
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_DESC_LEN)

uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    // Interface number, string index, EP notification address and size, EP data out & in address, EP size
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 5, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 10)
};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return desc_configuration;
}

// 4. String Descriptors
char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: Supported language is English (0x0409)
    "Rubber_DucK",             // 1: Manufacturer
    "Pico Composite Device",    // 2: Product
    "1234567890",               // 3: Serial Number
    "USB CDC",                  // 4: CDC Interface
    "HID Keyboard",             // 5: HID Interface
};

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    uint8_t chr_count;

    if (index == 0)
    {
        _tud_string_desc[1] = 0x0409;
        chr_count = 1;
    }
    else
    {
        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
            return NULL;

        const char *str = string_desc_arr[index];
        chr_count = strlen(str);
        if (chr_count > 31)
            chr_count = 31;

        for (uint8_t i = 0; i < chr_count; i++)
        {
            _tud_string_desc[1 + i] = str[i];
        }
    }

    _tud_string_desc[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _tud_string_desc;
}

// --------------------------------------------------------------------+
// TinyUSB HID Callbacks (Required even if not used)
// --------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}
  
