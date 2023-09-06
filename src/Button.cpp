#include <Button.h>

Button::Button() {}

Button::Button(Pin pin) {
	this->pin = pin;
	this->gpioPin = Settings::getPin(pin);

	if (pin == MOTHERBOARD_POWER_BUTTON || pin == MOTHERBOARD_RESET_BUTTON) {
		this->isReadonly = false;

		pinMode(this->gpioPin, OUTPUT);
        digitalWrite(this->gpioPin, this->releasedState);
    } else {
		this->isReadonly = true;
		pinMode(this->gpioPin, INPUT_PULLUP);
	}
}


void Button::hold() {
    if (!this->isReadonly)
        digitalWrite(this->gpioPin, this->pressedState);
}

void Button::release() {
    if (!this->isReadonly)
        digitalWrite(this->gpioPin, this->releasedState);
}


void Button::pressAndRelease() {
    pressAndRelease(false);
}
void Button::pressAndRelease(bool longPress) {
    if (this->isReadonly)
        return;

    if (longPress)
		pressAndRelease(Settings::getLongButtonPressDuration());
	else
        pressAndRelease(Settings::getShortButtonPressDuration());
}
void Button::pressAndRelease(int holdDuration) {
	if (this->isReadonly)
		return;

	hold();
	this->releaseTask->restartDelayed(holdDuration);
}

bool Button::isPressed() {
	return digitalRead(this->gpioPin) == this->pressedState;
}


// Getters/setters		
int Button::getPin() {
	return this->gpioPin;
}

void Button::setPin(int gpioPin) {
	Settings::setPin(this->pin, gpioPin);
	this->gpioPin = gpioPin;
}


int Button::getPressedState() {
	return this->pressedState;
}

void Button::setPressedState(int state) {
	// Settings::setActiveState(this->pin, state)
	this->pressedState = state;
	this->releasedState = !state;
}