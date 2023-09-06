// Settings.h
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <Arduino.h>
#include <Preferences.h>
#include <SHA256.h>

enum Pin {
    UNKNOWN,
    MOTHERBOARD_POWER_LED,
    MOTHERBOARD_HDD_LED,
    MOTHERBOARD_POWER_BUTTON,
    MOTHERBOARD_RESET_BUTTON,

    CASE_POWER_LED,
    CASE_HDD_LED,
    CASE_POWER_BUTTON,
    CASE_RESET_BUTTON
};

class Settings {
    public:
        // Static preferences instance
        static Preferences preferences;

        // pins
        /**
         * @brief Get the pin number (GPIO) for a given pin type (motherboard power button, case hdd led, etc).
         * @param pin Pin to get the GPIO pin mapping for.
         * @return GPIO pin number for the given pin
         */
        static int getPin(Pin name);
        /**
         * @brief Set the GPIO pin number for a given pin type (motherboard power button, case hdd led, etc).
         * @param pin Pin to set the GPIO pin mapping for.
         * @param gpio GPIO pin number, such as `D0`.
         */
        static void setPin(Pin name, int pin);


        // buttons
        /**
         * @brief Get the current duration of a long button press.
         * @return Delay between the down and release of a long button press.
         */
        static int getLongButtonPressDuration();
        /**
         * @brief Set the duration, in ms, of a long button press
         * @param duration Time between the down and release of a long button press.
         */
        static void setLongButtonPressDuration(int duration);
        /**
         * @brief Get the current duration of a short button press.
         * @return Delay between the down and release of a short button press.
         */
        static int getShortButtonPressDuration();
        /**
         * @brief Set the duration, in ms, of a short button press
         * @param duration Time between the down and release of a short button press.
         */
        static void setShortButtonPressDuration(int duration);


        // Wireless and web
        // Returns the configured WiFi SSID to connect to.
        static String getWirelessSSID();
        // Set the WiFi SSID to connect to. 
        static void setWirelessSSID(String ssid);

        // Returns the configured WiFi password (when connecting to)
        static String getWirelessPassword();
        // Sets the WiFi password (when connecting to)
        static void setWirelessPassword(String password);


        // Gets the password for the WiFi hotspot used to setup/reconnect the device.
        static String getWirelessAPPassword();
        // Sets the password for the WiFi hotspot used to setup/reconnect the device.
        static void setWirelessAPPassword(String password);


        // Gets the port used for the webserver.
        static int getWebServerPort();
        // Sets the port used for the webserver.
        static void setWebServerPort(int port);


        /**
         * @brief Gets the username for the web portal.
         * @return Web portal username.
         */
        static String getWebServerUsername();
        /**
         * @brief Sets the username for the web portal.
         * @param username New web portal username
         */
        static void setWebServerUsername(String username);

        /**
         * @brief Returns the hashed web authentication password. Why hashed?
         * Great question... if you could read this hash you could also reset it :D ??
         * @return Hashed web portal password.
         */
        static String getWebServerPassword();
        /**
         * @brief Sets the password required to use the web portal.
         * @param password New web portal password, will be hashed!
         */
        static void setWebServerPassword(String password);
        
};

#endif