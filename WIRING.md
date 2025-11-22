# ESP32 CYD HDMI Matrix Controller - Wiring Guide

## Hardware Components

1. **ESP32-2432S028R** (Cheap Yellow Display)
2. **HDMI Matrix Switch** with RS-232 serial control
3. Connecting wires
4. USB cable for programming and power

## Wiring Connections

### Serial Connection to HDMI Matrix Switch

The ESP32 CYD communicates with the HDMI matrix switch using UART (Serial2):

```
ESP32 CYD          HDMI Matrix Switch
---------          ------------------
GPIO 17 (TX)  -->  RX (Receive)
GPIO 16 (RX)  <--  TX (Transmit)
GND           -->  GND (Common Ground)
```

**Important Notes:**
- Ensure common ground connection between ESP32 and matrix switch
- If your matrix switch uses RS-232 levels (±12V), you'll need a level shifter:
  - Use a MAX3232 or similar RS-232 to TTL converter
  - Connect ESP32 TX/RX to the TTL side of the converter
  - Connect the RS-232 side to your matrix switch

### Power Supply

**Option 1: USB Power (Recommended for Development)**
```
USB Cable --> ESP32 CYD USB Port
```
- Provides 5V power
- Allows programming and debugging
- Monitor serial output

**Option 2: External 5V Power**
```
5V Power Supply --> 5V and GND pins on ESP32
```
- Use for permanent installations
- Ensure power supply provides at least 500mA

## Wiring Diagram

```
┌─────────────────────────────┐
│   ESP32 CYD (2432S028R)    │
│                             │
│  ┌─────────────────────┐   │
│  │   2.8" TFT Touch    │   │
│  │      Display        │   │
│  └─────────────────────┘   │
│                             │
│  GPIO 17 (TX) ●─────────────┼──→ RX  ┐
│  GPIO 16 (RX) ●─────────────┼──← TX  │
│  GND          ●─────────────┼──→ GND │  HDMI Matrix
│                             │        │  Switch
│  USB Port     □             │        │  (RS-232/TTL)
└─────────────────────────────┘        ┘

If using RS-232 Matrix Switch:

ESP32 TX → TTL TX ┐              ┌→ RS-232 RX → Matrix RX
ESP32 RX ← TTL RX │  MAX3232    │← RS-232 TX ← Matrix TX
ESP32 GND → GND   └──Converter──┘→ GND → Matrix GND
```

## Setup Steps

1. **Before Wiring:**
   - Disconnect power from both devices
   - Identify TX, RX, and GND pins on your HDMI matrix switch
   - Verify if your matrix uses TTL (3.3V/5V) or RS-232 (±12V) levels

2. **Connect Wires:**
   - Connect ESP32 GPIO 17 to matrix switch RX (or level shifter)
   - Connect ESP32 GPIO 16 to matrix switch TX (or level shifter)
   - Connect GND to GND (critical for proper communication)

3. **Power On:**
   - Connect USB cable to ESP32 CYD
   - Power on HDMI matrix switch
   - Display should show 6 buttons

4. **Test:**
   - Touch a button on the display
   - Monitor serial output on your computer to see commands being sent
   - Verify matrix switch responds to commands

## Troubleshooting Wiring Issues

### No Communication
- Check TX/RX are not swapped
- Verify common ground connection
- Confirm baud rate matches (115200)
- Test with a USB-to-serial adapter to verify commands

### Display Issues
- Display should work independently of serial connection
- If blank: Check backlight (GPIO 21 should be HIGH)
- If no touch: Touch controller is independent, may need calibration

### Level Shifting Required?
Test if you need a level shifter:
- Measure voltage on matrix switch TX pin (should be ~3.3V for TTL, ±12V for RS-232)
- If ±12V or voltage outside 0-3.3V range, use MAX3232 or similar converter

## Safety Notes

⚠️ **Important:**
- Never connect RS-232 levels (±12V) directly to ESP32 - this will damage the board
- Always disconnect power before wiring
- Double-check polarity and pin assignments
- Use a multimeter to verify ground continuity
