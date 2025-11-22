# ESP32 CYD HDMI Matrix Touch Switch Controller

An ESP32 application for the Cheap Yellow Display (CYD) that controls an HDMI matrix switch via serial commands using a touch interface.

## Hardware Requirements

- **ESP32-2432S028R** (Cheap Yellow Display/CYD)
  - ILI9341 2.8" TFT Display (320x240)
  - XPT2046 Touch Controller
  - ESP32 Development Board
- **HDMI Matrix Switch** with serial control support (115200 baud, 8N1)

## Features

- **6-Button Touch Interface** - Organized in a 3x2 grid layout
- **Serial Communication** - 115200 baud, 8 data bits, no parity, 1 stop bit (8N1)
- **Visual Feedback** - Buttons change color when pressed
- **Customizable Commands** - Easy to modify HDMI matrix switch commands

## Pin Configuration

### TFT Display (ILI9341)
- MISO: GPIO 12
- MOSI: GPIO 13
- SCLK: GPIO 14
- CS: GPIO 15
- DC: GPIO 2
- BACKLIGHT: GPIO 21

### Touch Controller (XPT2046)
- MOSI: GPIO 32
- MISO: GPIO 39
- CLK: GPIO 25
- CS: GPIO 33
- IRQ: GPIO 36

### Serial Communication (HDMI Matrix)
- TX: GPIO 17
- RX: GPIO 16
- Baud Rate: 115200
- Config: 8N1

## Installation

1. Install [PlatformIO](https://platformio.org/install)
2. Clone this repository
3. Open the project in VS Code with PlatformIO extension
4. Connect your CYD via USB
5. Build and upload:
   ```bash
   pio run --target upload
   ```

## Usage

The display shows 6 buttons labeled "Input 1" through "Input 6". Touch any button to send the corresponding command to your HDMI matrix switch.

### Customizing Commands

Edit the `commands` array in `src/main.cpp` (around line 137) to match your HDMI matrix switch protocol:

```cpp
String commands[BUTTON_COUNT] = {
    "MT00SW0101NT\r\n",  // Switch output 1 to input 1
    "MT00SW0102NT\r\n",  // Switch output 1 to input 2
    "MT00SW0103NT\r\n",  // Switch output 1 to input 3
    "MT00SW0104NT\r\n",  // Switch output 1 to input 4
    "MT00SW0105NT\r\n",  // Switch output 1 to input 5
    "MT00SW0106NT\r\n"   // Switch output 1 to input 6
};
```

### Customizing Button Labels

Modify the `labels` array in `src/main.cpp` (around line 134):

```cpp
String labels[BUTTON_COUNT] = {"Input 1", "Input 2", "Input 3", "Input 4", "Input 5", "Input 6"};
```

## Debugging

Open the serial monitor to see debug output:
```bash
pio device monitor
```

The application logs:
- Touch events with coordinates
- Button presses
- Commands sent to the HDMI matrix switch

## HDMI Matrix Switch Commands

The default commands follow a common matrix switch protocol format:
- `MT00SW[output][input]NT\r\n`

Examples:
- `MT00SW0101NT` - Switch output 1 to input 1
- `MT00SW0102NT` - Switch output 1 to input 2

**Note:** Adjust these commands according to your specific HDMI matrix switch model's protocol documentation.

## Troubleshooting

### Touch not working
- Check that the touch calibration values in `getTouch()` function are correct for your display
- The default values work for most CYD units, but may need adjustment

### Serial communication issues
- Verify the baud rate matches your HDMI matrix switch (default: 115200)
- Check TX/RX connections (GPIO 17 and 16)
- Ensure proper ground connection between ESP32 and matrix switch

### Display orientation
- Modify `tft.setRotation(1)` in `setup()` if the display appears rotated
- Values: 0, 1, 2, 3 for different orientations

## License

MIT License - Feel free to modify and use for your projects

## Credits

Built using:
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) by Bodmer
- PlatformIO ESP32 framework
