#include <Arduino.h>
#include "Joystick.h"
#include <PololuLedStrip.h>


// Define the number of LEDs in your strip
#define NUM_LEDS 20  // Adjusted to match your strip length
#define LED_PIN 6   // The pin your LED strip is connected to
#define GROUP_SIZE 10 // Number of LEDs per group

// Create an instance of the PololuLedStrip
PololuLedStrip<LED_PIN> ledStrip;

// Create a buffer for holding the colors (RGB values)
rgb_color colors[NUM_LEDS];

Joystick joystick = Joystick(A1, A0, 2);
int cell = false;

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

void setup() {
    Serial.begin(9600);
}

void loop() {
    JoystickState state = joystick.getState();
    switch (state)
    {
    case JoystickState::Up:
        cell = min(cell + 1, 1);
        /* code */
        break;
    case JoystickState::Down:
        cell = max(cell - 1, 0);
        /* code */
        break;
    default:
        break;
    }
    delay(150);
    allOff();
    lightGroup(cell, (rgb_color){255, 165, 0}); // Orange color (RGB: 255, 165, 0)

    ledStrip.write(colors, NUM_LEDS);

}
