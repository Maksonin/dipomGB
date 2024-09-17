#pragma once
#include <Arduino.h>

class ChannelData;

class WebServerEth {
  public:
    static uint8_t ethStatus;
    static IPAddress lanIp;
    static String dataMain;
    static String dataOut;
    static String dataIn;
    
    static ChannelData *chData;

    void init();
    void handleClient();
};