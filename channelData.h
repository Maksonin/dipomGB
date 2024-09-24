#pragma once
#include <Arduino.h>

class ChannelData
{
  public:
    static bool channel[8];
    static float channelV;
    static float channelA[8];
    static uint16_t acp;
    static const int spiClk = 1000000;  // 1 MHz
    
    void init();
    void setPins();
    void getCurrentdata();
};