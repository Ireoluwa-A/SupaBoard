#include <Arduino.h>
#include "Joystick.h"
#include "Button.h"
#include <PololuLedStrip.h>
#include <LiquidCrystal_I2C.h>


// Define the number of LEDs in your strip
#define LED_PIN 6   // The pin your LED strip is connected to

#define GROUP_SIZE 10 // Number of LEDs per group
#define NUM_GROUPS 4 // Total number of groups
#define NUM_LEDS (NUM_GROUPS * GROUP_SIZE) // Total number of LEDs

#define JOYSTICK_X_PIN A1
#define JOYSTICK_Y_PIN A0
#define JOYSTICK_BUTTON_PIN 2

unsigned long lastVerticalAction = 0;
unsigned long lastHorizontalAction = 0;

const unsigned long verticalCooldown = 500;   // 500ms cooldown for vertical actions
const unsigned long horizontalCooldown = 250; // 200ms cooldown for horizontal actions


// Create an instance of the PololuLedStrip
PololuLedStrip<LED_PIN> ledStrip;
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Create a buffer for holding the colors (RGB values)
rgb_color colors[NUM_LEDS];

bool selectingHours = true;

//TODO Build struct around this
uint8_t hours = 0;
uint8_t minutes = 0;

Joystick joystick = Joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);
Button joystickButton = Button(JOYSTICK_BUTTON_PIN);

uint8_t cell = 0;

void display() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm Clock");
    lcd.setCursor(0, 1);
    lcd.print("Set Alarm: ");
    lcd.print(hours);
    lcd.print(":");
    lcd.print(minutes);
    lcd.setCursor(0, 2);
    lcd.print("Selecting: ");
    lcd.print(selectingHours ? "Hours" : "Minutes");
    lcd.setCursor(0, 3);
    lcd.print("Group: ");
    lcd.print(cell);
    lcd.display();
}

void setupLCD() {
    lcd.init();
    lcd.backlight();
    lcd.home();
    lcd.print("Loading...");
}

void logAlarm() {
    bool isAM = hours < 12;
    uint8_t displayHours = hours;
    if (hours == 0) {
        displayHours = 12;
    } else if (hours > 12) {
        displayHours -= 12;
    }
    Serial.print(displayHours);
    Serial.print(":");
    char buffer[4];
    sprintf(buffer, "%02d", minutes);
    Serial.print(minutes == 0 ? "00" : buffer);
    Serial.println(isAM ? "AM" : "PM");
}

void lightGroup(int groupNum, rgb_color color) {
    int startLED = groupNum * GROUP_SIZE;
    int endLED = startLED + GROUP_SIZE;
    
    // Make sure we don't try to access LEDs beyond the strip length
    endLED = min(endLED, NUM_LEDS);
    
    for(int i = startLED; i < endLED; i++) {
      colors[i] = color;
    }
}

void allOff() {
    for(int i = 0; i < NUM_LEDS; i++) {
      colors[i] = (rgb_color){0, 0, 0}; // Black (off)
    }
    ledStrip.write(colors, NUM_LEDS);
}

// Helper function to check cooldown
bool canPerformAction(unsigned long &lastActionTime, unsigned long cooldown) {
    unsigned long currentTime = millis();
    if (currentTime - lastActionTime >= cooldown) {
        lastActionTime = currentTime;
        return true;
    }
    return false;
}

void setup() {
    Serial.begin(9600);
    setupLCD();
    display();
    allOff();
    lightGroup(cell, (rgb_color){255, 165, 0}); // Orange color (RGB: 255, 165, 0)
    ledStrip.write(colors, NUM_LEDS);
}

void loop() {

    JoystickState state = joystick.getState();
    ButtonEvent buttonEvent = joystickButton.getEvent();

    bool refreshDisplay = false;
    bool refreshLEDs = false;

    uint8_t prevCell = cell;
    uint8_t prevHours = hours;
    uint8_t prevMinutes = minutes;

    if (buttonEvent == ButtonEvent::Click) {
        if (selectingHours) {
            selectingHours = false;
        } else {
            selectingHours = true;
        }
        refreshDisplay = true;
    }

    switch (state)
    {
    case JoystickState::Down:
        if (canPerformAction(lastVerticalAction, verticalCooldown)) {
            cell = min(cell + 1, NUM_GROUPS - 1);

        }
        break;

    case JoystickState::Up:
        if (canPerformAction(lastVerticalAction, verticalCooldown)) {
            cell = max(cell - 1, 0);
        }
        break;
    case JoystickState::Left:
        if (canPerformAction(lastHorizontalAction, horizontalCooldown)) {
            if (selectingHours) {
                hours = max(hours - 1, 0);
            } else {
                minutes = max(minutes - 5, 0);
            }
        }
        break;
    case JoystickState::Right:
        if (canPerformAction(lastHorizontalAction, horizontalCooldown)) {
            if (selectingHours) {
                hours = min(hours + 1, 23);
            } else {
                minutes = min(minutes + 5, 55);
            }
        }
        break;
    default:
        break;
    }

    if (prevCell != cell) {
        refreshLEDs = true;
    }

    if (prevHours != hours || prevMinutes != minutes) {
        refreshDisplay = true;
    }
    refreshDisplay = refreshDisplay || refreshLEDs;

    if (refreshDisplay) {
        display();
    }
    if (refreshLEDs) {
        allOff();
        lightGroup(cell, (rgb_color){255, 165, 0}); // Orange color (RGB: 255, 165, 0)
        ledStrip.write(colors, NUM_LEDS);
    }
}
