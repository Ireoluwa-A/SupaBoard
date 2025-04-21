#include <FastLED.h>

// Define the number of LEDs in your strip
#define NUM_LEDS 28      // Adjusted to match your strip length
#define LED_PIN 8        // The pin your LED strip is connected to
#define HEIGHT 10
#define WIDTH 4
#define ANIMATION_DELAY 30  // Delay between each LED lighting up (milliseconds)
#define FLASH_DELAY 800     // Delay for flashing animation (milliseconds)
#define FLASH_COUNT 6       // Number of flashes

// Add global brightness control (0-255)
#define BRIGHTNESS 100      // Lower value = dimmer LEDs (max is 255)

// Create an array for the LEDs
CRGB leds[NUM_LEDS];

// Rainbow colors for RGB flash
CRGB rainbowColors[] = {
  CRGB::Yellow
};
const int numRainbowColors = 1;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Menu Options:");
  Serial.println("0: Orange sequential underline");
  Serial.println("1: Orange sequential all LEDs");
  Serial.println("3: Red flash all LEDs");
  Serial.println("4: Red flash underline only");
  Serial.println("6: RGB flash all LEDs");
  Serial.println("7: RGB flash underline only");
  Serial.println("8: Set brightness (0-255)");  // New option
  
  // Initialize the LED strip - WS2812B is common, but adjust if needed
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  // Set the global brightness
  FastLED.setBrightness(BRIGHTNESS);
  
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
    
    // Process the input
    switch(num) {
      case 0:  // Orange sequential underline
        allOff();
        animateGroup(false, CRGB(255, 165, 0));
        Serial.println("Lighting underline");
        break;
        
      case 1:  // Orange sequential all LEDs
        allOff();
        animateGroup(true, CRGB(255, 165, 0));
        Serial.println("Lighting all LEDs");
        break;
        
      case 3:  // Red flash all LEDs
        flashAnimation(true, CRGB::Red, false);
        Serial.println("Flashing all LEDs red");
        break;
        
      case 4:  // Red flash underline only
        flashAnimation(false, CRGB::Red, false);
        Serial.println("Flashing underline red");
        break;
        
      case 6:  // RGB flash all LEDs
        flashAnimation(true, CRGB::Black, true);  // Color doesn't matter when rainbow=true
        Serial.println("RGB flashing all LEDs");
        break;
        
      case 7:  // RGB flash underline only
        flashAnimation(false, CRGB::Black, true);  // Color doesn't matter when rainbow=true
        Serial.println("RGB flashing underline");
        break;
        
      case 8:  // Set brightness (new case)
        int brightness = Serial.parseInt();
        brightness = constrain(brightness, 0, 255);  // Ensure it's within valid range
        FastLED.setBrightness(brightness);
        FastLED.show();
        Serial.print("Brightness set to: ");
        Serial.println(brightness);
        break;
        
      default:
        Serial.println("Invalid option. Please choose 0, 1, 3, 4, 6, 7, or 8");
        break;
    }
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

// Function to light LEDs sequentially with animation
void animateGroup(bool allLEDs, CRGB color) {
  // Determine which LEDs to light based on the parameter
  int startLED, endLED;
  
  if (allLEDs) {
    startLED = 0;
    endLED = NUM_LEDS;
  } else {
    // Underline only
    startLED = WIDTH;
    endLED = WIDTH + HEIGHT;
  }
  
  // Light each LED one by one with a delay
  for(int i = startLED; i < endLED; i++) {
    leds[i] = color;
    FastLED.show();
    delay(ANIMATION_DELAY);
  }
}

// Function for flashing animation
void flashAnimation(bool allLEDs, CRGB color, bool rainbow) {
  // Determine which LEDs to flash
  int startLED, endLED;
  
  if (allLEDs) {
    startLED = 0;
    endLED = NUM_LEDS;
  } else {
    // Underline only
    startLED = WIDTH;
    endLED = WIDTH + HEIGHT;
  }

  // Flash LEDs on and off
  for(int i = 0; i < FLASH_COUNT; i++) {
    // Select color for this flash
    CRGB flashColor = color;
    if (rainbow) {
      flashColor = rainbowColors[i % numRainbowColors];
    }
    
    // Turn target LEDs on with the specified color
    for(int j = startLED; j < endLED; j++) {
      leds[j] = flashColor;
    }
    FastLED.show();
    delay(FLASH_DELAY);
    
    // Turn all LEDs off
    allOff();
    delay(FLASH_DELAY);
  }
}