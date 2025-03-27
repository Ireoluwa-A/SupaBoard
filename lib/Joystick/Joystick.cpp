#include <Arduino.h>
#include "Joystick.h"

bool inDeadzone(int value) {
    return value > JOYSTICK_STABLE_VALUE - DEADZONE_RANGE && value < JOYSTICK_STABLE_VALUE + DEADZONE_RANGE;
}

Direction Joystick::getDirection() {
    int x = analogRead(_xPin);
    int y = analogRead(_yPin);

    if (inDeadzone(x) && inDeadzone(y)) {
        return STABLE;
    }
    int xDistFromCenter = abs(x - JOYSTICK_STABLE_VALUE);
    int yDistFromCenter = abs(y - JOYSTICK_STABLE_VALUE);

    if (xDistFromCenter > yDistFromCenter) {
        if (x > JOYSTICK_STABLE_VALUE) {
            return RIGHT;
        } else {
            return LEFT;
        }
    } else {
        if (y > JOYSTICK_STABLE_VALUE){
            return UP;
        } else {
            return DOWN;
        }
    }
}

bool Joystick::isButtonClicked() {
    return digitalRead(_buttonPin) == LOW;
}
