#include <GyverDS18.h>
#include <microDS3231.h>
#include "Wire.h"
#include "SHT2x.h"

#include "channelData.h"

#define btnPin 21
#define ledPin1 0
#define ledPin2 1

//MicroDS3231 rtc;
SHT2x sht;
// dataSensors dataSensors;
// pinStatus pinStatus;

float ChannelData::dsTemper;
float ChannelData::shtTemper;
float ChannelData::shtHumid;

bool ChannelData::btn1;
bool ChannelData::led1;
bool ChannelData::led2;
bool ChannelData::channel[8];
uint8_t ChannelData::channelV[8];
uint8_t ChannelData::channelA[8];
uint16_t ChannelData::acp;

void ChannelData::init(){
  /* настройка gpio */
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  Wire.begin();
  sht.begin();
}

/* чтение температуры и влажности с sht21 */
void ChannelData::getShtTH(){
  bool success  = sht.read();
  if (success == false)
  {
    Serial.println("Failed read");
  }
  else
  {
    shtTemper = sht.getTemperature();
    shtHumid = sht.getHumidity();
  }
}

/* получение данных о gpio */
void ChannelData::getPinStatus(){
  btn1 = digitalRead(btnPin);

  digitalWrite(ledPin1, channel[0]);
  digitalWrite(ledPin2, channel[1]);
}