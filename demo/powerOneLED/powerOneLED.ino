/**
 * Description: Program to turn off all LEDs and then power a specific group of LEDs
 * with a color at reduced brightness
 */

#include <FastLED.h>

// Define the LED strip configuration
#define NUM_GROUP 5
#define TOTAL_LEDS 32 * NUM_GROUP   // Total LEDs in the strip
#define LED_PIN 7      // Pin connected to the LED strip
#define BRIGHTNESS 10  // Reduced brightness level (0-255)

// Create the LED array for the full strip
CRGB leds[TOTAL_LEDS];

// Variable to choose which group to light up (0-based index)
int activeGroup = 2;  // Change this value to select different groups (0 to NUM_GROUP-1)

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  
  // Initialize the LED strip for all LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // First turn OFF all LEDs
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  
  delay(500); // Brief pause so you can see they all turned off
  
  // Calculate the starting LED index for the chosen group
  int startLED = activeGroup * 32;
  
  // Now turn ON just the specified group of LEDs with red color
  for(int i = startLED; i < startLED + 32; i++) {
    leds[i] = CRGB::Red;
  }
  
  // Display the LEDs
  FastLED.show();
  
  Serial.println("Initialized: All LEDs turned off, then group " + String(activeGroup) + " set to red");
}

void loop() {
  // Nothing to do here as we just want to keep the LEDs on
}