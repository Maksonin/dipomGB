#pragma once
#include <Arduino.h>


class ChannelData
{
  public:
    static float dsTemper;
    static float shtTemper;
    static float shtHumid;

    static bool btn1;
    static bool led1;
    static bool led2;
    static bool channel[8];
    static uint8_t channelV[8];
    static uint8_t channelA[8];
    static uint16_t acp;
    
    void init();
    void getShtTH();
    void getPinStatus();
    void setPins();
};