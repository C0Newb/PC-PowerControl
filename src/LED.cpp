#include <LED.h>

LED::LED() {}

LED::LED(Pin pin) {
	this->pin = pin;
	this->gpioPin = Settings::getPin(pin);

	if (this->gpioPin == A0)
		this->isAnalog = true;

	if (pin == MOTHERBOARD_HDD_LED || pin == MOTHERBOARD_POWER_LED) {
		this->readOnly = true;
		if (!this->isAnalog) {
			pinMode(this->gpioPin, INPUT_PULLUP);
		}
	} else {
		// output
		pinMode(this->gpioPin, OUTPUT);
	}
}


void LED::turnOn() {
	if (!this->readOnly)
		digitalWrite(this->gpioPin, onState);
}


void LED::turnOff() {
	if (!this->readOnly)
		digitalWrite(this->gpioPin, offState);
}


void LED::flash(int duration) {
	turnOn();
	this->turnOffTask->restartDelayed(duration);
}


bool LED::isOn() {
	if (this->isAnalog) {
		if (analogRead(this->gpioPin) >= 600)
			return true;
		else
			return false;
	} else {
		return digitalRead(this->gpioPin) == this->onState;
	}
}


int LED::getPin() {
	return this->gpioPin;
}

void LED::setPin(int gpioPin) {
	Settings::setPin(this->pin, gpioPin);
	this->gpioPin = gpioPin;
}


int LED::getOnState() {
	return this->onState;
}

void LED::setOnState(int state) {
	// Settings::setActiveState(this->pin, state)
	this->onState = state;
	this->offState = !state;
}