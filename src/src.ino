/**
 * 1. Cycles through LED cell groups using a rotary encoder with wrap-around
 * 2. Highlights selected cell group in orange
 * 3. Shows current time on 7-segment display when a group is selected
 * 4. Allows setting time using rotary encoder and toggle/select buttons
 * 5. Flashes LED group red when set time is reached
 * 6. Triple flashes the LEDs when time is set
 */

#include "config.h"
 #include <FastLED.h>
 #include <RotaryEncoder.h>
 #include <Wire.h>
 #include <RTClib.h>
 #include <Adafruit_GFX.h>
 #include <Adafruit_LEDBackpack.h>

 
 bool confirmationFlashActive = false;
 int currentFlashCount = 0;
 unsigned long lastFlashTime = 0;
 int flashingGroupIndex = -1;
 
 // Create the LED array for the full strip
 CRGB leds[TOTAL_LEDS];
 
 // Create the encoder object with TWO03 LatchMode for better response
 RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
 
 // RTC object
 RTC_DS3231 rtc;
 
 // Array of 7-segment display objects
 Adafruit_7segment displays[MAX_DISPLAYS];
 
 // Last known rotary position
 int lastPos = -999;
 int selectedGroup = 0;
 
 // Currently active display (for showing time)
 int activeDisplay = -1;
 
 // Time setting variables
 int currentMode = MODE_NORMAL;
 int settingHours = 0;
 int settingMinutes = 0;
 unsigned long lastBlinkTime = 0;
 bool blinkState = false;
 const unsigned long BLINK_INTERVAL = 500; // 500ms blink interval
 
 // Activity tracking for encoder
 unsigned long lastEncoderActivity = 0;
 const unsigned long ACTIVITY_TIMEOUT = 1000; // 1 second timeout
 
 // Alarm time for each group (24-hour format)
 int alarmHours[MAX_DISPLAYS] = {-1, -1, -1, -1, -1}; // -1 means no alarm set
 int alarmMinutes[MAX_DISPLAYS] = {0, 0, 0, 0, 0};
 
 // Alarm state tracking
 bool alarmActive[MAX_DISPLAYS] = {false, false, false, false, false};
 unsigned long lastAlarmBlinkTime = 0;
 bool alarmBlinkState = false;
 const unsigned long ALARM_BLINK_INTERVAL = 250; // 250ms for faster alarm blinking
 
 // Time display flags
 bool timeDisplayOn[MAX_DISPLAYS] = {false, false, false, false, false};
 
 void setup() {
   // Initialize serial for debugging
   Serial.begin(9600);
   
   // Initialize button pins
   pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);
   pinMode(TOGGLE_BUTTON_PIN, INPUT_PULLUP);
   
   // Initialize RTC
   if (!rtc.begin()) {
     Serial.println("Couldn't find RTC");
     while (1);
   }
   
   // Set RTC to the current time (compilation time)
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   Serial.println("RTC time set to compilation time");
   
   // Initialize all 7-segment displays
   for (int i = 0; i < MAX_DISPLAYS; i++) {
     displays[i].begin(displayAddresses[i]);
     displays[i].setBrightness(20);
     displays[i].clear();
     displays[i].writeDisplay();
   }
   
   // Initialize the LED strip
   FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
   FastLED.setBrightness(BRIGHTNESS);
   
   // Turn OFF all LEDs initially and keep them off
   fill_solid(leds, TOTAL_LEDS, CRGB::Black);
   FastLED.show();
   
   // Set initial rotary encoder position
   encoder.setPosition(0);
   
   Serial.println("Initialized: All LEDs off, ready to cycle through groups with encoder");
 }
 
 void loop() {
   // Process rotary encoder input
   encoder.tick();
   
   // Check if any alarms should be triggered
   checkAlarms();
   
   // Handle confirmation flashes if active
   if (confirmationFlashActive) {
     handleConfirmationFlash();
   } else {
     // Only handle other modes if not flashing
     
     // Handle different modes
     switch (currentMode) {
       case MODE_NORMAL:
         handleNormalMode();
         break;
       case MODE_SET_HOUR:
         handleSetHourMode();
         break;
       case MODE_SET_MINUTE:
         handleSetMinuteMode();
         break;
     }
     
     // Handle button presses depending on current mode
     handleButtonPresses();
     
     // Update any active alarms (blinking)
     handleActiveAlarms();
     
     // Update any active time displays
     updateTimeDisplays();
   }
 }
 
 void handleNormalMode() {
   // Get the current position and apply step multiplier if needed
   long newPosition = encoder.getPosition() * ROTARYSTEPS;
   
   // If position has changed, update the selected group with wrap-around
   if (newPosition != lastPos) {
     // Calculate direction of rotation
     int direction = (newPosition > lastPos) ? 1 : -1;
     
     // Update selected group with wrap-around
     selectedGroup = (selectedGroup + direction) % NUM_GROUP;
     // Handle negative wrap-around
     if (selectedGroup < 0) selectedGroup += NUM_GROUP;
     
     Serial.print("Encoder Position: ");
     Serial.print(newPosition);
     Serial.print(", Selected Group: ");
     Serial.println(selectedGroup);
     
     // Reset display to default color if not in alarm state
     fill_solid(leds, TOTAL_LEDS, OFF_COLOR);
     for (int i = 0; i < NUM_GROUP; i++) {
       if (!alarmActive[i]) {
         // Fill group with default color
         for (int j = i * LEDS_PER_GROUP; j < (i + 1) * LEDS_PER_GROUP; j++) {
           leds[j] = DEFAULT_COLOR;
         }
       }
     }
     
     // Highlight the selected group
     highlightGroup(selectedGroup);
     
     lastPos = newPosition;
   }
 }
 
 void handleSetHourMode() {
   // Get the current position
   long newPosition = encoder.getPosition();
   
   // If position has changed, update the hours
   if (newPosition != lastPos) {
     // Calculate direction of rotation
     int direction = (newPosition > lastPos) ? 1 : -1;
     
     // Update hours with wrap-around (0-23)
     settingHours = (settingHours + direction) % 24;
     // Handle negative wrap-around
     if (settingHours < 0) settingHours += 24;
     
     Serial.print("Setting Hours: ");
     Serial.println(settingHours);
     
     lastPos = newPosition;
     
     // Update last activity time to indicate encoder is being used
     lastEncoderActivity = millis();
     
     // Show the time immediately when encoder is rotated
     displaySettingTime(activeDisplay, true);
   }
   
   // Only blink when the encoder hasn't been used for a while
   if (millis() - lastEncoderActivity >= ACTIVITY_TIMEOUT) {
     if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
       blinkState = !blinkState;
       lastBlinkTime = millis();
       
       // Display time with possibly blinking hours
       displaySettingTime(activeDisplay, blinkState);
     }
   }
 }
 
 void handleSetMinuteMode() {
   // Get the current position
   long newPosition = encoder.getPosition();
   
   // If position has changed, update the minutes
   if (newPosition != lastPos) {
     // Calculate direction of rotation
     int direction = (newPosition > lastPos) ? 1 : -1;
     
     // Update minutes with wrap-around (0-59)
     settingMinutes = (settingMinutes + direction) % 60;
     // Handle negative wrap-around
     if (settingMinutes < 0) settingMinutes += 60;
     
     Serial.print("Setting Minutes: ");
     Serial.println(settingMinutes);
     
     lastPos = newPosition;
     
     // Update last activity time to indicate encoder is being used
     lastEncoderActivity = millis();
     
     // Show the time immediately when encoder is rotated
     displaySettingTime(activeDisplay, true);
   }
   
   // Only blink when the encoder hasn't been used for a while
   if (millis() - lastEncoderActivity >= ACTIVITY_TIMEOUT) {
     if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
       blinkState = !blinkState;
       lastBlinkTime = millis();
       
       // Display time with possibly blinking minutes
       displaySettingTime(activeDisplay, blinkState);
     }
   }
 }
 
 void handleButtonPresses() {
   // Check if select button is pressed
   if (digitalRead(SELECT_BUTTON_PIN) == LOW) {
     // Debounce the button
     delay(50);
     if (digitalRead(SELECT_BUTTON_PIN) == LOW) {
       // Wait for button release
       while (digitalRead(SELECT_BUTTON_PIN) == LOW);
       
       switch (currentMode) {
         case MODE_NORMAL:
           // Start time setting mode
           Serial.println("Select button pressed for group: " + String(selectedGroup));
           activeDisplay = selectedGroup;
           
           // Get current time from RTC for initial values or use previously set alarm time
           if (alarmHours[activeDisplay] >= 0) {
             // Use previously set alarm time
             settingHours = alarmHours[activeDisplay];
             settingMinutes = alarmMinutes[activeDisplay];
           } else {
             // Use current time
             DateTime now = rtc.now();
             settingHours = now.hour();
             settingMinutes = now.minute();
           }
           
           // Enter hour setting mode
           currentMode = MODE_SET_HOUR;
           Serial.println("Entered hour setting mode");
           
           // Reset encoder position for setting
           encoder.setPosition(0);
           lastPos = 0;
           lastEncoderActivity = 0; // Reset activity timer
           
           // Show the time immediately
           displaySettingTime(activeDisplay, true);
           break;
           
         case MODE_SET_HOUR:
         case MODE_SET_MINUTE:
           // Save the time and exit setting mode
           Serial.print("Time set to: ");
           Serial.print(settingHours);
           Serial.print(":");
           if (settingMinutes < 10) Serial.print("0");
           Serial.println(settingMinutes);
           
           // Save the alarm time for this group
           alarmHours[activeDisplay] = settingHours;
           alarmMinutes[activeDisplay] = settingMinutes;
           
           // Display the final time
           displayCurrentTime(activeDisplay, settingHours, settingMinutes);
           
           // Keep time display on for this group
           timeDisplayOn[activeDisplay] = true;
           
           // Initiate confirmation flash
           startConfirmationFlash(activeDisplay);
           
           // Return to normal mode
           currentMode = MODE_NORMAL;
           Serial.println("Exited time setting mode, alarm time saved");
           
           // Reset encoder position
           encoder.setPosition(0);
           lastPos = 0;
           break;
       }
       
       delay(250); // Debounce
     }
   }
   
   // Check if toggle button is pressed
   if (digitalRead(TOGGLE_BUTTON_PIN) == LOW) {
     // Debounce the button
     delay(50);
     if (digitalRead(TOGGLE_BUTTON_PIN) == LOW) {
       // Wait for button release
       while (digitalRead(TOGGLE_BUTTON_PIN) == LOW);
       
       switch (currentMode) {
         case MODE_NORMAL:
           // If an alarm is active for the selected group, turn it off
           if (alarmActive[selectedGroup]) {
             alarmActive[selectedGroup] = false;
             Serial.print("Alarm for group ");
             Serial.print(selectedGroup);
             Serial.println(" turned off");
             
             // Reset the LED colors for this group
             for (int i = selectedGroup * LEDS_PER_GROUP; i < (selectedGroup + 1) * LEDS_PER_GROUP; i++) {
               leds[i] = DEFAULT_COLOR;
             }
             
             // Highlight the selected group
             highlightGroup(selectedGroup);
           } 
           // If time display is on for the selected group, turn it off
           else if (timeDisplayOn[selectedGroup]) {
             timeDisplayOn[selectedGroup] = false;
             displays[selectedGroup].clear();
             displays[selectedGroup].writeDisplay();
             Serial.print("Time display for group ");
             Serial.print(selectedGroup);
             Serial.println(" turned off");
           }
           // Otherwise, toggle the alarm setting for this group
           else if (alarmHours[selectedGroup] >= 0) {
             // Clear the alarm for this group
             alarmHours[selectedGroup] = -1;
             displays[selectedGroup].clear();
             displays[selectedGroup].writeDisplay();
             Serial.print("Alarm for group ");
             Serial.print(selectedGroup);
             Serial.println(" cleared");
           }
           break;
           
         case MODE_SET_HOUR:
           // Switch to minute setting
           currentMode = MODE_SET_MINUTE;
           Serial.println("Switched to minute setting mode");
           
           // Reset encoder position for setting
           encoder.setPosition(0);
           lastPos = 0;
           lastEncoderActivity = 0; // Reset activity timer
           
           // Show the time immediately
           displaySettingTime(activeDisplay, true);
           break;
           
         case MODE_SET_MINUTE:
           // Switch back to hour setting
           currentMode = MODE_SET_HOUR;
           Serial.println("Switched to hour setting mode");
           
           // Reset encoder position for setting
           encoder.setPosition(0);
           lastPos = 0;
           lastEncoderActivity = 0; // Reset activity timer
           
           // Show the time immediately
           displaySettingTime(activeDisplay, true);
           break;
       }
       
       delay(250); // Debounce
     }
   }
 }
 
 void checkAlarms() {
   // Get current time
   DateTime now = rtc.now();
   int currentHour = now.hour();
   int currentMinute = now.minute();
   
   // Check each group for alarm trigger
   for (int i = 0; i < NUM_GROUP; i++) {
     // Skip if no alarm set for this group
     if (alarmHours[i] < 0) continue;
     
     // Check if current time matches alarm time
     if (currentHour == alarmHours[i] && currentMinute == alarmMinutes[i] && !alarmActive[i]) {
       // Activate alarm
       Serial.print("ALARM TRIGGERED for group ");
       Serial.println(i);
       alarmActive[i] = true;
     }
   }
 }
 
 /**
  * Start the triple-flash confirmation animation
  */
 void startConfirmationFlash(int groupIndex) {
   confirmationFlashActive = true;
   currentFlashCount = 0;
   lastFlashTime = 0; // Force immediate first flash
   flashingGroupIndex = groupIndex;
   
   Serial.print("Starting confirmation flash for group ");
   Serial.println(groupIndex);
 }
 
 /**
  * Handle the triple-flash confirmation animation
  */
 void handleConfirmationFlash() {
   if (millis() - lastFlashTime >= FLASH_INTERVAL) {
     lastFlashTime = millis();
     
     // Calculate if we're on an "on" or "off" flash
     bool isOn = (currentFlashCount % 2 == 0);
     
     // Set the flashing color
     int startIdx = flashingGroupIndex * LEDS_PER_GROUP;
     int endIdx = startIdx + LEDS_PER_GROUP;
     
     // Update the LEDs based on current flash state
     for (int i = startIdx; i < endIdx; i++) {
       leds[i] = isOn ? CONFIRM_COLOR : DEFAULT_COLOR;
     }
     
     // Show the updated LEDs
     FastLED.show();
     
     // Increment flash count
     currentFlashCount++;
     
     // Check if we've completed the triple flash (6 transitions total: on-off-on-off-on-off)
     if (currentFlashCount >= CONFIRMATION_FLASH_COUNT * 2) {
       // Flash sequence complete
       confirmationFlashActive = false;
       
       // Restore default color and highlight
       for (int i = startIdx; i < endIdx; i++) {
         leds[i] = DEFAULT_COLOR;
       }
       
       // If this is the selected group, highlight it
       if (flashingGroupIndex == selectedGroup) {
         highlightGroup(selectedGroup);
       } else {
         FastLED.show();
       }
       
       Serial.println("Confirmation flash complete");
     }
   }
 }
 
 void handleActiveAlarms() {
   // Handle blinking for any active alarms
   if (millis() - lastAlarmBlinkTime >= ALARM_BLINK_INTERVAL) {
     alarmBlinkState = !alarmBlinkState;
     lastAlarmBlinkTime = millis();
     
     // Update LEDs for any active alarms
     for (int i = 0; i < NUM_GROUP; i++) {
       if (alarmActive[i]) {
         int startIdx = i * LEDS_PER_GROUP;
         int endIdx = startIdx + LEDS_PER_GROUP;
         
         // Set color based on blink state
         CRGB color = alarmBlinkState ? ALARM_COLOR : OFF_COLOR;
         
         // Update LEDs for this group
         for (int j = startIdx; j < endIdx; j++) {
           leds[j] = color;
         }
       }
     }
     
     // Show updated LEDs
     FastLED.show();
   }
 }
 
 void updateTimeDisplays() {
   // Update any active time displays once per second
   static unsigned long lastTimeUpdate = 0;
   if (millis() - lastTimeUpdate >= 1000) {
     DateTime now = rtc.now();
     
     // Update each active display
     for (int i = 0; i < MAX_DISPLAYS; i++) {
       if (timeDisplayOn[i] && !alarmActive[i] && currentMode == MODE_NORMAL) {
         if (alarmHours[i] >= 0) {
           // Show the alarm time
           displayCurrentTime(i, alarmHours[i], alarmMinutes[i]);
         } else {
           // Show current time
           displayCurrentTime(i, now.hour(), now.minute());
         }
       }
     }
     
     lastTimeUpdate = millis();
   }
 }
 
 /**
  * Highlights a specific LED group with the highlight color
  */
 void highlightGroup(int groupIndex) {
   // Only highlight if not in alarm state
   if (alarmActive[groupIndex]) return;
   
   // Calculate start and end indices for the group
   int startIndex = groupIndex * LEDS_PER_GROUP;
   int endIndex = startIndex + LEDS_PER_GROUP;
   
   // Set the group LEDs to highlight color
   for (int i = startIndex; i < endIndex; i++) {
     leds[i] = HIGHLIGHT_COLOR;
   }
   
   // Update LEDs
   FastLED.show();
   
   Serial.print("Highlighted group ");
   Serial.println(groupIndex);
 }
 
 /**
  * Displays the current time on a specific 7-segment display
  */
 void displayCurrentTime(int displayIndex, int hours, int minutes) {
   // Make sure the display index is valid
   if (displayIndex < 0 || displayIndex >= MAX_DISPLAYS) {
     return;
   }
   
   // Clear the display
   displays[displayIndex].clear();
   
   // Convert 24-hour format to 12-hour if needed
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
   displays[displayIndex].writeDigitNum(0, displayHours / 10, false);  // First digit of hour
   displays[displayIndex].writeDigitNum(1, displayHours % 10, false);
   
   // Position for minutes: 3 and 4
   displays[displayIndex].writeDigitNum(3, minutes / 10, false);
   displays[displayIndex].writeDigitNum(4, minutes % 10, false);
   
   // Turn on colon between hours and minutes
   displays[displayIndex].drawColon(true);
   
   // Update the display
   displays[displayIndex].writeDisplay();
   
   // Print time to serial monitor only if in setting mode or first display
   if (currentMode != MODE_NORMAL || displayIndex == activeDisplay) {
     Serial.print("Time for display #");
     Serial.print(displayIndex);
     Serial.print(": ");
     Serial.print(displayHours);
     Serial.print(":");
     if (minutes < 10) Serial.print("0");
     Serial.print(minutes);
     Serial.println(isPM ? " PM" : " AM");
   }
 }
 
 /**
  * Displays the time being set with blinking digits
  */
 void displaySettingTime(int displayIndex, bool showBlink) {
   // Make sure the display index is valid
   if (displayIndex < 0 || displayIndex >= MAX_DISPLAYS) {
     return;
   }
   
   // Clear the display
   displays[displayIndex].clear();
   
   // Convert 24-hour format to 12-hour if needed
   int displayHours = settingHours;
   bool isPM = false;
   
   if (settingHours > 12) {
     displayHours = settingHours - 12;
     isPM = true;
   } else if (settingHours == 0) {
     displayHours = 12;
   } else if (settingHours == 12) {
     isPM = true;
   }
   
   // If actively adjusting or showBlink is true, show the digits
   bool showHours = (millis() - lastEncoderActivity < ACTIVITY_TIMEOUT) || 
                    (currentMode != MODE_SET_HOUR) || 
                    showBlink;
                    
   bool showMinutes = (millis() - lastEncoderActivity < ACTIVITY_TIMEOUT) || 
                      (currentMode != MODE_SET_MINUTE) || 
                      showBlink;
   
   // Position for hours: 0 and 1 (blinking if in hour setting mode)
   if (showHours) {
     displays[displayIndex].writeDigitNum(0, displayHours / 10, false);
     displays[displayIndex].writeDigitNum(1, displayHours % 10, false);
   }
   
   // Position for minutes: 3 and 4 (blinking if in minute setting mode)
   if (showMinutes) {
     displays[displayIndex].writeDigitNum(3, settingMinutes / 10, false);
     displays[displayIndex].writeDigitNum(4, settingMinutes % 10, false);
   }
   
   // Turn on colon between hours and minutes
   displays[displayIndex].drawColon(true);
   
   // Update the display
   displays[displayIndex].writeDisplay();
 }