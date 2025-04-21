#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// LED strip configuration
#define NUM_LEDS 32      // Total number of LEDs in the strip
#define LED_PIN 8        // The pin your LED strip is connected to
#define BRIGHTNESS 1  // Global brightness (0-255)
#define DISPLAY_BRIGHTNESS 5  // Brightness for the 7-segment display

// Timer variables
unsigned long timerStartTime = 0;
const unsigned long countdownDuration = 120000; // 2 minutes in milliseconds
const unsigned long updateInterval = 1000;      // Update every second
unsigned long lastUpdateTime = 0;
bool timerRunning = false;

// Create an array for the LEDs
CRGB leds[NUM_LEDS];

// Create display object
Adafruit_7segment display = Adafruit_7segment();

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize the LED strip
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Initialize the 7-segment display
  display.begin(0x70);
  display.setBrightness(DISPLAY_BRIGHTNESS);
  
  // Turn on all LEDs with a soft orange color
  // RGB values for soft orange (255, 127, 80)
  allOn(CRGB(255, 127, 80));
  
  // Show initial timer display (2:00)
  displayTime(2, 0);
  
  Serial.println("All LEDs turned on with soft orange color and 2-minute countdown ready!");
  Serial.println("Press 's' to start/reset the countdown timer");
}

void loop() {
  // Update timer if running
  if (timerRunning) {
    unsigned long currentTime = millis();
    
    // Calculate elapsed time since timer started
    unsigned long elapsedTime = currentTime - timerStartTime;
    
    // Check if countdown is complete
    if (elapsedTime >= countdownDuration) {
      // Timer finished
      timerRunning = false;
      
      // Display 0:00 and flash the LEDs
      displayTime(0, 0);
      flashLEDs();
      
      // Reset display to 2:00
      displayTime(2, 0);
      
      Serial.println("Countdown complete!");
    } 
    // Update display every second
    else if (currentTime - lastUpdateTime >= updateInterval) {
      // Calculate remaining time
      unsigned long remainingTime = countdownDuration - elapsedTime;
      int minutes = remainingTime / 60000;
      int seconds = (remainingTime % 60000) / 1000;
      
      // Update the display
      displayTime(minutes, seconds);
      
      // Update last update time
      lastUpdateTime = currentTime;
    }
  }
  
  // Check if data is available from Serial (for control)
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    // Process commands
    switch(cmd) {
      case 'r': // Red
        allOn(CRGB::Red);
        break;
      case 'g': // Green
        allOn(CRGB::Green);
        break;
      case 'b': // Blue
        allOn(CRGB::Blue);
        break;
      case 'w': // White
        allOn(CRGB::White);
        break;
      case 'o': // Back to soft orange
        allOn(CRGB(255, 127, 80));
        break;
      case 's': // Start/reset timer
        startTimer();
        break;
    }
    
    // Clear any remaining characters
    while(Serial.available() > 0) {
      Serial.read();
    }
  }
  
  // Small delay to prevent excessive looping
  delay(10);
}

// Function to start/reset the timer
void startTimer() {
  timerStartTime = millis();
  lastUpdateTime = timerStartTime;
  timerRunning = true;
  
  // Reset display to 2:00
  displayTime(2, 0);
  
  Serial.println("Countdown timer started!");
}

// Function to turn on all LEDs with a specified color
void allOn(CRGB color) {
  // Set all LEDs to the specified color
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  
  // Update the LED strip
  FastLED.show();
  
  Serial.print("All LEDs set to color: ");
  Serial.print(color.r); Serial.print(",");
  Serial.print(color.g); Serial.print(",");
  Serial.println(color.b);
}

// Function to display time on the 7-segment display
void displayTime(int minutes, int seconds) {
  // Clear the display
  display.clear();
  
  // Set minutes digit (only need one digit for 0-9)
  display.writeDigitNum(0, minutes, false);
  
  // Set seconds digits (tens and ones positions)
  display.writeDigitNum(3, seconds / 10, false);  // Tens digit
  display.writeDigitNum(4, seconds % 10, false);  // Ones digit
  
  // Turn on colon between minutes and seconds
  display.drawColon(true);
  
  // Update the display
  display.writeDisplay();
  
  // Print to serial monitor for debugging
  Serial.print("Countdown: ");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.println(seconds);
}

// Function to flash LEDs when countdown completes
void flashLEDs() {
  // Store original color
  CRGB originalColor = leds[0];
  
  // Flash between white and off 5 times
  for (int i = 0; i < 5; i++) {
    // Turn all LEDs white
    allOn(CRGB::White);
    delay(250);
    
    // Turn all LEDs off
    allOn(CRGB::Black);
    delay(250);
  }
  
  // Restore original color
  allOn(originalColor);
}