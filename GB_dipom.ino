#include "webServerEsp.h"
#include "channelData.h"

#define MISO 5
#define MOSI 6
#define SCK 4
#define CS 10

WebServerEsp myServer;
ChannelData data;

String prepareJsonData(){
  String tmp = "";
  tmp += "{ ";

  tmp += "\"setting\": { \"mode\":";
  tmp += myServer.mode;
  tmp += ", \"wifiSsid\":";
  tmp += "\"" + myServer.wifiSsid + "\"";
  tmp += ", \"wifiPass\":";
  tmp += "\"" + myServer.wifiPass + "\"";
  tmp += " },";

  tmp += "\"temper\": { \"shtTemper\":";
  tmp += data.shtTemper;
  tmp += ", \"shtHumid\":";
  tmp += data.shtHumid;
  tmp += " },";

  tmp += "\"pinStatus\": { \"btn\":";
  tmp += data.btn1;
  tmp += ", \"led\": [";
  tmp += (String)data.led1 + "," + (String)data.led2;
  tmp += "]";

  tmp += ", \"relay\": [";
  for(int i = 0; i < 8; i++){
    tmp += (String)data.channel[i];
    if(i < 7)
       tmp += ",";
  }
  tmp += "]";

  tmp += ", \"relayV\": [";
  for(int i = 0; i < 8; i++){
    tmp += (String)data.channelV[i];
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

  myServer.init();

  /* настройка датчиков */
  data.init();
  /* настройка датчиков */

  
}

/* цикл */
void loop() {
  myServer.dataOut = prepareJsonData();

  myServer.serverHandleClient();

  // Serial.print("AP IP address: ");
  // Serial.println(myIP);

  // readOW();
  // getDStime();

  data.getShtTH();
  data.getPinStatus();


  // listDir(SPIFFS, "/", 0);
  // readFileFS(SPIFFS, "/index.html");

  // printUartData();

  //delay(300);

}

/* вывод данных в UART */
// void printUartData(){
//   Serial.print("DS temp: ");
//   Serial.println(dataSensors.dsTemper);
//   Serial.print("SHT temp: ");
//   Serial.println(dataSensors.shtTemper);
//   Serial.print("SHT humid: ");
//   Serial.println(dataSensors.shtHumid);
 
//   Serial.print("Hour: ");
//   Serial.println(dataSensors.hour);
//   Serial.print("Minutes: ");
//   Serial.println(dataSensors.min);
//   Serial.print("Secundes: ");
//   Serial.println(dataSensors.sec);
  
//   Serial.println("");

//   Serial.print("Btn1: ");
//   Serial.println(pinStatus.btn1);
//   Serial.print("Led1: ");
//   Serial.println(pinStatus.led1);
//   Serial.print("Led2: ");
//   Serial.println(pinStatus.led2);

//   Serial.println("");
// }
