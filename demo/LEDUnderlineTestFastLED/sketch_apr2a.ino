#include <FastLED.h>

// Define the number of LEDs in your strip
#define NUM_LEDS 28      // Adjusted to match your strip length
#define LED_PIN 8        // The pin your LED strip is connected to
#define HEIGHT 10
#define WIDTH 4

// Create an array for the LEDs
CRGB leds[NUM_LEDS];

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Enter 0 for underline and 1 for all LEDs");
  
  // Initialize the LED strip - WS2812B is common, but adjust if needed
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  // Turn all LEDs off at startup
  allOff();
}

void loop() {
  // Check if data is available to read from Serial
  if (Serial.available() > 0) {
    // Read the incoming byte
    int num = Serial.parseInt();
    
    // Clear any remaining characters
    while(Serial.available() > 0) {
      Serial.read();
    }
    
    // Validate input
    if(num == 0 || num == 1) {
      // Turn all LEDs off first
      allOff();
      
      // Light up the selected group with orange color
      lightGroup(num, CRGB(255, 165, 0)); // Orange color (RGB: 255, 165, 0)
      
      Serial.println("Lighting " + String(num));
    } else {
      Serial.println("Invalid number. Enter a value between 0 and 1");
    }
    
    // Show the updated LED colors
    FastLED.show();
  }
  
  // Small delay to prevent excessive looping
  delay(50);
}

// Function to turn off all LEDs
void allOff() {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black; // Black (off)
  }
  FastLED.show();
}

void lightGroup(int groupNum, CRGB color) {
  // if group num = all LEDS
  int startLED = groupNum ? 0 : WIDTH;
  int endLED = groupNum ? NUM_LEDS : WIDTH + HEIGHT;
  
  for(int i = startLED; i < endLED; i++) {
    leds[i] = color;
  }
}