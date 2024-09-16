#include "WString.h"
#pragma once
#include <Arduino.h>

class WebServerEth {
  public:
    static uint8_t ethStatus;
    static String dataOut;
    void init();
    void handleClient();
};