/**
 * ESP32 CYD HDMI Matrix Switch Controller
 * 
 * Controls an HDMI matrix switch via serial commands (115200 baud, 8N1)
 * Features a 6-button touch interface on the Cheap Yellow Display
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

// TFT Display instance
TFT_eSPI tft = TFT_eSPI();

// Touch calibration data for CYD
#define TOUCH_THRESHOLD 600
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

// Serial port configuration for HDMI matrix switch
#define MATRIX_SERIAL Serial2
#define MATRIX_RX 16
#define MATRIX_TX 17
#define MATRIX_BAUD 115200

// Button layout configuration
#define BUTTON_ROWS 3
#define BUTTON_COLS 2
#define BUTTON_COUNT 6
#define BUTTON_MARGIN 10
#define BUTTON_SPACING 10

// Colors
#define BUTTON_COLOR TFT_BLUE
#define BUTTON_PRESSED_COLOR TFT_DARKGREEN
#define BUTTON_TEXT_COLOR TFT_WHITE
#define BACKGROUND_COLOR TFT_BLACK

// Button structure
struct Button {
    int16_t x, y, w, h;
    uint8_t id;
    String label;
    String command;
    bool pressed;
};

Button buttons[BUTTON_COUNT];
int buttonWidth, buttonHeight;

// Touch variables
SPIClass touchSPI = SPIClass(HSPI);
uint16_t touchX = 0, touchY = 0;
bool touched = false;

/**
 * Initialize the touch controller
 */
void initTouch() {
    touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    pinMode(XPT2046_CS, OUTPUT);
    digitalWrite(XPT2046_CS, HIGH);
    pinMode(XPT2046_IRQ, INPUT);
}

/**
 * Read touch coordinates from XPT2046
 */
bool getTouch(uint16_t &x, uint16_t &y) {
    if (digitalRead(XPT2046_IRQ) != LOW) {
        return false;
    }

    touchSPI.beginTransaction(SPISettings(2500000, MSBFIRST, SPI_MODE0));
    digitalWrite(XPT2046_CS, LOW);

    // Read X coordinate
    touchSPI.transfer(0x90); // Start, A2-A0=001 (Y position), Mode=0, SER/DFR=1, PD1-PD0=00
    uint16_t rawX = touchSPI.transfer(0x00) << 8;
    rawX |= touchSPI.transfer(0x00);
    rawX >>= 3;

    // Read Y coordinate
    touchSPI.transfer(0xD0); // Start, A2-A0=101 (X position), Mode=0, SER/DFR=1, PD1-PD0=00
    uint16_t rawY = touchSPI.transfer(0x00) << 8;
    rawY |= touchSPI.transfer(0x00);
    rawY >>= 3;

    digitalWrite(XPT2046_CS, HIGH);
    touchSPI.endTransaction();

    // Calibration and mapping for CYD (adjust these values if needed)
    // These values are typical for the CYD but may need fine-tuning
    x = map(rawX, 200, 3700, 0, 320);
    y = map(rawY, 200, 3700, 0, 240);
    
    // Rotation adjustment (CYD is typically rotated)
    uint16_t temp = x;
    x = 320 - y;
    y = temp;

    return true;
}

/**
 * Draw a button on the display
 */
void drawButton(Button &btn) {
    uint16_t color = btn.pressed ? BUTTON_PRESSED_COLOR : BUTTON_COLOR;
    
    tft.fillRoundRect(btn.x, btn.y, btn.w, btn.h, 8, color);
    tft.drawRoundRect(btn.x, btn.y, btn.w, btn.h, 8, TFT_WHITE);
    
    tft.setTextColor(BUTTON_TEXT_COLOR);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    tft.drawString(btn.label, btn.x + btn.w / 2, btn.y + btn.h / 2);
}

/**
 * Initialize buttons with labels and commands
 */
void initButtons() {
    // Calculate button dimensions
    int totalWidth = tft.width() - (2 * BUTTON_MARGIN) - ((BUTTON_COLS - 1) * BUTTON_SPACING);
    int totalHeight = tft.height() - (2 * BUTTON_MARGIN) - ((BUTTON_ROWS - 1) * BUTTON_SPACING);
    buttonWidth = totalWidth / BUTTON_COLS;
    buttonHeight = totalHeight / BUTTON_ROWS;

    // Define button labels and commands for HDMI matrix switch
    // Modify these commands according to your HDMI matrix switch protocol
    String labels[BUTTON_COUNT] = {"Home", "Work", "OrangePi", "Cable", "Scene 5", "Scene 6"};
    String commands[BUTTON_COUNT] = {
        "MT00SW0101NT\r\n",  // Switch output 1 to input 1
        "MT00SW0102NT\r\n",  // Switch output 1 to input 2
        "MT00SW0103NT\r\n",  // Switch output 1 to input 3
        "MT00SW0104NT\r\n",  // Switch output 1 to input 4
        "MT00SW0105NT\r\n",  // Switch output 1 to input 5
        "MT00SW0106NT\r\n"   // Switch output 1 to input 6
    };

    // Create buttons in a grid layout
    int index = 0;
    for (int row = 0; row < BUTTON_ROWS; row++) {
        for (int col = 0; col < BUTTON_COLS; col++) {
            if (index < BUTTON_COUNT) {
                buttons[index].x = BUTTON_MARGIN + col * (buttonWidth + BUTTON_SPACING);
                buttons[index].y = BUTTON_MARGIN + row * (buttonHeight + BUTTON_SPACING);
                buttons[index].w = buttonWidth;
                buttons[index].h = buttonHeight;
                buttons[index].id = index + 1;
                buttons[index].label = labels[index];
                buttons[index].command = commands[index];
                buttons[index].pressed = false;
                
                drawButton(buttons[index]);
                index++;
            }
        }
    }
}

/**
 * Check if a touch point is within a button
 */
int checkButtonPress(uint16_t x, uint16_t y) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (x >= buttons[i].x && x <= (buttons[i].x + buttons[i].w) &&
            y >= buttons[i].y && y <= (buttons[i].y + buttons[i].h)) {
            return i;
        }
    }
    return -1;
}

/**
 * Send command to HDMI matrix switch
 */
void sendMatrixCommand(String command) {
    MATRIX_SERIAL.print(command);
    Serial.println("Sent command: " + command);
}

/**
 * Setup function
 */
void setup() {
    // Initialize USB serial for debugging
    Serial.begin(115200);
    Serial.println("ESP32 CYD HDMI Matrix Controller Starting...");

    // Initialize matrix switch serial interface
    MATRIX_SERIAL.begin(MATRIX_BAUD, SERIAL_8N1, MATRIX_RX, MATRIX_TX);
    Serial.println("Matrix Serial initialized at 115200 baud (8N1)");

    // Initialize TFT display
    tft.init();
    tft.setRotation(1); // Landscape orientation
    tft.fillScreen(BACKGROUND_COLOR);
    
    // Set backlight to full brightness
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Initialize touch controller
    initTouch();
    
    // Draw title
    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextDatum(TC_DATUM);
    tft.setTextSize(2);
    tft.drawString("HDMI Matrix Control", tft.width() / 2, 5);

    delay(500);

    // Initialize and draw buttons
    initButtons();
    
    Serial.println("Setup complete!");
}

/**
 * Main loop
 */
void loop() {
    static int lastButton = -1;
    static unsigned long lastTouchTime = 0;
    static bool touchHandled = false;
    
    // Read touch input
    if (getTouch(touchX, touchY)) {
        unsigned long currentTime = millis();
        
        // Debounce touch input (wait 200ms between touches)
        if (currentTime - lastTouchTime > 200 && !touchHandled) {
            int buttonIndex = checkButtonPress(touchX, touchY);
            
            if (buttonIndex >= 0) {
                Serial.printf("Button %d pressed at (%d, %d)\n", buttonIndex + 1, touchX, touchY);
                
                // Visual feedback
                buttons[buttonIndex].pressed = true;
                drawButton(buttons[buttonIndex]);
                
                // Send command to HDMI matrix switch
                sendMatrixCommand(buttons[buttonIndex].command);
                
                lastButton = buttonIndex;
                touchHandled = true;
            }
            
            lastTouchTime = currentTime;
        }
    } else {
        // Touch released
        if (touchHandled && lastButton >= 0) {
            delay(150); // Keep button highlighted briefly
            buttons[lastButton].pressed = false;
            drawButton(buttons[lastButton]);
            lastButton = -1;
        }
        touchHandled = false;
    }
    
    // Small delay to prevent excessive polling
    delay(10);
}
