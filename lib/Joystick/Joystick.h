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
        JoystickState _prevState;
        long _timeSinceLastChange;
        JoystickState getDirection();

    public:
        Joystick(int xPin, int yPin) {
            _timeSinceLastChange = 0;
            _xPin = xPin;
            _yPin = yPin;
            _prevState = JoystickState::Neutral;
            pinMode(xPin, INPUT);
            pinMode(yPin, INPUT);
        };
        
        JoystickState getState();
};

#endif