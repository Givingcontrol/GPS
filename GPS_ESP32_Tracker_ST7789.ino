//
// V0 OLED tft
//V1 SPI IPS 240x240 ST7789


#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Wire.h>               
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
//#include <BlynkSimpleEsp32.h>

#define TFT_CS        15
#define TFT_RST        4 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         2

float latitude , longitude  ;
int   month , day , year , hour, minute, second, centisecond;
String  latitude_string , longitiude_string, month_string , day_string, year_string ;
String   hour_string, minute_string, second_string, centisecond_string;



// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
/*
const char *ssid =  "Your_SSID";     
const char *pass =  "Your_Password"; 
char auth[] = "ec8EX0fnX1y78Oo6pO8lp8Of_AWUd***"; 

WidgetMap myMap(V0); 
WiFiClient client;
*/
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

TinyGPSCustom satsInView(gps, "GPGSV", 3);         // $GPGSV sentence, third element

void setup()
{
  Serial.begin(115200);
  delay(100);

  // OR use this initializer (uncomment) if using a 1.3" or 1.54" 240x240 TFT:
  tft.init(240, 240);           // Init ST7789 240x240
  
  delay(100);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);
  tft.println("Start up ");
 /* 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");             
  }
  Serial.println("");
  Serial.println("WiFi connected!");
*/

 


  SerialGPS.begin(9600, SERIAL_8N1, 16, 17); //GPS TX >ESP32 RX(16), GPS RX >ESP32 TX(17)
 // Blynk.begin(auth, ssid, pass);
 // Blynk.virtualWrite(V0, "clr"); 
}
void loop()
{
  while (SerialGPS.available() > 0) {
      // GPS connecxted
       /* Serial.println("Serial detected ");
          tft.fillScreen(ST77XX_BLACK);
          tft.setCursor(0, 10);
          tft.print("Serial detected ");
          
     delay(500);*/
    if (gps.encode(SerialGPS.read()))
    {
       /*   Serial.println("GPS query ");
           tft.fillScreen(ST77XX_BLACK);
          tft.setCursor(0, 10);
          tft.print("GPS query ");
     delay(500);*/
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        latitude_string = String(latitude , 6);
        longitude = gps.location.lng();
        longitiude_string = String(longitude , 6);
        Serial.print("Latitude = ");
        Serial.println(latitude_string);
        Serial.print("Longitude = ");
        Serial.println(longitiude_string);
        
 tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);
  tft.println("Lat: ");
  tft.setCursor(120, 0);
   tft.setTextColor(ST77XX_MAGENTA);
  tft.print(latitude_string);
   tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 60);
  tft.print("Long: ");
  tft.setCursor(120, 60);
   tft.setTextColor(ST77XX_MAGENTA);
  tft.print(longitiude_string);
      
   if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
        month = gps.date.month();
        month_string = String(month);
        day = gps.date.day();
        day_string = String(day);
        year = gps.date.year();
        year_string = String(year);
     tft.setTextColor(ST77XX_WHITE);
     tft.setCursor(0, 120);
     tft.print("Date: ");
     tft.setCursor(120, 120);
     tft.print(day_string);
     tft.print(F("/"));
     tft.print(month_string);
     tft.print(F("/"));
     tft.print(year_string);
  }

   if (gps.time.isValid())
  { 
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(0, 180);
    tft.print("Time: ");
    tft.setCursor(120, 180);
    if (gps.time.hour() < 10) 
          Serial.print(F("0")),
          tft.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    hour = gps.time.hour();
        hour_string = String(hour);
        tft.print(hour_string);
        tft.print(F(":"));
    if (gps.time.minute() < 10) 
          Serial.print(F("0")),
          tft.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    minute = gps.time.minute();
        minute_string = String(minute);
        tft.print(minute_string);
        tft.print(F(":"));
    
    if (gps.time.second() < 10) Serial.print(F("0")), tft.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    second = gps.time.second();
        second_string = String(second);
        tft.print(second_string);
        tft.print(F("."));
    
    if (gps.time.centisecond() < 10) Serial.print(F("0")), tft.print(F("0"));
    Serial.print(gps.time.centisecond());
    centisecond = gps.time.centisecond();
        centisecond_string = String(centisecond);
        tft.print(centisecond_string);
   }
 
       // Blynk.virtualWrite(V0, 1, latitude, longitude, "Location");
      //  tft.tft();
      }
     delay(1000);
     Serial.println();  
    }
  }  
 // Blynk.run();
}
