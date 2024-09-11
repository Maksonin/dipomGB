#include <Preferences.h>
//#include <Ethernet.h>
#include <WiFi.h>
#include <WebServer.h>
#include "FS.h"
#include "SPIFFS.h"

#include <GyverDS18.h>
#include <microDS3231.h>
#include "Wire.h"
#include "SHT2x.h"

// #define OneWirePin 20
#define FORMAT_SPIFFS_IF_FAILED true

#define btnPin 21
#define ledPin1 0
#define ledPin2 1

#define MISO 5
#define MOSI 6
#define SCK 4
#define CS 10

// GyverDS18Single ds(OneWirePin);
MicroDS3231 rtc;
SHT2x sht;

WebServer server(80);
IPAddress myIP;

Preferences pref;

struct setting {
  bool mode; // 0 - работа по сети, 1 - работа по вифи
  String wifiSsid;
  String wifiPass;
} setting;

// структура хранит значения параметров с датчиков 
struct dataSensors {
  float dsTemper;
  float shtTemper;
  float shtHumid;
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
} dataSensors;

// структура хранит значения пинов
struct pinStatus{
  bool btn1;
  bool led1;
  bool led2;
  bool chanel[8];
  uint8_t chanelV[8];
  uint8_t chanelA[8];
  uint16_t acp;
} pinStatus;


void clearParameters(const char *id){
  pref.remove(id);
}

/* чтение температуры с ds18b20 */
// void readOW(){
//   if (ds.ready()) {         // измерения готовы по таймеру
//         if (ds.readTemp()) {  // если чтение успешно
//           dataSensors.dsTemper = ds.getTemp();
//         } else {
//           dataSensors.dsTemper = 255;
//         }

//         ds.requestTemp();  // запрос следующего измерения
//     }
// }

/* чтение времени с ds */
// void getDStime(){
//   DateTime now = rtc.getTime();
//   dataSensors.sec = now.second;
//   dataSensors.min = now.minute;
//   dataSensors.hour = now.hour;
// }

/* чтение температуры и влажности с sht21 */
void getShtTH(){
  bool success  = sht.read();
  if (success == false)
  {
    Serial.println("Failed read");
  }
  else
  {
    dataSensors.shtTemper = sht.getTemperature();
    dataSensors.shtHumid = sht.getHumidity();
  }
}

/* получение данных о gpio */
void getPinStatus(){
  pinStatus.btn1 = digitalRead(btnPin);

  digitalWrite(ledPin1, pinStatus.led1);
  digitalWrite(ledPin2, pinStatus.led2);
}

/* Функция формирующая ответ на запрос веб-сервера */
void handle_OnConnect() {
  server.send(200, "text/html", readFileFS(SPIFFS, "/index.html")); 
}

void handle_Data(){
  server.send(200, "text/html", prepareJsonData());
}

String prepareJsonData(){
  String tmp = "";
  tmp += "{ ";

  tmp += "\"temper\": { \"shtTemper\":";
  tmp += dataSensors.shtTemper;
  tmp += ", \"shtHumid\":";
  tmp += dataSensors.shtHumid;
  tmp += " },";

  tmp += "\"pinStatus\": { \"btn\":";
  tmp += pinStatus.btn1;
  tmp += ", \"led\": [";
  tmp += (String)pinStatus.led1 + "," + (String)pinStatus.led2;
  tmp += "]";

  tmp += ", \"relay\": [";
  for(int i = 0; i < 8; i++){
    tmp += (String)pinStatus.chanel[i];
    if(i < 7)
       tmp += ",";
  }
  tmp += "]";

  tmp += ", \"relayV\": [";
  for(int i = 0; i < 8; i++){
    tmp += (String)pinStatus.chanelV[i];
    if(i < 7)
       tmp += ",";
  }
  tmp += "]";

  tmp += ", \"relayA\": [";
  for(int i = 0; i < 8; i++){
    tmp += (String)pinStatus.chanelA[i];
    if(i < 7)
       tmp += ",";
  }
  tmp += "]";
  tmp += " }";

  tmp += " }";
  Serial.println(tmp);
  return tmp;
}

/* чтение пути SPIFFS */
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
/* чтение пути SPIFFS */

/* чтение файла SPIFFS */
String readFileFS(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  String tmp;
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return "";
  }

  Serial.println("- read from file:");
  
  while (file.available()) {
    tmp += file.readString();
  }
  
  Serial.println(tmp);
  file.close();
  return tmp;
}
/* чтение файла SPIFFS */

/* настройка */
void setup() {
  Serial.begin(9600);
  
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  /* восстановление настроек из энергонезависимой памяти */
  pref.begin("setting", false);
  // clearParameters("wifiSsid");
  // clearParameters("wifiPass");
  setting.wifiSsid = pref.getString("wifiSsid","0");
  setting.wifiPass = pref.getString("wifiPass","0");
  pref.end();
  /* восстановление настроек из энергонезависимой памяти */

  Serial.println(setting.wifiSsid);
  Serial.println(setting.wifiPass);

  /* настройка wifi */
  if(setting.wifiSsid == "0"){
    WiFi.softAP("userAP", "012345678");
    server.begin();
    Serial.println("HTTP server started");
    myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.on("/", handle_OnConnect);
    server.on("/data", handle_Data);
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(setting.wifiSsid, setting.wifiPass);
    // Wait for connection
    int i = 0;
    while ((WiFi.status() != WL_CONNECTED) && (i < 10)) {
      delay(1000);
      i++;
      Serial.print(".");
    }

    Serial.println("");

    if(WiFi.status() != WL_CONNECTED){
      Serial.println(" NOT CONNECT! ");
    }
    else {
      Serial.print("Connected to ");
      Serial.println(setting.wifiSsid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }
  /* настройка wifi */

  /* настройка датчиков */
  // ds.requestTemp();  // первый запрос на измерение температуры DS18B20

  Wire.begin();
  sht.begin();

  // if (!rtc.begin()) {
  //   Serial.println("DS3231 not found");
  // }

  // DateTime initTime;
  // initTime.second = 0;
  // rtc.setTime(initTime);

  /* настройка датчиков */

  /* настройка gpio */
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  pinStatus.led1 = true;

  /* настройка gpio */

}

/* цикл */
void loop() {
  server.handleClient();

  // Serial.print("AP IP address: ");
  // Serial.println(myIP);

  // readOW();
  // getDStime();
  getShtTH();
  getPinStatus();

  // listDir(SPIFFS, "/", 0);
  // readFileFS(SPIFFS, "/index.html");

  // printUartData();

  delay(300);

}

/* вывод данных в UART */
void printUartData(){
  Serial.print("DS temp: ");
  Serial.println(dataSensors.dsTemper);
  Serial.print("SHT temp: ");
  Serial.println(dataSensors.shtTemper);
  Serial.print("SHT humid: ");
  Serial.println(dataSensors.shtHumid);
 
  Serial.print("Hour: ");
  Serial.println(dataSensors.hour);
  Serial.print("Minutes: ");
  Serial.println(dataSensors.min);
  Serial.print("Secundes: ");
  Serial.println(dataSensors.sec);
  
  Serial.println("");

  Serial.print("Btn1: ");
  Serial.println(pinStatus.btn1);
  Serial.print("Led1: ");
  Serial.println(pinStatus.led1);
  Serial.print("Led2: ");
  Serial.println(pinStatus.led2);

  Serial.println("");
}
