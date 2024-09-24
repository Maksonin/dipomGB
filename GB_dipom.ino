#include "webServerEsp.h"
#include "channelData.h"
#include "Wire.h"

#define MISO 5
#define MOSI 6
#define SCK 4
#define CS 10

WebServerEsp myServer;

ChannelData data;
ChannelData *chData = &data;

// функция формирует JSON для отправки в веб-интерфейс
String prepareJsonData(){
  String tmp = "";
  tmp += "{ ";

  tmp += "\"setting\": { \"mode\":";
  tmp += myServer.mode;
  tmp += ", \"wifiSsid\":";
  tmp += "\"" + myServer.wifiSsid + "\"";
  tmp += ", \"wifiPass\":";
  tmp += "\"" + myServer.wifiPass + "\"";
  tmp += ", \"wifiIp\":";
  tmp += "\"" + myServer.wifiIp.toString() + "\"";
  tmp += ", \"lanIp\":";
  tmp += "\"" + myServer.lanIp.toString() + "\"";
  tmp += " },";

  tmp += "\"pinStatus\": { \"relayV\":";
  tmp += (String)data.channelV;

  tmp += ", \"relay\": [";
  for(int i = 0; i < 8; i++){
    tmp += (String)data.channel[i];
    if(i < 7)
       tmp += ",";
  }
  tmp += "]";

  tmp += ", \"relayA\": [";
  for(int i = 0; i < 8; i++){
    tmp += (String)data.channelA[i];
    if(i < 7)
       tmp += ",";
  }
  tmp += "]";
  tmp += " }";

  tmp += " }";
  //Serial.println(tmp);
  return tmp;
}

/* настройка */
void setup() {
  Serial.begin(9600);

  /* настройка веб-интерфейсов */
  myServer.init();

  /* настройка датчиков */
  data.init();

  myServer.chData = chData;
}

/* цикл */
void loop() {
  myServer.dataOut = prepareJsonData(); // подготовка JSON посылки
  
  myServer.serverHandleClient(); // прослушивание запросов к веб-серверам

  data.setPins(); // устновка состояния каналов управления

  data.getCurrentdata(); // чтение значений тока в каналах управления
}