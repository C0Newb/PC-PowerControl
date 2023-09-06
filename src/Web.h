#pragma once

#include <Arduino.h>
#ifdef ESP32
    #include <AsyncTCP.h>
#else
    #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include <Settings.h>

class Web {
    private:
        // Current session token
        String sessionToken;
        AsyncWebServer* server;

    public:
        static String generateSessionToken();
        static String generateSessionToken(int length);

        Web();
        Web(AsyncWebServer *server);

        void handleLogin(AsyncWebServerRequest *request, bool redirectOnSuccessfull);
        bool isValidSessionToken(String token);
        void clearSessionToken();
        bool isAuthenticated(AsyncWebServerRequest *request);
};