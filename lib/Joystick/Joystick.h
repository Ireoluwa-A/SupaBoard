#include <Arduino.h>


#define JOYSTICK_STABLE_VALUE 516
#define DEADZONE_RANGE 100

#ifndef JOYSTICK_H
#define JOYSTICK_H

enum class JoystickState {
    Neutral,
    Up,
    Down,
    Left,
    Right
};
  

class Joystick {
    private:
        int _xPin;
        int _yPin;
        int _buttonPin;
        JoystickState _prevState;
        long _timeSinceLastChange;
        JoystickState getDirection();

    public:
        Joystick(int xPin, int yPin, int buttonPin) {
            _timeSinceLastChange = 0;
            _xPin = xPin;
            _yPin = yPin;
            _buttonPin = buttonPin;
            _prevState = JoystickState::Neutral;
            pinMode(xPin, INPUT);
            pinMode(yPin, INPUT);
            pinMode(buttonPin, INPUT_PULLUP);
        };
        
        JoystickState getState();
        bool isButtonClicked();

};

#endif