#include <Arduino.h>
#include "Joystick.h"

bool inDeadzone(int value) {
    return value > JOYSTICK_STABLE_VALUE - DEADZONE_RANGE && value < JOYSTICK_STABLE_VALUE + DEADZONE_RANGE;
}

JoystickState Joystick::getState() {
    JoystickState currentState = getDirection();
    if (currentState != _prevState) {
        _timeSinceLastChange = millis();
        _prevState = currentState;
        return _prevState;
    } else if (millis() - _timeSinceLastChange > 500) {
        _timeSinceLastChange = millis();
        return _prevState;
    } else {
        return JoystickState::Neutral;
    }
}

JoystickState Joystick::getDirection() {
    int x = analogRead(_xPin);
    int y = analogRead(_yPin);

    if (inDeadzone(x) && inDeadzone(y)) {
        return JoystickState::Neutral;
    }
    int xDistFromCenter = abs(x - JOYSTICK_STABLE_VALUE);
    int yDistFromCenter = abs(y - JOYSTICK_STABLE_VALUE);

    if (xDistFromCenter > yDistFromCenter) {
        if (x > JOYSTICK_STABLE_VALUE) {
            return JoystickState::Right;
        } else {
            return JoystickState::Left;
        }
    } else {
        if (y > JOYSTICK_STABLE_VALUE){
            return JoystickState::Up;
        } else {
            return JoystickState::Down;
        }
    }
}

bool Joystick::isButtonClicked() {
    return digitalRead(_buttonPin) == LOW;
}
