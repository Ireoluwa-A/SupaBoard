/**
 * Combined program to control 7-segment displays and LED strips
 * - Lights up LEDs in groups of 32
 * - Displays random time on corresponding 7-segment display
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <FastLED.h>

// Define the LED strip configuration
#define NUM_GROUP 5                // Number of groups/cells
#define LEDS_PER_CELL 32           // Number of LEDs in a single cell
#define TOTAL_LEDS 32 * NUM_GROUP  // Total LEDs in the strip
#define LED_PIN 8                  // Pin connected to the LED strip
#define BRIGHTNESS 130             // Brightness level (0-255)
#define ANIMATION_SPEED 10         // Delay between lighting each LED (milliseconds)

// Create the LED array for the full strip
CRGB leds[TOTAL_LEDS];

// Define the maximum number of displays
#define MAX_DISPLAYS 5

// Array of display objects (one per group)
Adafruit_7segment displays[MAX_DISPLAYS];

// Array of I2C addresses for each display
const uint8_t displayAddresses[MAX_DISPLAYS] = {0x70, 0x71, 0x72, 0x73, 0x74};

// Arrays to store the current random time for each display
int displayHours[MAX_DISPLAYS];
int displayMinutes[MAX_DISPLAYS];

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  
  // Initialize the LED strip
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Turn OFF all LEDs initially
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  
  // Initialize random seed using an unconnected analog pin
  randomSeed(analogRead(0));
  
  // Initialize all displays
  for (int i = 0; i < NUM_GROUP; i++) {
    displays[i].begin(displayAddresses[i]);
    displays[i].setBrightness(20);
    displays[i].clear();
    displays[i].writeDisplay();
  }
  
  Serial.println("Combined LED and 7-Segment Display System Initialized");
}

void loop() {
  // Start with all LEDs and displays off
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  
  for (int i = 0; i < NUM_GROUP; i++) {
    displays[i].clear();
    displays[i].writeDisplay();
  }
  
  delay(1000); // Wait a second before starting the animation
  
  // For each cell/group
  for (int cell = 0; cell < NUM_GROUP; cell++) {
    // Generate random time for this display
    int hours = random(24);
    int minutes = random(60);
    
    // Store the time values
    displayHours[cell] = hours;
    displayMinutes[cell] = minutes;
    
    // Display the time on the corresponding 7-segment display
    displayTime(cell, hours, minutes);
    
    // Sequentially light up each LED in the current cell
    for (int led = 0; led < LEDS_PER_CELL; led++) {
      // Calculate the LED index
      int ledIndex = (cell * LEDS_PER_CELL) + led;
      
      // Light up this LED (keeping previous LEDs lit)
      leds[ledIndex] = CRGB::Red;
      
      // Show the updated pattern
      FastLED.show();
      
      // Wait before lighting the next LED
      delay(ANIMATION_SPEED);
    }
    
    // All LEDs in the cell are now lit
    delay(200); // Brief pause before starting the next cell
  }
  
  // Wait a while with all LEDs and displays lit
  delay(3000);
  
  // Optional: turn everything off before restarting
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  
  for (int i = 0; i < NUM_GROUP; i++) {
    displays[i].clear();
    displays[i].writeDisplay();
  }
  
  delay(1000);
}

void displayTime(int displayIndex, int hours, int minutes) {
  // Make sure the display index is valid
  if (displayIndex < 0 || displayIndex >= NUM_GROUP) {
    return;
  }
  
  // Clear the display
  displays[displayIndex].clear();
  
  // Convert 24-hour format to 12-hour if needed (optional)
  int displayHours = hours;
  bool isPM = false;
  
  if (hours > 12) {
    displayHours = hours - 12;
    isPM = true;
  } else if (hours == 0) {
    displayHours = 12;
  } else if (hours == 12) {
    isPM = true;
  }
  
  // Position for hours: 0 and 1
  displays[displayIndex].writeDigitNum(0, displayHours / 10, false);  // First digit of hour (might be blank for 1-9)
  displays[displayIndex].writeDigitNum(1, displayHours % 10, false);
  
  // Position for minutes: 3 and 4
  displays[displayIndex].writeDigitNum(3, minutes / 10, false);
  displays[displayIndex].writeDigitNum(4, minutes % 10, false);
  
  // Turn on colon between hours and minutes
  displays[displayIndex].drawColon(true);
  
  // Update the display
  displays[displayIndex].writeDisplay();
  
  // Print to serial monitor for debugging
  Serial.print("Display #");
  Serial.print(displayIndex);
  Serial.print(" (0x");
  Serial.print(displayAddresses[displayIndex], HEX);
  Serial.print(") Random Time: ");
  Serial.print(displayHours);
  Serial.print(":");
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.println(isPM ? " PM" : " AM");
}