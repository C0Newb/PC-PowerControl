# Control your computer's power (buttons) remotely
ESP32/ESP8266 program to host a website that allows you to press/hold the power/reset button and view the power/hdd LED status.


The ESP32/8266 stands in the middle of the power/reset buttons and power/hdd leds on the case. This allows a sort of "lock-out" feature to prevent the case power button from doing anything 

Project is managed/built/whatever using Platform.io in VsCode (sorry, you need Chrome to run this :/).

---

This code *should* work with a ESP32, however, I have not added the build options nor tested with the ESP32 (it's a bit overkill).

Beta stages, I'll finish it up at some point (maybe).\
There's no configurable settings that you can change from the web page, currently they're "hard coded."\
(Well, sorta, I make use of the preferences library to allow a few things to be customizable, just none of that customization is available from the web interface yet!)

**Default web username is `admin` and the password is `pcpowercontrol`** .. available at `../login` (you should be automatically redirected).


Because I didn't want to continue typing, here's a basic table showing the progress:

| Name | Stage |
| --   | --    |
| Remote power/reset button control | Supported |
| View power LED status | Supported\* (\*some motherboards produce a weird output, so at the moment only on LED is supported at a time via the analog pin) |
| View HDD LED status | Supported\* (\* see above note, may not work on your machine) |
| Password protected webpage | Implemented, but not amazing (only one browser can login at once, the security should be okay but the password cannot be changed) |
| -- | -- |
| OTA updated | Not available |
| Lockout physical power/reset button | Not available (it's there, but cannot be configured via the webpage) |
| Easy WiFi setup | You **must** enter the WiFi SSID/password and recompile! |
| Configure GPIO pins | Cannot change via web interface |