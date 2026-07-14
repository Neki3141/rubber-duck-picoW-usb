# Rubber USB Keyboard Demo

A TinyUSB HID keyboard project for the **Raspberry Pi Pico W** that sends
keystrokes to a host PC from a predefined script. The script can be triggered
automatically on startup or via a GPIO button for testing.

## Table of Contents

1. [Hardware requirements](#hardware-requirements)
2. [Software requirements](#software-requirements)
3. [Getting the source](#getting-the-source)
4. [Building & flashing](#building--flashing)
5. [Writing a script (`script.hpp`)](#writing-a-script-scripthpp)
6. [Running the program](#running-the-program)
   - 6.1 [Normal mode (auto-run on startup)](#normal-mode-auto-run-on-startup)
   - 6.2 [Button-triggered test loop](#button-triggered-test-loop)
7. [Command format reference](#command-format-reference)
8. [Debugging & logging](#debugging--logging)
9. [Contributing](#contributing)
10. [License](#license)

---

## Hardware requirements

| Item | Minimum spec | Purpose |
|------|--------------|---------|
| **Raspberry Pi Pico W** (or original Pico) | 2 MiB flash, RP2040 MCU | Runs the TinyUSB stack & GPIO code |
| **Micro-USB cable** | Data-capable | Power + UF2 flashing |
| **Push-button** (optional) | 2-wire momentary, active-low | Enables manual trigger of the script |
| **Breadboard / wiring** (optional) | None | To connect the button to GPIO 15 |
| **Host PC** (Windows / macOS / Linux) | Any OS with USB HID support | Receives the simulated keyboard events |

> **Note:** The GPIO pin for the button is hard-coded as `#define BUTTON_PIN 15`.
> If you need a different pin, change the definition in `main.cpp` and re-compile.

---

## Software requirements

| Tool | Version (as of 2026-07-13) | Install command (Ubuntu/Debian) |
|------|----------------------------|----------------------------------|
| **CMake** | >= 3.13 | `sudo apt-get install cmake` |
| **GNU Arm Embedded Toolchain** | `gcc-arm-none-eabi` 10+ | `sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi` |
| **Raspberry Pi Pico SDK** | v2.2.0 (or later) | `git clone https://github.com/raspberrypi/pico-sdk.git` |
| **picotool** (for flashing) | >= 1.5 | `sudo apt-get install picotool` (or build from source) |

> The repository already contains `pico_sdk_import.cmake` which sources the SDK.
> Ensure `PICO_SDK_PATH` points to the location where you cloned the SDK.
> TinyUSB is pulled in by the SDK, no extra install needed.

---

## Getting the source

```bash
git clone https://github.com/Neki3141/rubber_usb.git
cd rubber_usb
```

The project layout is:

```
├─ CMakeLists.txt          # Build configuration
├─ main.cpp                # USB init + optional button loop
├─ key_sent.{hpp,cpp}      # Command parsing & key sending
├─ key_translate.{hpp,cpp} # Char -> HID keycode map
├─ script.hpp              # User-editable command vector
├─ command.txt             # Plain-text test script (optional)
└─ build/                  # Out-of-source build directory (generated)
```

---

## Building & flashing

```bash
# 1. Create a build folder and invoke CMake
mkdir -p build && cd build
cmake .. -DPICO_SDK_PATH=/path/to/pico-sdk

# 2. Compile (add -jN to use N cores)
make -j4

# 3. Flash the UF2 file (requires picotool)
picotool -U rubber_usb.uf2
#    - Or copy the UF2 manually after entering BOOTSEL:
#      cp rubber_usb.uf2 /media/$USER/RPI-RP2/
```

After flashing, the device will enumerate on the host as "Rubber_DucK Keyboard"
(see the HID descriptor strings).

---

## Writing a script (`script.hpp`)

`script.hpp` defines a global `std::vector<std::string> COMMANDS`. Each entry is
a line that `KeySent::processInput()` will parse.

### Example template (already in the repo)

```cpp
#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <vector>
#include <string>

std::vector<std::string> COMMANDS = {
    "-w: b",                                   // Windows (GUI) key + 'b'
    "-i: https://youtu.be/dQw4w9WgXcQ\n"       // Type a URL (no modifiers)
};

#endif // SCRIPT_HPP
```

### How to add your own commands

1. Open `script.hpp` in your editor.
2. Append a new string following the syntax described in the next section.
3. Save & rebuild (the vector is compiled directly, no runtime reload).

You can also edit `command.txt` and run `input_task()` manually (useful for
quick ad-hoc testing), but the compiled `COMMANDS` vector is the canonical way
for the firmware.

---

## Running the program

### 6.1 Normal mode (auto-run on startup)

When the device powers up:

1. TinyUSB initializes and waits for the host to mount (`while (!tud_mounted())`).
2. After a short (~500 ms) hand-shake delay the code calls `input_task()`.
3. `input_task()` iterates over `COMMANDS` and sends each line with a 500 ms
   inter-command pause.
4. When the list is exhausted the HID report `tud_hid_keyboard_report(0, 0, NULL)`
   is sent to release all keys.

**Result:** The host receives the keystrokes as if you typed them on a physical keyboard.

### 6.2 Button-triggered test loop

The **debug section** (commented out in `main.cpp`) shows how to use a GPIO
button to drive the same `input_task()` on demand.

To enable it:

1. Uncomment the block between line 58 and line 81 in `main.cpp`.
2. Re-build and flash the firmware.

```cpp
// int main()
// {
//     stdio_init_all();
//     gpio_init(BUTTON_PIN);
//     gpio_set_dir(BUTTON_PIN, GPIO_IN);
//     gpio_pull_up(BUTTON_PIN); // active-low button
//     bool button_pressed = false;
//
//     tusb_init();
//     while (!tud_mounted()) { tud_task(); }
//
//     while (1) {
//         tud_task(); // keep USB stack alive
//         if (gpio_get(BUTTON_PIN) == 0) {          // pressed
//             if (!button_pressed) {
//                 input_task();                     // run the script once
//                 button_pressed = true;
//             }
//         } else {
//             button_pressed = false;               // released - ready for next press
//         }
//     }
// }
```

**Behaviour:**

- Press the button -> the script runs once.
- Release -> the firmware resets the flag, allowing the next press to trigger again.

This is handy for rapid iteration: you can edit `script.hpp`, rebuild, flash,
then use the button to test without power-cycling the board.

---

## Command format reference

Each line must start with a **modifier prefix** followed by a colon `:` and a
**space-separated payload**. The parser only looks at the first token before
the first space.

| Prefix | Meaning | Example | Effect |
|--------|---------|---------|--------|
| `-s:` | Shift (left shift) | `-s: a` | Sends **'A'** (shift + 'a') |
| `-c:` | Ctrl (left control) | `-c: c` | Sends **Ctrl + 'c'** |
| `-a:` | Alt (left alt) | `-a: x` | Sends **Alt + 'x'** |
| `-w:` | Win / GUI (left GUI) | `-w: d` | Sends **Win + 'd'** |
| `-i:` | Insert string (no modifiers) | `-i: hello\n` | Types the literal characters `hello` and press enter |
| No prefix | *Invalid* - rejected with an error message | - | - |

**Rules**

- The **command token** must be exactly four characters (`-x:`) - longer prefixes are ignored.
- The payload may contain any printable ASCII character; the translation table
  (`KeyTranslate::translateKey`) handles letters, numbers, punctuation, and common symbols.
- Newline (`\n`) or carriage-return characters at the end of a payload are stripped automatically.
- Multiple commands can be placed on separate lines in the vector; they are
  processed sequentially with a 500 ms pause (`sleep_ms(500)`) to give the host
  time to consume each report.

---

## Debugging & logging

- The firmware prints diagnostic messages to the **Serial (USB CDC)** console via
  `printf`. To view them, open a serial terminal (e.g., `minicom -D /dev/ttyACM0 -b 115200`).
- In `KeySent::sendKey()` you will see:

  ```cpp
  std::cout << "Sending key: " << key << " with shift: " << (use_shift ? "true" : "false") << std::endl;
  std::cout << "Modifier byte: " << std::hex << static_cast<int>(modifier) << std::dec << std::endl;
  ```

  These help verify that the correct HID modifier bits are set.

- The **button test** prints nothing by default; you can add `printf` calls
  inside the loop to confirm press detection.

---

## Contributing

1. Fork the repository.
2. Create a feature branch (`git checkout -b my-feature`).
3. Make changes (e.g., add new command prefixes, improve timing).
4. Run `make -j4 && picotool -U rubber_usb.uf2` to test on hardware.
5. Submit a Pull Request with a concise description and, if applicable, an
   updated `README.md` explaining new usage.

Please keep the coding style consistent:

- Two-space indentation,
- Comment every public method,
- Preserve the existing "CRITICAL FIX" comments that document important
  bug-prevention logic.

---

## License

This project is released under the **MIT License** - see the `LICENSE` file in
the repository root.

---

*Happy hacking! If you run into any issues with the script syntax, button
wiring, or USB enumeration, feel free to ask for more specific help.*
