#include "key_translate.hpp"

uint8_t KeyTranslate::translateKey(char key) {
    switch (key) {
        case 'a': return HID_KEY_A;
        case 'b': return HID_KEY_B;
        case 'c': return HID_KEY_C;
        case 'd': return HID_KEY_D;
        case 'e': return HID_KEY_E;
        case 'f': return HID_KEY_F;
        case 'g': return HID_KEY_G;
        case 'h': return HID_KEY_H;
        case 'i': return HID_KEY_I;
        case 'j': return HID_KEY_J;
        case 'k': return HID_KEY_K;
        case 'l': return HID_KEY_L;
        case 'm': return HID_KEY_M;
        case 'n': return HID_KEY_N;
        case 'o': return HID_KEY_O;
        case 'p': return HID_KEY_P;
        case 'q': return HID_KEY_Q;
        case 'r': return HID_KEY_R;
        case 's': return HID_KEY_S;
        case 't': return HID_KEY_T;
        case 'u': return HID_KEY_U;
        case 'v': return HID_KEY_V;
        case 'w': return HID_KEY_W;
        case 'x': return HID_KEY_X;
        case 'y': return HID_KEY_Y;
        case 'z': return HID_KEY_Z;

        case 'A': return HID_KEY_A; // Shift + uint16_t
        case 'B': return HID_KEY_B; // Shift + uint16_t
        case 'C': return HID_KEY_C; // Shift + uint16_t
        case 'D': return HID_KEY_D; // Shift + uint16_t
        case 'E': return HID_KEY_E; // Shift + uint16_t
        case 'F': return HID_KEY_F; // Shift + uint16_t
        case 'G': return HID_KEY_G; // Shift + uint16_t
        case 'H': return HID_KEY_H; // Shift + uint16_t
        case 'I': return HID_KEY_I; // Shift + uint16_t
        case 'J': return HID_KEY_J; // Shift + uint16_t
        case 'K': return HID_KEY_K; // Shift + uint16_t
        case 'L': return HID_KEY_L; // Shift + uint16_t
        case 'M': return HID_KEY_M; // Shift + uint16_t
        case 'N': return HID_KEY_N; // Shift + uint16_t
        case 'O': return HID_KEY_O; // Shift + uint16_t
        case 'P': return HID_KEY_P; // Shift + uint16_t
        case 'Q': return HID_KEY_Q; // Shift + uint16_t
        case 'R': return HID_KEY_R; // Shift + uint16_t
        case 'S': return HID_KEY_S; // Shift + uint16_t
        case 'T': return HID_KEY_T; // Shift + uint16_t
        case 'U': return HID_KEY_U; // Shift + uint16_t
        case 'V': return HID_KEY_V; // Shift + uint16_t
        case 'W': return HID_KEY_W; // Shift + uint16_t
        case 'X': return HID_KEY_X; // Shift + uint16_t
        case 'Y': return HID_KEY_Y; // Shift + uint16_t
        case 'Z': return HID_KEY_Z; // Shift + uint16_t

        case '0': return HID_KEY_0;
        case '1': return HID_KEY_1;
        case '2': return HID_KEY_2;
        case '3': return HID_KEY_3;
        case '4': return HID_KEY_4;
        case '5': return HID_KEY_5;
        case '6': return HID_KEY_6;
        case '7': return HID_KEY_7;
        case '8': return HID_KEY_8;
        case '9': return HID_KEY_9;

        case ' ': return HID_KEY_SPACE;
        case '\t': return HID_KEY_TAB;
        case '\n': return HID_KEY_ENTER;

        case '!': return HID_KEY_1; // Shift + uint16_t
        case '@': return HID_KEY_2; // Shift + uint16_t
        case '#': return HID_KEY_3; // Shift + uint16_t
        case '$': return HID_KEY_4; // Shift + uint16_t
        case '%': return HID_KEY_5; // Shift + uint16_t
        case '^': return HID_KEY_6; // Shift + uint16_t
        case '&': return HID_KEY_7; // Shift + uint16_t
        case '*': return HID_KEY_8; // Shift + uint16_t
        case '(': return HID_KEY_9; // Shift + uint16_t
        case ')': return HID_KEY_0; // Shift + uint16_t

        case '-': return HID_KEY_MINUS;
        case '_': return HID_KEY_MINUS; // Shift + uint16_t
        case '=': return HID_KEY_EQUAL;
        case '+': return HID_KEY_EQUAL; // Shift + uint16_t
        case '`': return HID_KEY_GRAVE;
        case '~': return HID_KEY_GRAVE; // Shift + uint16_t
        case '[': return HID_KEY_BRACKET_LEFT;
        case '{': return HID_KEY_BRACKET_LEFT; // Shift + uint16_t
        case ']': return HID_KEY_BRACKET_RIGHT;
        case '}': return HID_KEY_BRACKET_RIGHT; // Shift + uint16_t
        case '\\': return HID_KEY_BACKSLASH;
        case '|': return HID_KEY_BACKSLASH; // Shift + uint16_t
        case ';': return HID_KEY_SEMICOLON;
        case ':': return HID_KEY_SEMICOLON; // Shift + uint16_t
        case '\'': return HID_KEY_APOSTROPHE;
        case '"': return HID_KEY_APOSTROPHE; // Shift + uint16_t
        case ',': return HID_KEY_COMMA;
        case '<': return HID_KEY_COMMA; // Shift + uint16_t
        case '.': return HID_KEY_PERIOD ;
        case '>': return HID_KEY_PERIOD ; // Shift + uint16_t
        case '/': return HID_KEY_SLASH;
        case '?': return HID_KEY_SLASH; // Shift + uint16_t
        
        default: return 0; // Not a valid key
    }
}



