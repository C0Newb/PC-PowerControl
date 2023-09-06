#include <Settings.h>

String pinToString(Pin pin) {
    switch (pin) {
        case MOTHERBOARD_HDD_LED:
            return "motherboardHDDLED";
        case MOTHERBOARD_POWER_BUTTON:
            return "motherboardPowerButton";
        case MOTHERBOARD_RESET_BUTTON:
            return "motherboardResetButton";

        case CASE_POWER_LED:
            return "casePowerLED";
        case CASE_HDD_LED:
            return "caseHDDLED";
        case CASE_POWER_BUTTON:
            return "casePowerButton";
        case CASE_RESET_BUTTON:
            return "caseResetButton";
        
        default:
            return "unknown";
    }
}

Preferences Settings::preferences = Preferences();

// Pins
int Settings::getPin(Pin pin) {
    String pinName = pinToString(pin);
    return preferences.getInt(pinName.c_str(), -1);
}
void Settings::setPin(Pin pin, int gpio) {
    String pinName = pinToString(pin);
    preferences.putInt(pinName.c_str(), gpio);
}


// Button presses

int Settings::getLongButtonPressDuration() {
    return preferences.getInt("longButtonPressDuration", 5000);
}

void Settings::setLongButtonPressDuration(int duration) {
    preferences.putInt("longButtonPressDuration", duration);
}


int Settings::getShortButtonPressDuration() {
    return preferences.getInt("shortButtonPressDuration", 250);
}

void Settings::setShortButtonPressDuration(int duration) {
    preferences.putInt("shortButtonPressDuration", duration);
}


// Wireless
String Settings::getWirelessSSID() {
    return preferences.getString("wirelessSSID", "");
}
void Settings::setWirelessSSID(String ssid) {
    preferences.putString("wirelessSSID", ssid);
}
String Settings::getWirelessPassword() {
    return preferences.getString("wirelessPassword", "");
}
void Settings::setWirelessPassword(String password) {
    preferences.putString("wirelessPassword", password);
}

String Settings::getWirelessAPPassword() {
    return preferences.getString("wirelessAPPassword", "PC-PowerController_1234");
}
void Settings::setWirelessAPPassword(String password) {
    preferences.putString("wirelessAPPassword", password);
}

int Settings::getWebServerPort() {
    return preferences.getInt("webServerPort", 80);
}
void Settings::setWebServerPort(int port) {
    preferences.putInt("webServerPort", port);
}

String Settings::getWebServerUsername() {
    return preferences.getString("webServerUsername", "admin");
}
void Settings::setWebServerUsername(String username) {
    preferences.putString("webServerUsername", username);
}
String Settings::getWebServerPassword() {
    String defaultHash = "101886f37334fc7155212cb606810cba72d824f149d35f4e6be718efcb326645"; // it's "pcpowercontrol", btw
    return preferences.getString("webServerPasswordHash", defaultHash);
}
void Settings::setWebServerPassword(String password) {
    // Hash the provided password
    SHA256 sha256;
    sha256.update(password.c_str(), password.length());
    uint8_t hash[sha256.hashSize()];
    sha256.finalize(hash, sha256.hashSize());

    // Convert the hash to a hexadecimal string
    char hashString[2 * sha256.hashSize() + 1];
    for (int i = 0; i < sha256.hashSize(); i++) {
            snprintf(&hashString[i * 2], 3, "%02x", hash[i]);
    }
    hashString[128] = '\0'; // Null-terminate the string

    preferences.putString("webServerPasswordHash", hashString);
}
