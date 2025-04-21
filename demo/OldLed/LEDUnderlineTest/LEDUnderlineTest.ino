#include <PololuLedStrip.h>

// Define the number of LEDs in your strip
#define NUM_LEDS 28  // Adjusted to match your strip length
#define LED_PIN 8   // The pin your LED strip is connected to
#define GROUP_SIZE 28 // Number of LEDs per group
#define HEIGHT 10
#define WIDTH 4

// Create an instance of the PololuLedStrip
PololuLedStrip<LED_PIN> ledStrip;

// Create a buffer for holding the colors (RGB values)
rgb_color colors[NUM_LEDS];

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Enter 0 for underline and 1 for all LEDs");
  
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
      lightGroup(num, (rgb_color){255, 165, 0}); // Orange color (RGB: 255, 165, 0)
      
      Serial.println("Lighting" + String(num));
    } else {
      Serial.println("Invalid number. Enter a value between 0 and 1");
    }
    
    // Show the updated LED colors
    ledStrip.write(colors, NUM_LEDS);
  }
  
  // Small delay to prevent excessive looping
  delay(50);
}

// Function to turn off all LEDs
void allOff() {
  for(int i = 0; i < NUM_LEDS; i++) {
    colors[i] = (rgb_color){0, 0, 0}; // Black (off)
  }
  ledStrip.write(colors, NUM_LEDS);
}


void lightGroup(int groupNum, rgb_color color) {
  int startLED = groupNum ? 0 : HEIGHT + WIDTH + HEIGHT;
  int endLED = NUM_LEDS;
  
  for(int i = startLED; i < endLED; i++) {
    colors[i] = color;
  }
}