# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Development Setup

- **Repository root**: `/home/kien/kien/rubber_usb`
- **Build system**: CMake (requires Raspberry Pi Pico SDK)
- **Key dependencies**:
  - `pico_sdk_import.cmake` (included via `pico_sdk_init()`)
  - TinyUSB for USB HID functionality
  - `hardware_gpio` for GPIO access
- **Standard commands** (run from repository root):
  ```bash
  # Install dependencies (Ubuntu example)
  sudo apt-get install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi

  # Build the project
  mkdir -p build && cd build
  cmake .. -DPICO_SDK_PATH=/path/to/pico-sdk
  make -j4

  # Flash the device (requires picotool)
  picotool -U rubber_usb.uf2
  ```

## Project Architecture

### Core Components

1. **main.cpp**
   - Entry point that initializes TinyUSB and runs `input_task()`
   - Handles USB mounting delay and device task loop
   - Provides the main execution flow for the rubber USB device

2. **key_sent.hpp / key_sent.cpp**
   - Implements `KeySent` class responsible for processing command strings
   - Manages modifier state (Shift, Ctrl, Alt, Win) reset per line
   - Handles key translation and sending via `sendKey()`
   - Contains debug/loopback test code commented out for production use

3. **key_translate.hpp / key_translate.cpp**
   - Maps characters to HID keycodes (including shifted symbols)
   - Provides `translateKey()` function used by `KeySent::sendKey()`
   - Supports alphanumerics, punctuation, and special symbols

4. **command.txt**
   - Simple text file containing test command sequences
   - Example format: `-c: sadfsadf FAII fjI`
   - Used for validating command parsing and key sending logic

### USB HID Implementation Details

- Uses TinyUSB descriptors to present as a **keyboard** device
- Implements required callbacks:
  - `tud_hid_get_report_cb`
  - `tud_hid_set_report_cb`
  - `tud_descriptor_device_cb`
  - `tud_hid_descriptor_report_cb`
  - `tud_descriptor_configuration_cb`
  - `tud_descriptor_string_cb`
- Handles key press/release timing with 10ms preparation and 10ms hold
- Uses modifier byte composition for Shift, Ctrl, Alt, GUI combinations

## Common Tasks

- **Build**: `mkdir -p build && cd build && cmake .. && make`
- **Flash**: Use `picotool` to upload the generated `.uf2` binary
- **Debug**: Uncomment the button test section in `main.cpp` to use GPIO button input
- **Command Validation**: Check `command.txt` entries for syntax (`-c:` prefix, space-separated arguments)

## Important Notes

- Modifier state (`is_shift`, `is_alt`, `is_ctrl`, `is_win`) is **reset at the start of each line** to prevent bleed-over
- Timing constants:
  - 4-second interval for periodic key sending
  - 10ms preparation before key press
  - 50ms key hold duration
- The USB device must be manually reset (or powered cycle) after each flash
- Comments in `main.cpp` indicate where GPIO button testing can be enabled

## Testing Strategy

- No formal test suite is present; validation relies on:
  - Observing USB key output on a connected host
  - Using `command.txt` for deterministic command sequences
  - Manual verification of key mappings via console output

I've created the initial CLAUDE.md file with guidance for future Claude Code sessions. The file contains:

1. Development setup instructions (dependencies, build commands, flashing)
2. Project architecture overview (main.cpp, key_sent.*, key_translate.*, command.txt)
3. USB HID implementation details (descriptor callbacks, timing, modifiers)
4. Common tasks (build, flash, debug, command validation)
5. Testing strategy (no formal tests, manual verification approach)

The file follows the required format starting with "# CLAUDE.md" and provides concrete, repository-specific guidance without generic development advice. Future instances can reference this file to understand the codebase structure and development workflow.
