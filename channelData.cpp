#include "HardwareSerial.h"
#include <GyverDS18.h>
#include <microDS3231.h>
#include "Wire.h"
#include <SPI.h>

#include "channelData.h"

#define CS1 5
#define CS2 17
#define CS3 4

bool ChannelData::channel[8];
float ChannelData::channelV;
float ChannelData::channelA[8];
uint16_t ChannelData::acp;
const int ChannelData::spiClk;  // 1 MHz

SPIClass *vspi = NULL;

void ChannelData::init(){
  Wire.begin();

  vspi = new SPIClass(SPI);
  vspi->begin();
}

/* установка выходов расширителя в соответствии с массивом состояний */
void ChannelData::setPins(){
  uint8_t dataHighByte = 255; // Старший байт (P10...P17)
  uint8_t dataLowByte = 0; // Младший байт (P00...P07)

  for(int i = 0; i < 8; i++){
    dataLowByte |= channel[i] << i;
  }

  Wire.beginTransmission(0x20);
  Wire.write(dataLowByte); // Записываем младший байт (P00...P07)
  Wire.write(dataHighByte); // Записываем старший байт (P10...P17)
  Wire.endTransmission();
}

/* считывание данных АЦП */
// U = (опорное напряжение * значение АЦП * коэффициент делителя) / число разрядов АЦП.
// U = (1.8 * chan * 138.89) / 4096 
// с учетом резистора 0.1 Ом умножение U на 100 даст актуальное значение тока
void ChannelData::getCurrentdata(){
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  
  // поочередное считывание каналов микросхем АЦП и перерасчет их в числовые значения
  for(uint8_t channel = 0; channel < 9; channel+4){
    if(channel < 5)
      digitalWrite(CS1, LOW);
    else if ((channel > 4) && (channel < 8))
      digitalWrite(CS2, LOW);
    else digitalWrite(CS3, LOW);

    vspi->transfer(0x05);
    if(channel == 8){
      channelV = ((1.8 * vspi->transfer(0x00) * 138.89) / 4096);
    }
    else {
      channelA[channel] = ((1.8 * vspi->transfer(0x06) * 138.89) / 4096) * 100;
      channelA[channel+1] = ((1.8 * vspi->transfer(0x07) * 138.89) / 4096) * 100;
      channelA[channel+2] = ((1.8 * vspi->transfer(0x08) * 138.89) / 4096) * 100;
      channelA[channel+3] = ((1.8 * vspi->transfer(0x00) * 138.89) / 4096) * 100;
    }

    if(channel < 5)
      digitalWrite(CS1, HIGH);
    else if ((channel > 4) && (channel < 8))
      digitalWrite(CS2, HIGH);
    else digitalWrite(CS3, HIGH);
  }
  vspi->endTransaction();
}