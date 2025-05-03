/**
 * Rotary Encoder Test Sketch
 * 
 * This code tests if a rotary encoder is working properly by displaying
 * its position values in the Serial Monitor.
 */

#include <Arduino.h>
#include <RotaryEncoder.h>

// Rotary Encoder pins
#define PIN_IN1 2  // Data pin
#define PIN_IN2 4  // Clock pin

// Rotary encoder settings
#define ROTARYSTEPS 1
#define ROTARYMIN 0
#define ROTARYMAX 16  // Maximum limit for testing

// Create the encoder object with TWO03 LatchMode for better response
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

// Last known rotary position
int lastPos = -1;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  Serial.println("Rotary Encoder Test Sketch");
  Serial.println("-------------------------");
  Serial.println("Turn the encoder knob to see position changes");
  
  // Set initial rotary encoder position to middle of range
  encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
}

void loop() {
  // Process rotary encoder input
  encoder.tick();
  
  // Get the current position
  int newPos = encoder.getPosition() * ROTARYSTEPS;
  
  // Enforce limits
  if (newPos < ROTARYMIN) {
    encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
    newPos = ROTARYMIN;
    Serial.println("Reached minimum limit");
  } else if (newPos > ROTARYMAX) {
    encoder.setPosition(ROTARYMAX / ROTARYSTEPS);
    newPos = ROTARYMAX;
    Serial.println("Reached maximum limit");
  }
  
  // If position has changed, display the new value
  if (lastPos != newPos) {
    Serial.print("Encoder Position: ");
    Serial.println(newPos);
    
    // Get rotation direction
    int direction = (int)(encoder.getDirection());
    if (direction < 0) {
      Serial.println("Direction: Counterclockwise");
    } else if (direction > 0) {
      Serial.println("Direction: Clockwise");
    }
    
    Serial.println("-------------------------");
    
    lastPos = newPos;
  }
  
  // Small delay to stabilize readings
  delay(10);
}