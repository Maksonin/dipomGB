#include <WiFi.h>
#include <WebServer.h>
#include "webServerEsp.h"
#include <Preferences.h>
#include "SPIFFS.h"

#include <uri/UriBraces.h>

#include "channelData.h"

uint8_t WebServerEsp::mode;
String WebServerEsp::wifiSsid;
String WebServerEsp::wifiPass;
String WebServerEsp::dataOut;
String WebServerEsp::dataIn;

#define FORMAT_SPIFFS_IF_FAILED true

Preferences pref;

WebServer server(80);
IPAddress myIP;

void WebServerEsp::init(){
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  /* восстановление настроек из энергонезависимой памяти */
  pref.begin("setting", false);

  pref.putString("wifiSsid","Netta Wi-Fi");
  pref.putString("wifiPass","hopelovehappiness");
  // clearParameters("wifiSsid");
  // clearParameters("wifiPass");

  wifiSsid = pref.getString("wifiSsid","0");
  wifiPass = pref.getString("wifiPass","0");
  pref.end();
  /* восстановление настроек из энергонезависимой памяти */

  Serial.println(wifiSsid);
  Serial.println(wifiPass);

  /* настройка wifi */
  if(wifiSsid == "0"){
    mode = WF_AP;
    WiFi.softAP("userAP", "012345678");

    Serial.println("HTTP server started");
    myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }
  else {
    mode = WF_STA;
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSsid, wifiPass);
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
      Serial.println(wifiSsid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }
  /* настройка wifi */
  server.begin();
  server.on("/", handle_OnConnect);
  server.on("/data", handle_Data);
  server.on(UriBraces("/relay:{}"), handle_Relay);
}

void WebServerEsp::serverHandleClient(){
  server.handleClient();
}

/* Функция формирующая ответ на запрос "/" веб-сервера */
void WebServerEsp::handle_OnConnect() {
  server.send(200, "text/html", readFileFS(SPIFFS, "/index.html")); 
}

/* Функция формирующая ответ на запрос "/data" веб-сервера */
void WebServerEsp::handle_Data(){
  server.send(200, "text/html", dataOut);
}

/* Функция формирующая ответ на запрос "/relay:{}" веб-сервера */
void WebServerEsp::handle_Relay(){
  String relay = server.pathArg(0);
  Serial.print("Включено реле - ");
  Serial.println(relay);
  uint8_t i = relay.toInt() - 1;
  
  // if(pinStatus.channel[i] == 1)
  //   pinStatus.channel[i] = 0;
  // else
  //   pinStatus.channel[i] = 1;

  server.send(200, "text/html","OK");
}

/* Функция удаляет элемент id в файловой системе */
void WebServerEsp::clearParameters(const char *id){
  pref.remove(id);
}


/* чтение пути SPIFFS */
void WebServerEsp::listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
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
String WebServerEsp::readFileFS(fs::FS &fs, const char *path) {
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