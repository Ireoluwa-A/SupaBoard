#include <Arduino.h>


#define JOYSTICK_STABLE_VALUE 516
#define DEADZONE_RANGE 100

#ifndef JOYSTICK_H
#define JOYSTICK_H

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    STABLE,
};

struct JoystickReading {
    Direction direction;
    bool buttonClicked;
};

class Joystick {
    private:
        int _xPin;
        int _yPin;
        int _buttonPin;

    public:
        Joystick(int xPin, int yPin, int buttonPin) {
            _xPin = xPin;
            _yPin = yPin;
            _buttonPin = buttonPin;
            pinMode(xPin, INPUT);
            pinMode(yPin, INPUT);
            pinMode(buttonPin, INPUT_PULLUP);
        };
        
        Direction getDirection();
        bool isButtonClicked();
        
};

#endif