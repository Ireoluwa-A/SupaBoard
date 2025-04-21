/**
 * Description: Program to sequentially light up each LED in a cell
 * A single cell consists of 32 LEDs
 */

#include <FastLED.h>

// Define the LED strip configuration
#define NUM_GROUP 5
#define LEDS_PER_CELL 32           // Number of LEDs in a single cell
#define TOTAL_LEDS 32 * NUM_GROUP  // Total LEDs in the strip
#define LED_PIN 8                  // Pin connected to the LED strip
#define BRIGHTNESS 130             // Brightness level (0-255)
#define ANIMATION_SPEED 10         // Delay between lighting each LED (milliseconds)

// Create the LED array for the full strip
CRGB leds[TOTAL_LEDS];

// Variable to track the currently active LED
int currentLed = 0;
int currentCell = 0;

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  
  // Initialize the LED strip
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Turn OFF all LEDs initially
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  
  Serial.println("Initialized: Sequential LED animation ready");
}

void loop() {
  // // Start with all LEDs off
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  delay(1000); // Wait a second before starting the animation
  
  // For each cell
  for (int cell = 0; cell < NUM_GROUP; cell++) {
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
    // Pause to show the fully lit cell
    // delay(1000);
    
    // Turn off all LEDs in this cell before moving to the next
    // for (int i = cell * LEDS_PER_CELL; i < (cell + 1) * LEDS_PER_CELL; i++) {
    //   leds[i] = CRGB::Black;
    // }
    // FastLED.show();
    delay(200); // Brief pause before starting the next cell
  }
}