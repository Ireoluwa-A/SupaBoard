#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// Define the maximum number of displays we want to support
#define MAX_DISPLAYS 5

// Array of display objects
Adafruit_7segment displays[MAX_DISPLAYS];

// Number of active displays (currently 2, but can be expanded up to MAX_DISPLAYS)
int numDisplays = 2;

// Array of I2C addresses for each display
const uint8_t displayAddresses[MAX_DISPLAYS] = {0x70, 0x71, 0x72, 0x73, 0x74};

void setup() {
  // Initialize all active displays
  for (int i = 0; i < numDisplays; i++) {
    displays[i].begin(displayAddresses[i]);
    displays[i].setBrightness(20);
  }
  
  // Initialize random seed using an unconnected analog pin
  randomSeed(analogRead(0));
  
  Serial.begin(9600);
  Serial.println("Random Time Display initialized!");
  Serial.print("Number of active displays: ");
  Serial.println(numDisplays);
}

void loop() {
  // Generate random times for each display
  for (int i = 0; i < numDisplays; i++) {
    // Generate random hours (0-23)
    int hours = random(24);
    
    // Generate random minutes (0-59)
    int minutes = random(60);
    
    // Format and display the time on the specific display
    displayTime(i, hours, minutes);
  }
  
  // Wait 3 seconds before showing new random times
  delay(3000);
}

void displayTime(int displayIndex, int hours, int minutes) {
  // Make sure the display index is valid
  if (displayIndex < 0 || displayIndex >= numDisplays) {
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