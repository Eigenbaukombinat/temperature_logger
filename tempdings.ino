#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SD.h>
#include <cactus_io_AM2302.h>
#include <RTClib.h>

//
// DHT temp sensor
//
// pin 2 == 5V (software redefined)
// pin 3 == data
// pin 4 == GND (redefined)
//
// define data pin for temperature and setup the dht
#define TEMP_PIN 3
AM2302 dht(TEMP_PIN);

//
// TM1637 4-digit LED display
//
// CLK == PIN5
// DATA == PIN6
// GND == GND
// VCC == 5V
#define CLK 5
#define DATA 6
// init display
SevenSegmentExtended display = SevenSegmentExtended(CLK, DATA);
// define special characters like "°C" and "%"
const uint8_t degree[] = { 0b01100011, 0b00111001 };
const uint8_t percent[] = { 0b01100011, 0b01011100 };

//
// Real-Time Clock
//
// SDA == SDA
// SCL == SCL
// VCC == 3.3V
// GND == GND
RTC_DS3231 rtc;
//DS3231  rtc();

//
// SD Card
//
// CS == PIN 10
// DI== MOS == PIN 11
// DO == MISO == PIN 12
// CLK == PIN 13
// GND == GND
// 5V == 5V
#define SD_CS 10
// file-handle for writing
File file;
// array for date-time string
char data[20];


void setup() {
  // start display
  display.init();
  display.clear();
  display.setCursor(0, 2);
  Serial.begin(9600);

  // clock setup
  rtc.begin();
  // (optional) for setting current date and time; attention: uses compile time!
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // cheat for DHT power supply, use pins instead
  // pin2 == 5V
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  // pin4 == GND
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  
  // init temp sensor and wait 1000ms before first measurement
  dht.begin();
  delay(1000);

  //
  // SD
  //
  // start SD Card on cable select pin SD_CS
  SD.begin(SD_CS);
}

void loop() {
  // read temp and humidity
  dht.readTemperature();
  dht.readHumidity();

  // display temperature
  display.clear();
  display.print(int(dht.temperature_C));
  // add "°C" and wait 2000ms
  display.printRaw(degree, 2, 2);
  delay(2000);
  
  // display humidity
  display.clear();
  display.print(int(dht.humidity));
  // add a percentage sign and wait 2000ms
  display.printRaw(percent, 2, 2);
  delay(2000); 

  // display date and wait 2000ms
  DateTime now = rtc.now();
  sprintf(data, "%02d%02d", now.day(), now.month());
  display.print(data);
  delay(2000); 

  // display time and wait 2000ms
  display.printTime(now.hour(), now.minute());
  delay(2000);

  // format date time for logging
  sprintf(data, "%4d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second() );
   
  // debug output if wanted to serial console
  if(true)
  {
    Serial.print(data);
    Serial.print(" ");
    Serial.print(dht.temperature_C);
    Serial.print("°C ");
    Serial.print(dht.humidity);
    Serial.println("%");
  }

  // open file and write content to sd-card
  file = SD.open("measure.txt", FILE_WRITE);
  if(file) {
    // write data to sd card
    file.print(data);
    file.print(" ");
    file.print(dht.temperature_C);
    file.print("°C ");
    file.print(dht.humidity);
    file.println("%");
    file.flush();
    file.close();
  }
  else {
    // on error: display ERRR on display
    display.clear();
    display.print("ERRR");
    delay(2000);
  }


}
