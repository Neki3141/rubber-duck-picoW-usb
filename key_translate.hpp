#ifndef KEY_TRANSLATE_HPP
#define KEY_TRANSLATE_HPP

// #include <cstdint>
#include <cstdint>
#include "tusb.h"
#include <stdio.h>
#include "class/hid/hid_device.h"

class KeyTranslate {
public:
    static uint8_t translateKey(char key);
private:
    int test;   
};

#endif // KEY_TRANSLATE_HPP
