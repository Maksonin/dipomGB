#pragma once
#include <Arduino.h>
#include "FS.h"

enum {
  WF_AP,
  LAN,
  WF_AP_LAN,
  WF_STA,
  WF_STA_LAN
} deviceMode;

class WebServerEsp {
  public:
    static uint8_t mode;
    static String wifiSsid;
    static String wifiPass;

    static String dataOut;
    static String dataIn;

    void init();
    void serverHandleClient();
    static void handle_OnConnect();
    static void handle_Data();
    static void handle_Relay();
    static String prepareJsonData();
    void clearParameters(const char *id);
    static void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
    static String readFileFS(fs::FS &fs, const char *path);
};