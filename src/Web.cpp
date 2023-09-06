#include <Web.h>

Web::Web() {}

Web::Web(AsyncWebServer *server) {
    this->server = server;
}

String Web::generateSessionToken(int length) {
    String charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    String token = "";

    for (int i = 0; i < length; i++) {
        int randomIndex = random(charset.length());
        token += charset[randomIndex];
    }
    return token;
}
String Web::generateSessionToken() {
    return Web::generateSessionToken(64);
}

void Web::clearSessionToken() {
    this->sessionToken = "";
}
bool Web::isValidSessionToken(String token) {
    if (token == nullptr || this->sessionToken == nullptr)
        return false;
    if (token.equals("") || this->sessionToken.equals(""))
        return false;

    // Serial.println("Active valid session: " + this->sessionToken);
    // Serial.println("Checking token: " + token);
    return token.equals(this->sessionToken);
}
void Web::handleLogin(AsyncWebServerRequest *request, bool redirectOnSuccessfull) {
    if (request->method() == HTTP_POST) {
        // Read form data
        String username = request->arg("username");
        String password = request->arg("password");

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

        // Validate username and password hash
        if (username == Settings::getWebServerUsername() && strcmp(hashString, Settings::getWebServerPassword().c_str()) == 0) {
            // Authentication succeeded, generate session token
            String sessionToken = generateSessionToken();
            this->sessionToken = sessionToken;

            // Set session token cookie
            AsyncWebServerResponse *response = request->beginResponse(302, "application/json", "{\"status\": \"OK\"}");
            response->addHeader("Set-Cookie", "session-token=" + sessionToken + "; path=/");
            if (redirectOnSuccessfull)
                response->addHeader("Location", "/");
            request->send(response);
        } else {
            // Authentication failed
            request->send(401, "text/plain", "{\"status\": \"Unauthorized\"}");
        }
    } else {
        request->send(405, "text/plain", "Method Not Allowed");
    }
}

bool Web::isAuthenticated(AsyncWebServerRequest *request) {
    // Get the value of the "session-token" cookie from the request
    String cookieValue;
    if (request->hasHeader("Cookie")) {
        String cookieHeader = request->header("Cookie");
        // Serial.println(cookieHeader);
        int cookieIndex = cookieHeader.indexOf("session-token=");
        if (cookieIndex != -1) {
            int endIndex = cookieHeader.indexOf(';', cookieIndex);
            if (endIndex == -1) {
                endIndex = cookieHeader.length();
            }
            cookieValue = cookieHeader.substring(cookieIndex + 14, endIndex);
        } else {
            // Serial.println("No \"session-token\" cookie set, user not authenticated.");
        }
    } else {
        // Serial.println("No cookies, user not authenticated.");
    }

    // Check if the cookie value matches the expected session token
    if (!this->isValidSessionToken(cookieValue)) {
        // not logged in
        //request->redirect("/login");
        return false;
    }
    // Serial.println("User authenticated.");
    return true;
    // all good!
}