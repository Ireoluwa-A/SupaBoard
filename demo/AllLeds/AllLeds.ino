/**
 * Description: Program to turn off all 64 LEDs and then power just 32 LEDs
 * with white color at reduced brightness
 */

#include <FastLED.h>

// Define the LED strip configuration
#define NUM_GROUP 5
#define TOTAL_LEDS 32 * NUM_GROUP   // Total LEDs in the strip
#define ACTIVE_LEDS 32 * NUM_GROUP // Number of LEDs to actually power
#define LED_PIN 8      // Pin connected to the LED strip
#define BRIGHTNESS 255  // Reduced brightness level (0-255)

// Create the LED array for the full strip
CRGB leds[TOTAL_LEDS];

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  
  // Initialize the LED strip for all 64 LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // First turn OFF all 64 LEDs
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  
  delay(500); // Brief pause so you can see they all turned off
  
  // Now turn ON just the specified 32 LEDs with white color
  fill_solid(leds, ACTIVE_LEDS, CRGB::Red);
  
  // Display the LEDs
  FastLED.show();
  
  Serial.println("Initialized: All 64 LEDs turned off, then 32 LEDs set to white");
}

void loop() {
  // Nothing to do here as we just want to keep the LEDs on
}