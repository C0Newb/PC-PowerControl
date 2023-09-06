// LED.h
#ifndef _LED_H_
#define _LED_H_

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

#include <Settings.h>

class LED {
	private:
		// Pin name/type
		Pin pin;
		// Actual GPIO pin
		int gpioPin;
		// Read the LED status via an anlog reading
		bool isAnalog;
		// Read the status of the LED, do not change it.
		bool readOnly;

		// State the LED has to be in to be "on"
		int onState = LOW;
		// State the LED has to be in to be "off"
		int offState = HIGH;
	public:
		// Task used to schedule the LED to turn off
		Task* turnOffTask;

		LED();


		LED(Pin pin);


		/**
		 * @brief Turn of the LED.
		 */
		void turnOn();

		/**
		 * @brief Turn off the LED.
		 */
		void turnOff();

		/**
		 * @brief Turn on the LED and then turn it off after a set `duration`.
		 * @param duration Amount of time the LED is to remain on.
		 */
		void flash(int duration);

		/**
		 * @brief Check whether the LED is currently on or not.
		 * @return True if the LED is active, false if it is not. 
		 */
		bool isOn();

		/**
		 * @brief Returns the GPIO pin of this LED.
		 * @return Pyhiscal GPIO pin of the LED.
		 */
		int getPin();

		/**
		 * @brief Allows you to set the GPIO pin of this LED.
		 * @param gpioPin Physical GPIO pin of the LED.
		 */
		void setPin(int gpioPin);
		/**
		 * @brief Whether HIGH or LOW is the on state, what signifies the LED to be shinning.
		 * @return State the LED must be in to be turned on.
		 */
		int getOnState();
		/**
		 * @brief Sets whether the LED is turned on with signal `HIGH` or `LOW`.
		 * @param state State to be considered on.
		 */
		void setOnState(int state);
};

#endif