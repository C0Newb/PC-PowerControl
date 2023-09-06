// Button.h
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

#include <Settings.h>

class Button {
	private:
		// Which button we represent pin
		Pin pin;
		// The GPIO pin
		int gpioPin;
		// Read the status, do not write!
		bool isReadonly;

		int releasedState = HIGH;
		// for PCs, when the + pin is shorted to ground it counts as a "press".
		int pressedState = LOW;

	public:
		// Task used to schedule a button release
		Task *releaseTask;


		Button();
		/**
		 * @brief Construct a new Button object. Creates the task and sets the GPIO pin mode.
		 * 
		 * Does not set the release callback!
		 * You are required to a few things before using this class (set .releaseTask.setCallback() to call this->release())
		 * 
		 * @param pin Pin this button represents. 
		 */
		Button(Pin pin);


		/**
		 * @brief Presses and holds the button down.
		 */
		void hold();
		
		/**
		 * @brief Releases the button.
		 */
		void release();

		/**
		 * @brief Similates a short (250ms) button press.
		 */
		void pressAndRelease();
		
		/**
		 * @brief Similates a short (250ms) button press or a long one (5000ms).
		 * @param longPress Whether the press is a short (250ms) one or a long (5000ms) one.
		 */
		void pressAndRelease(bool longPress);
		
		/**
		 * @brief Presses the button and holds it for `holdDuration` miliseconds.
		 * @param holdDuration How long to wait before releasing the button.
		 */
		void pressAndRelease(int holdDuration);
		
		/**
		 * @brief Whether the button is currently being pressed or not.
		 * @return True if the button is being pressed (or similating a press), false if the button is neither pressed nor is one being similated.
		 */
		bool isPressed();


		/**
		 * @brief Returns the GPIO pin of this button.
		 * @return Pyhiscal GPIO pin of the button.
		 */
		int getPin();

		/**
		 * @brief Allows you to set the GPIO pin of this button.
		 * @param gpioPin Physical GPIO pin of the button.
		 */
		void setPin(int gpioPin);
		
		/**
		 * @brief Whether HIGH or LOW is the "pressed" state, what signifies a press.
		 * @return State the button must be in to be "pressed".
		 */
		int getPressedState();
		
		/**
		 * @brief Sets whether the button is pressed on `HIGH` or `LOW`.
		 * @param state State to be considered pressed.
		 */
		void setPressedState(int state);
};

#endif