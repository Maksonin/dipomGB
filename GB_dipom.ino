#include <GyverDS18.h>
#include <microDS3231.h>
#include "Wire.h"
#include "SHT2x.h"

#define OneWirePin 10

#define btnPin 21
#define ledPin1 0
#define ledPin2 1

GyverDS18Single ds(OneWirePin);
MicroDS3231 rtc;
SHT2x sht;

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
  uint16_t acp;
} pinStatus;

/* чтение температуры с ds18b20 */
void readOW(){
  if (ds.ready()) {         // измерения готовы по таймеру
        if (ds.readTemp()) {  // если чтение успешно
          dataSensors.dsTemper = ds.getTemp();
        } else {
          dataSensors.dsTemper = 255;
        }

        ds.requestTemp();  // запрос следующего измерения
    }
}

/* чтение времени с ds */
void getDStime(){
  DateTime now = rtc.getTime();
  dataSensors.sec = now.second;
  dataSensors.min = now.minute;
  dataSensors.hour = now.hour;
}

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

/* настройка */
void setup() {
  Serial.begin(9600);

  ds.requestTemp();  // первый запрос на измерение температуры DS18B20

  Wire.begin();
  sht.begin();

  if (!rtc.begin()) {
    Serial.println("DS3231 not found");
  }

  DateTime initTime;
  initTime.second = 0;
  rtc.setTime(initTime);

  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  pinStatus.led1 = true;

}

/* цикл */
void loop() {

  readOW();
  getDStime();
  getShtTH();
  getPinStatus();

  printUartData();

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
