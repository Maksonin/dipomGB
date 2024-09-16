#include <SPI.h>
#include <Ethernet.h>
#include "webServerEthernet.h"

uint8_t WebServerEth::ethStatus;
String WebServerEth::dataOut;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

EthernetServer lanServer(80);

void WebServerEth::init(){
  Ethernet.init(10);
  Ethernet.begin(mac);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found");
    ethStatus = 0;
    return;
  }
  else if (Ethernet.hardwareStatus() == EthernetW5100) {
    Serial.println("W5100 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5200) {
    Serial.println("W5200 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5500) {
    Serial.println("W5500 Ethernet controller detected.");
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    ethStatus = 0;
    return;
  }
  ethStatus = 2;
  lanServer.begin();
  Serial.print("w5500 server is at ");
  Serial.println(Ethernet.localIP());
}

void WebServerEth::handleClient(){
  // listen for incoming clients
  EthernetClient client = lanServer.available();
  if (client) {
    Serial.println("new client");
    // an HTTP request ends with a blank line
    bool currentLineIsBlank = true;
    String tmp = "-";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.print(c);
        tmp += c;

        if (c == '\n' && currentLineIsBlank) {
          int i = 0;
          int space = 0;
          String param[3];
          while(tmp[i] != '\0'){
              param[space] += tmp[i];
              if((tmp[i] == ' ')||(tmp[i] == ':'))
                  space++;
              if(space == 3)
                  break;
              i++;
          }

          //Serial.println(tmp);
          Serial.println(param[0]);
          Serial.println(param[1]);
          Serial.println(param[2]);

          param[1].trim(); // удаление пробелов из начала и конца строки

          if(param[1].equalsIgnoreCase("/")){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            //Serial.println(dataOut);
            client.print(dataOut);
            break;
          }
          else if(param[1].equalsIgnoreCase("/data")){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("data");
            break;
          }
          else if(param[1].equalsIgnoreCase("/relay:")){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("relay");
            client.println(param[2]);
            break;
          }
          else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("no page");
            break;
          }
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}