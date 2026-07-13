#ifndef KEY_SENT_HPP
#define KEY_SENT_HPP


#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <hardware/timer.h>
#include <pico/time.h>
#include <string>

#include "pico/stdlib.h"
#include "key_translate.hpp"

class KeySent {
    public:
        KeySent() = default;
        
        void processInput(const std::string& input);
              
        private:
        uint8_t keycode[6] = {0}; // Array to hold the keycodes for the keys to be sent
        bool is_win{false};
        bool is_shift{false}; 
        bool is_ctrl{false};
        bool is_alt{false};

        std::size_t line_number{0};
        std::uint32_t start_ms{to_ms_since_boot(get_absolute_time())}; // Initialize start_ms with the current time in milliseconds
        


        void processString(const std::string& keys);
        void sendKey(char key, bool is_shift);
        void sendCombo(char key);
    
};      







#endif // KEY_SENT_HPP  
