/**
 * Config.h
 * Configuration settings, pin mappings, and constants
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <FastLED.h>

// Display configuration
#define MAX_DISPLAYS 5
const uint8_t DISPLAY_ADDRESSES[MAX_DISPLAYS] = {0x70, 0x71, 0x72, 0x73, 0x74};

// Button Pins
#define SELECT_BUTTON_PIN 11
#define TOGGLE_BUTTON_PIN 12

// Rotary Encoder pins
#define ENCODER_PIN1 2  // Data pin
#define ENCODER_PIN2 4  // Clock pin

// LED Strip pin
#define LED_PIN 7

// LED strip configuration
#define NUM_GROUPS 5
#define LEDS_PER_GROUP 32
#define TOTAL_LEDS (LEDS_PER_GROUP * NUM_GROUPS)
#define BRIGHTNESS 100  // Brightness level (0-255)

// Rotary encoder settings
#define ROTARY_STEPS 1

// Colors
#define DEFAULT_COLOR CRGB::Red
#define HIGHLIGHT_COLOR CRGB::Orange
#define ALARM_COLOR CRGB::Red
#define OFF_COLOR CRGB::Black
#define CONFIRM_COLOR CRGB::Green // Color for confirmation flashes

// Time setting modes
#define MODE_NORMAL 0
#define MODE_SET_HOUR 1
#define MODE_SET_MINUTE 2

// Triple flash confirmation
#define CONFIRMATION_FLASH_COUNT 3
#define FLASH_INTERVAL 150 // 150ms interval for confirmation flashes

// Timing constants
#define BLINK_INTERVAL 500 // 500ms blink interval for time setting
#define ALARM_BLINK_INTERVAL 250 // 250ms for faster alarm blinking
#define ACTIVITY_TIMEOUT 1000 // 1 second timeout for encoder activity
#define TIME_UPDATE_INTERVAL 1000 // 1 second interval for updating time displays
#define DEBOUNCE_DELAY 50 // 50ms delay for button debouncing

#endif // CONFIG_H