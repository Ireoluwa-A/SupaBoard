// Define joystick pins

#define JOYSTICK_X_PIN A0;
#define JOYSTICK_Y_PIN A1;

#define BUTTON_BIN 2;

#define JOYSTICK_STABLE_VALUE 516;
#define DEADZONE_RANGE 100;

const int joystickXPin = A0;
const int joystickYPin = A1;
const int buttonPin = 2;

enum Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STABLE,
};

Direction prevState = STABLE;
Direction currentState = STABLE;
const int JOYSTICK_STABLE_VALUE = 516;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set button pin as input
  pinMode(buttonPin, INPUT_PULLUP);
}

bool isContained(int val) {
  return (val < JOYSTICK_STABLE_VALUE + delta) &&
    (val > JOYSTICK_STABLE_VALUE - delta);
}

Direction getDirection(int x, int y) {
  if (isContained(x, 100) && isContained(y, 100)) {
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

void loop() {
  // Read joystick values
  int joystickYValue = analogRead(joystickXPin);
  int joystickXValue = analogRead(joystickYPin);

  Direction direction = getDirection(joystickXValue, joystickYValue);
  Serial.println(direction);
  Serial.print("\n\n");
  // Read button state
  int buttonState = digitalRead(buttonPin);

  // Print values to serial monitor for debugging
  Serial.print("X: ");
  Serial.print(joystickXValue);
  Serial.print(" Y: ");
  Serial.print(joystickYValue);
  Serial.print(" Button: ");
  Serial.println(buttonState);
  delay(500);
  // Use the joystick values to control other components or actions
  // Example: Map joystick values to LED brightness
  // map(joystickXValue, 0, 1023, 0, 255); // Map to 0-255 range for LED brightness
}

