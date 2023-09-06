// Main.h
#pragma once
#define ONBOARD_LED 2

#include <Arduino.h>
#include <TaskScheduler.h>

#ifdef ESP32
    #include <AsyncTCP.h>
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
    #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
//#include <Crypto.h> // Included in setting

#include <Button.h>
#include <LED.h>
#include <Settings.h>
#include <Web.h>

// Resources
#include "../data/web/index_html.h"
#include "../data/web/login_html.h"
#include "resources.h"