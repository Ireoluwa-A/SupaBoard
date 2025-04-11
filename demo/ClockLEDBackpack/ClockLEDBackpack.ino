#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// Create display object
Adafruit_7segment display = Adafruit_7segment();

void setup() {
  // Initialize the display (uses default I2C address 0x70)
  display.begin(0x70);
  
  display.setBrightness(20);
  
  // Initialize random seed using an unconnected analog pin
  randomSeed(analogRead(0));
  
  Serial.begin(9600);
  Serial.println("Random Time Display initialized!");
}

void loop() {
  // Generate random hours (0-23)
  int hours = random(24);
  
  // Generate random minutes (0-59)
  int minutes = random(60);
  
  // Format and display the time
  displayTime(hours, minutes);
  
  // Wait 3 seconds before showing a new random time
  delay(3000);
}

void displayTime(int hours, int minutes) {
  // Clear the display
  display.clear();
  
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
  display.writeDigitNum(0, displayHours / 10, false);  // First digit of hour (might be blank for 1-9)
  display.writeDigitNum(1, displayHours % 10, false);
  
  // Position for minutes: 3 and 4
  display.writeDigitNum(3, minutes / 10, false);
  display.writeDigitNum(4, minutes % 10, false);
  
  // Turn on colon between hours and minutes
  display.drawColon(true);
  
  // Update the display
  display.writeDisplay();
  
  // Print to serial monitor for debugging
  Serial.print("Random Time: ");
  Serial.print(displayHours);
  Serial.print(":");
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.println(isPM ? " PM" : " AM");
}