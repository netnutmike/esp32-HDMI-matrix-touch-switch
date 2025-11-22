# HDMI Matrix Switch Command Reference

This document provides guidance on configuring serial commands for various HDMI matrix switch models.

## Default Configuration

The application is pre-configured with a common command format:
```
MT00SW[OUTPUT][INPUT]NT\r\n
```

Example: `MT00SW0101NT\r\n` switches output 1 to input 1.

## Common HDMI Matrix Switch Protocols

### 1. MT-VIKI / MT-ViKi Style (Default)
**Command Format:** `MT00SW[OUTPUT][INPUT]NT\r\n`

Examples:
```
MT00SW0101NT\r\n  // Output 1 to Input 1
MT00SW0102NT\r\n  // Output 1 to Input 2
MT00SW0201NT\r\n  // Output 2 to Input 1
```

### 2. Generic ASCII Style
**Command Format:** `SW [INPUT] [OUTPUT]\r\n`

Examples:
```
SW 1 1\r\n  // Output 1 to Input 1
SW 2 1\r\n  // Output 1 to Input 2
SW 1 2\r\n  // Output 2 to Input 1
```

### 3. Monoprice Blackbird Style
**Command Format:** `:[INPUT][OUTPUT]AVX\r\n`

Examples:
```
:11AVX\r\n  // Output 1 to Input 1
:21AVX\r\n  // Output 1 to Input 2
:12AVX\r\n  // Output 2 to Input 1
```

### 4. HDMI Matrix with Hex Commands
**Command Format:** Binary/Hex commands

Example (varies by manufacturer):
```cpp
// Send as hex bytes
uint8_t cmd[] = {0xAA, 0x01, 0x01, 0xBB};  // Input 1 to Output 1
MATRIX_SERIAL.write(cmd, sizeof(cmd));
```

### 5. OREI / J-Tech Style
**Command Format:** `SW I0[INPUT] O0[OUTPUT]\r\n`

Examples:
```
SW I01 O01\r\n  // Output 1 to Input 1
SW I02 O01\r\n  // Output 1 to Input 2
```

## How to Configure Commands

### Method 1: Modify in main.cpp

Edit the `commands` array in `src/main.cpp` (around line 137):

```cpp
String commands[BUTTON_COUNT] = {
    "YOUR_COMMAND_1\r\n",
    "YOUR_COMMAND_2\r\n",
    "YOUR_COMMAND_3\r\n",
    "YOUR_COMMAND_4\r\n",
    "YOUR_COMMAND_5\r\n",
    "YOUR_COMMAND_6\r\n"
};
```

### Method 2: Create a Commands Header File

1. Create `include/commands.h`:

```cpp
#ifndef COMMANDS_H
#define COMMANDS_H

// Monoprice Blackbird example
const char* CMD_INPUT_1 = ":11AVX\r\n";
const char* CMD_INPUT_2 = ":21AVX\r\n";
const char* CMD_INPUT_3 = ":31AVX\r\n";
const char* CMD_INPUT_4 = ":41AVX\r\n";
const char* CMD_INPUT_5 = ":51AVX\r\n";
const char* CMD_INPUT_6 = ":61AVX\r\n";

#endif
```

2. Include in `main.cpp`:
```cpp
#include "commands.h"
```

3. Use in commands array:
```cpp
String commands[BUTTON_COUNT] = {
    CMD_INPUT_1, CMD_INPUT_2, CMD_INPUT_3,
    CMD_INPUT_4, CMD_INPUT_5, CMD_INPUT_6
};
```

## Testing Commands

### Using Serial Monitor

1. Open PlatformIO serial monitor:
   ```bash
   pio device monitor
   ```

2. Touch a button on the display

3. Observe the command being sent in the monitor

### Manual Testing via Computer

Before implementing in code, test commands manually:

1. Connect matrix switch to computer via USB-to-serial adapter

2. Use a serial terminal (screen, minicom, PuTTY, etc.):
   ```bash
   screen /dev/ttyUSB0 115200
   ```

3. Type commands manually and verify matrix responds

4. Once confirmed, add working commands to the application

## Common Command Patterns

### Multi-Output Commands
If controlling multiple outputs simultaneously:
```cpp
// Switch all outputs to input 1
commands[0] = "MT00SWALL01NT\r\n";

// Or chain commands
void sendCommand(int input) {
    for (int output = 1; output <= 4; output++) {
        String cmd = "SW " + String(input) + " " + String(output) + "\r\n";
        MATRIX_SERIAL.print(cmd);
        delay(50);  // Small delay between commands
    }
}
```

### Status Query Commands
Some switches support status queries:
```cpp
// Query current routing
MATRIX_SERIAL.print("STATUS?\r\n");

// Read response in loop()
if (MATRIX_SERIAL.available()) {
    String response = MATRIX_SERIAL.readStringUntil('\n');
    Serial.println("Matrix response: " + response);
}
```

## Baud Rate Configuration

If your matrix uses a different baud rate, modify in `main.cpp`:

```cpp
#define MATRIX_BAUD 115200  // Change to 9600, 19200, 38400, 57600, etc.
```

Common baud rates:
- 9600 (older devices)
- 19200
- 38400
- 57600
- 115200 (modern devices)

## Need Help?

If you're unsure about your matrix switch commands:
1. Check the manufacturer's documentation
2. Look for a protocol or command reference manual
3. Search for "[Your Matrix Model] serial commands"
4. Open an issue with your matrix switch model for assistance
