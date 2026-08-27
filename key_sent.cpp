#include "key_sent.hpp"
#include "key_translate.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <stdexcept>

void KeySent::processInput(const std::string &input) {
    line_number++;
    
    // Ignore comments or empty lines safely
    if (input.empty() || input[0] == '/') {
        return; 
    }
    
    size_t spacePos = input.find(':');
    if (spacePos == std::string::npos || spacePos > 4) {
        return; // No valid command space boundary found
    }

    // Split the input into command and the rest of the string
    std::string command = input.substr(0, spacePos);
    std::string input_string = input.substr(spacePos + 1);

    // CRITICAL FIX: Reset state modifiers every time a new line is evaluated 
    // so flags don't permanently bleed into future lines.
    is_shift = false;
    is_alt = false;
    is_ctrl = false;
    is_win = false;

    for (char c : command){
        switch (c){
            case 's':
                is_shift = true;
                break;
            case 'a':
                is_alt = true;
                break;
            case 'c':
                is_ctrl = true;
                break;
            case 'w':
                is_win = true;
                break;
            case 'i':
                // No state change, just process the input_string
                processString(input_string);
                return; // Exit early since we don't need to set any flags
            case 'd':
                // Delay command, parse the number and call delay function
                delay(input_string);
                return; // Exit early after processing delay
            default:
                std::cerr << "Error: Invalid comment character '" << c << "' on line " << line_number << std::endl;
                return; // Exit early on invalid comment character
        } 
        
    }

    // process shortcut or keybinds
    sendKey(input_string[0], is_shift);
    std::cout << "input_char: " << input_string[0] << std::endl;
    return; // Exit after processing the keybind
}

void KeySent::processString(const std::string& keys) {
    static const std::string shift_symbols = "!@#$%^&*()_+{}|:\"<>?~";
    
    for (char key : keys) {
        // FIX: Explicitly drop any trailing newline or carriage return characters
        // passed down from stream reading (like std::getline) to stop the ghost "Enter" keypress.
        if (key == '\0') {
            break; 
        }

        // FIX: Evaluates if either the global line flag (-s:) OR individual character properties 
        // require a Shift modifier activation.
        bool needs_shift = is_shift || (shift_symbols.find(key) != std::string::npos) || (key >= 'A' && key <= 'Z');

        sendKey(key, needs_shift);
    }  

}

void KeySent::sendKey(char key, bool use_shift) {
    // Completely clear the internal array to 0 on every call
    for (int i = 0; i < 6; i++) {
        keycode[i] = 0;
    }
    
    keycode[0] = KeyTranslate::translateKey(key);
    uint8_t modifier = 0;
    
    if (use_shift) {

    modifier |= KEYBOARD_MODIFIER_LEFTSHIFT;
    std::cout << "Sending key: " << key << " with shift: " << (use_shift ? "true" : "false") << std::endl;
    std::cout << "Modifier byte: " << std::hex << static_cast<int>(modifier) << std::dec << std::endl;
    }
    if (is_ctrl)   modifier |= KEYBOARD_MODIFIER_LEFTCTRL;
    if (is_alt)    modifier |= KEYBOARD_MODIFIER_LEFTALT;
    if (is_win)    modifier |= KEYBOARD_MODIFIER_LEFTGUI;
    
    // Use volatile to prevent compiler register-caching optimizations
    uint32_t loop_start = to_ms_since_boot(get_absolute_time());
    uint32_t action_time = loop_start;
    bool pressed = false;
    

    while (true) {
        // Run the USB background worker on every single iteration
        tud_task(); 

        uint32_t current_time = to_ms_since_boot(get_absolute_time());


        // Stage 1: Send the key press after a 20ms preparation window
        if (!pressed && (current_time - loop_start >= 10)) {
            tud_hid_keyboard_report(0, modifier, keycode);
            pressed = true;
            action_time = current_time; 
        }

        // Stage 2: Hold the key down for 40ms, then send a real release packet and break
        if (pressed && (current_time - action_time >= 10)) {
            keycode[0] = 0; // Clear the keycode to indicate release
            tud_hid_keyboard_report(0, 0, keycode); 
            break; 
        }
    }
    
}

void KeySent::delay(std::string delay_ms_str) {
    int delay_ms = std::stoi(delay_ms_str);
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < delay_ms) {
        tud_task(); // Keep the USB stack alive during the delay
                    // This is important to prevent the host from thinking the device has disconnected
    }
}
