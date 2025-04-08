#include "Button.h"

#define DEBOUNCE_DELAY 50          // Time to debounce button presses
#define HOLD_TIME 1000             // Time in ms to detect a hold event
#define DOUBLE_CLICK_TIME 600      // Time window for detecting a double-click

Button::Button(uint8_t pin) : _pin(pin), _lastState(HIGH), _lastEvent(ButtonEvent::None) {
    pinMode(_pin, INPUT_PULLUP);
}

ButtonEvent Button::getEvent() {
    bool currentState = digitalRead(_pin);
    unsigned long now = millis();

    // Detect button press
    if (currentState == LOW && _lastState == HIGH) {
        _lastPressTime = now;

        // Check for double-click
        if (now - _lastReleaseTime < DOUBLE_CLICK_TIME) {
            _lastEvent = ButtonEvent::DoubleClick;
        } else {
            _lastEvent = ButtonEvent::Click;
        }
    }

    // Detect button release
    if (currentState == HIGH && _lastState == LOW) {
        _lastReleaseTime = now;
        if (now - _lastPressTime >= HOLD_TIME) {
            _lastEvent = ButtonEvent::Hold;
        } else {
            _lastEvent = ButtonEvent::Release;
        }
    }

    _lastState = currentState;
    ButtonEvent event = _lastEvent;
    _lastEvent = ButtonEvent::None;
    return event;
}
