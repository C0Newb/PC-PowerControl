#include <Main.h>
Scheduler scheduler;

///////////////////////////////////////
//             Defaults              //
///////////////////////////////////////
const int DEFAULT_MOTHERBOARD_POWER_LED_PIN = A0;    // (in) <- mb power LED
const int DEFAULT_MOTHERBOARD_HDD_LED_PIN = D1;      // <- mb hdd LED
const int DEFAULT_MOTHERBOARD_RESET_BUTTON_PIN = D3; // -> (out) mb's reset button
const int DEFAULT_MOTHERBOARD_POWER_BUTTON_PIN = D2; // -> mb's power button

const int DEFAULT_CASE_POWER_LED_PIN = ONBOARD_LED; // -> case power LED
const int DEFAULT_CASE_HDD_LED_PIN = D5;            // -> case hdd LED
const int DEFAULT_CASE_POWER_BUTTON_PIN = D6;       // <- case power button
const int DEFAULT_CASE_RESET_BUTTON_PIN = D7;       // <- case reset button

const char *DEFAULT_WIFI_SSID = "MY WiFi";
const char *DEFAULT_WIFI_PASSWORD = "Almost committed this one!"; // someone should really remove this, huh?

///////////////////////////////////////
//             Methods               //
///////////////////////////////////////
void wifiSetup();
void serialReadout();
void statusPoller();
Task statusPollerTask(50, TASK_FOREVER, &statusPoller, &scheduler, true);
Task serialReadoutTask(500, TASK_FOREVER, &serialReadout, &scheduler, false);
Task wifiSetupTask(1000, 1, &wifiSetup, &scheduler, true);

// blah, blah, you cannot pass a class's instance into as a task callback
void motherboardPowerButtonReleaseCallback();
void motherboardResetButtonReleaseCallback();
void casePowerButtonReleaseCallback();
void caseResetButtonReleaseCallback();

void motherboardPowerLEDTurnOffCallback();
void motherboardHDDLEDTurnOffCallback();
void casePowerLEDTurnOffCallback();
void caseHDDLEDTurnOffCallback();

// Create AsyncWebServer
AsyncWebServer server(80);
Web web(&server);

Button motherboardPowerButton;
Button motherboardResetButton;
Button casePowerButton;
Button caseResetButton;

LED motherboardPowerLED;
LED motherboardHDDLED;
LED casePowerLED;
LED caseHDDLED;

Task motherboardPowerButtonReleaseTask(0, TASK_ONCE, &motherboardPowerButtonReleaseCallback, &scheduler, false);
Task motherboardResetButtonReleaseTask(0, TASK_ONCE, &motherboardResetButtonReleaseCallback, &scheduler, false);
Task casePowerButtonReleaseTask(0, TASK_ONCE, &casePowerButtonReleaseCallback, &scheduler, false);
Task caseResetButtonReleaseTask(0, TASK_ONCE, &caseResetButtonReleaseCallback, &scheduler, false);
Task motherboardPowerLEDTurnOffTask(0, TASK_ONCE, &motherboardPowerLEDTurnOffCallback, &scheduler, false);
Task motherboardHDDLEDTurnOffTask(0, TASK_ONCE, &motherboardHDDLEDTurnOffCallback, &scheduler, false);
Task casePowerLEDTurnOffTask(0, TASK_ONCE, &casePowerLEDTurnOffCallback, &scheduler, false);
Task caseHDDLEDTurnOffTask(0, TASK_ONCE, &caseHDDLEDTurnOffCallback, &scheduler, false);

bool allowCaseButtons = true;

// Interaction
IRAM_ATTR void casePowerButtonIterrupt() {
    if (allowCaseButtons) {
        if (casePowerButton.isPressed())
            motherboardPowerButton.hold();
        else
            motherboardPowerButton.release();
    }
}

IRAM_ATTR void caseResetButtonIterrupt() {
    if (allowCaseButtons) {
        if (caseResetButton.isPressed())
            motherboardResetButton.hold();
        else
            motherboardResetButton.release();
    }
}

void setup() {
// Serial port for debugging purposes
#if ESP32
    Serial.begin(115200);
#else
    Serial.begin(9600);
    Serial.println("--! Raising serial baudrate to 115200 !--");
    Serial.end();
    Serial.begin(115200);
#endif

    Serial.println("");
    Serial.println(".\\>");
    Serial.println(STARTUP_HEADER);

    auto wStatus = [](String message) {
        Serial.println(message);
    };
    wStatus("");
    wStatus("Booting up..!");

    wStatus("Registering preferences");
    Settings::preferences.begin("pc-powercontrol");

    // Register default pins
    wStatus("Validating pin configuration");
    if (Settings::getPin(Pin::MOTHERBOARD_POWER_BUTTON) == -1)
        Settings::setPin(Pin::MOTHERBOARD_POWER_BUTTON, DEFAULT_MOTHERBOARD_POWER_BUTTON_PIN);
    if (Settings::getPin(Pin::MOTHERBOARD_RESET_BUTTON) == -1)
        Settings::setPin(Pin::MOTHERBOARD_RESET_BUTTON, DEFAULT_MOTHERBOARD_RESET_BUTTON_PIN);
    if (Settings::getPin(Pin::CASE_POWER_BUTTON) == -1)
        Settings::setPin(Pin::CASE_POWER_BUTTON, DEFAULT_CASE_POWER_BUTTON_PIN);
    if (Settings::getPin(Pin::CASE_RESET_BUTTON) == -1)
        Settings::setPin(Pin::CASE_RESET_BUTTON, DEFAULT_CASE_RESET_BUTTON_PIN);

    if (Settings::getPin(Pin::MOTHERBOARD_POWER_LED) == -1)
        Settings::setPin(Pin::MOTHERBOARD_POWER_LED, DEFAULT_MOTHERBOARD_POWER_LED_PIN);
    if (Settings::getPin(Pin::MOTHERBOARD_HDD_LED) == -1)
        Settings::setPin(Pin::MOTHERBOARD_HDD_LED, DEFAULT_MOTHERBOARD_HDD_LED_PIN);
    if (Settings::getPin(Pin::CASE_POWER_LED) == -1)
        Settings::setPin(Pin::CASE_POWER_LED, DEFAULT_CASE_POWER_LED_PIN);
    if (Settings::getPin(Pin::CASE_HDD_LED) == -1)
        Settings::setPin(Pin::CASE_HDD_LED, DEFAULT_CASE_HDD_LED_PIN);

    if (Settings::getWirelessSSID() == "") {
        wStatus("Resetting wireless SSID and password!");
        Settings::setWirelessSSID(DEFAULT_WIFI_SSID);
        Settings::setWirelessPassword(DEFAULT_WIFI_PASSWORD);
    }

    // Setup buttons
    wStatus("Setting up buttons");
    wStatus("\tbutton: motherboardPowerButton");
    motherboardPowerButton = Button(Pin::MOTHERBOARD_POWER_BUTTON);
    motherboardPowerButton.releaseTask = &motherboardPowerButtonReleaseTask;

    wStatus("\tbutton: motherboardResetButton");
    motherboardResetButton = Button(Pin::MOTHERBOARD_RESET_BUTTON);
    motherboardResetButton.releaseTask = &motherboardResetButtonReleaseTask;

    wStatus("\tbutton: casePowerButton");
    casePowerButton = Button(Pin::CASE_POWER_BUTTON);
    casePowerButton.releaseTask = &casePowerButtonReleaseTask;

    wStatus("\tbutton: caseResetButton");
    caseResetButton = Button(Pin::CASE_RESET_BUTTON);
    caseResetButton.releaseTask = &caseResetButtonReleaseTask;

    // LEDS
    wStatus("Setting up LEDs");
    wStatus("\tled: motherboardPowerLED");
    motherboardPowerLED = LED(Pin::MOTHERBOARD_POWER_LED);
    motherboardPowerLED.turnOffTask = &motherboardPowerLEDTurnOffTask;

    wStatus("\tled: motherboardHDDLED");
    motherboardHDDLED = LED(Pin::MOTHERBOARD_HDD_LED);
    motherboardHDDLED.turnOffTask = &motherboardHDDLEDTurnOffTask;

    wStatus("\tled: casePowerLED");
    casePowerLED = LED(Pin::CASE_POWER_LED);
    casePowerLED.turnOffTask = &casePowerLEDTurnOffTask;

    wStatus("\tled: caseHDDLED");
    caseHDDLED = LED(Pin::CASE_HDD_LED);
    caseHDDLED.turnOffTask = &caseHDDLEDTurnOffTask;

    wStatus("Setting up physical button interrupts");
    // Reset button
    // if HIGH is the "pressed" state, then the RISING event is it being "pressed".
    attachInterrupt(digitalPinToInterrupt(caseResetButton.getPin()), caseResetButtonIterrupt, CHANGE);

    // Power button
    // if HIGH is the "pressed" state, then the RISING event is it being "pressed".
    attachInterrupt(digitalPinToInterrupt(casePowerButton.getPin()), casePowerButtonIterrupt, CHANGE);

    wStatus("Setting up web server, port: " + String(Settings::getWebServerPort()));

    if (Settings::getWebServerPort() != 80)
        server = AsyncWebServer(Settings::getWebServerPort());


    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });

    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) {
        web.handleLogin(request, false);
    });
    server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", login_html, login_html_len);
        request->send(response);
    });

    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            web.clearSessionToken();
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Set-Cookie", "session-token=; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT");
            response->addHeader("Location", "/login");
            request->send(response);
        } else {
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Location", "/login");
            request->send(response);
        }
    });
    server.on("/logout", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            web.clearSessionToken();
            AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\": \"OK\"}");
            response->addHeader("Set-Cookie", "session-token=; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT");
            request->send(response);
        } else {
            request->send(401, "application/json", "{\"status\": \"Unauthorized\"}");
        }
    });

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html, index_html_len);
            // AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", "index_html");
            request->send(response);
        } else {
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Location", "/login");
            request->send(response);
        }
    });

    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            /*
                {
                    "motherboard": {
                        "power_led": true, // light on?
                        "hdd_led": true,
                        "power_button": false, // being pressed?
                        "reset_button": false,
                    },
                    "case": {
                        "power_led": true, // being light on?
                        "hdd_led": true,
                        "power_button": false, // getting pressed?
                        "reset_button": false,
                    },
                }
            */

            // Create a JSON document
            StaticJsonDocument<1024> json;

            // Populate the JSON document
            JsonObject motherboard = json.createNestedObject("motherboard");
            motherboard["power_led"] = motherboardPowerLED.isOn();
            if (motherboardPowerLED.getPin() == A0)
                motherboard["power_led_raw"] = analogRead(A0);
            motherboard["hdd_led"] = motherboardHDDLED.isOn();
            if (motherboardHDDLED.getPin() == A0)
                motherboard["hdd_led_raw"] = analogRead(A0);

            motherboard["power_button"] = motherboardPowerButton.isPressed();
            motherboard["reset_button"] = motherboardResetButton.isPressed();

            JsonObject caseComponent = json.createNestedObject("case");
            caseComponent["power_led"] = casePowerLED.isOn();
            caseComponent["hdd_led"] = caseHDDLED.isOn();
            caseComponent["power_button"] = casePowerButton.isPressed();
            caseComponent["reset_button"] = caseResetButton.isPressed();

            // Serialize the JSON document to a string
            String jsonString;
            serializeJson(json, jsonString);
            request->send(200, "application/json", jsonString);
        } else {
            request->send(401, "application/json", "{\"status\": \"Unauthorized\"}");
        }
    });

    // Button fire
    server.on("/api/press/power", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            motherboardPowerButton.pressAndRelease();
            request->send(200, "text/plain", "{\"status\": \"OK\"}");
        } else {
            request->send(401, "application/json", "{\"status\": \"Unauthorized\"}");
        }
    });
    server.on("/api/press/power_short", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            motherboardPowerButton.pressAndRelease(false);
            request->send(200, "text/plain", "{\"status\": \"OK\"}");
        } else {
            request->send(401, "application/json", "{\"status\": \"Unauthorized\"}");
        }
    });
    server.on("/api/press/power_long", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            motherboardPowerButton.pressAndRelease(true);
            request->send(200, "text/plain", "{\"status\": \"OK\"}");
        } else {
            request->send(401, "application/json", "{\"status\": \"Unauthorized\"}");
        }
    });
    server.on("/api/press/reset", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (web.isAuthenticated(request)) {
            motherboardResetButton.pressAndRelease();
            request->send(200, "text/plain", "{\"status\": \"OK\"}");
        } else {
            request->send(401, "application/json", "{\"status\": \"Unauthorized\"}");
        }
    });

    // Start server
    server.begin();

    wStatus("Setting LEDs");
    if (motherboardPowerLED.isOn()) {
        wStatus("Turning on case power LED");
        casePowerLED.turnOn();
    } else {
        wStatus("Turning off case power LED");
        casePowerLED.turnOff();
    }

    if (motherboardHDDLED.isOn()) {
        wStatus("Turning on case HDD LED");
        caseHDDLED.turnOn();
    } else {
        wStatus("Turning off case HDD LED");
        caseHDDLED.turnOff();
    }
}

void wifiSetup() {
    // Connect to Wi-Fi
    WiFi.hostname("ESP PC-PowerControl");
    Serial.println("");
    Serial.println("WiFi SSID: " + Settings::getWirelessSSID());
    Serial.print("Connecting to WiFi");
    WiFi.begin(Settings::getWirelessSSID(), Settings::getWirelessPassword());
    while (WiFi.status() != WL_CONNECTED) {
        casePowerLED.turnOn();
        delay(350);
        casePowerLED.turnOff();
        delay(150);
        Serial.print(".");
    }
    if (motherboardPowerLED.isOn())
        casePowerLED.turnOn();
    else
        casePowerLED.turnOff();

    Serial.println("");
    Serial.println("Connected to WiFi!");

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());
}

void statusPoller() {
    if (motherboardPowerLED.isOn())
        casePowerLED.turnOn();
    else
        casePowerLED.turnOff();

    if (motherboardHDDLED.isOn())
        caseHDDLED.turnOn();
    else
        caseHDDLED.turnOff();
}

void serialReadout() {
    String status = "pwrBtn: ";
    status += (casePowerButton.isPressed() == true) ? "pressed" : "released";
    status += " - rstBtn: ";
    status += (caseResetButton.isPressed() == true) ? "pressed" : "released";
    status += " -|- pwrLed: ";
    status += (motherboardPowerLED.isOn() == true) ? "ON" : "off";
    status += " - hddLed: ";
    status += (motherboardHDDLED.isOn() == true) ? "ON" : "off";

    Serial.print("\033[A\33[2K\r");
    Serial.println(status);
}

void motherboardPowerButtonReleaseCallback() {
    motherboardPowerButton.release();
}
void motherboardResetButtonReleaseCallback() {
    motherboardResetButton.release();
}
void casePowerButtonReleaseCallback() {
    casePowerButton.release();
}
void caseResetButtonReleaseCallback() {
    caseResetButton.release();
}

void motherboardPowerLEDTurnOffCallback() {
    motherboardPowerLED.turnOff();
}
void motherboardHDDLEDTurnOffCallback() {
    motherboardHDDLED.turnOff();
}
void casePowerLEDTurnOffCallback() {
    casePowerLED.turnOff();
}
void caseHDDLEDTurnOffCallback() {
    caseHDDLED.turnOff();
}

void loop() {
    // Control case proxy logic
    scheduler.execute();
}