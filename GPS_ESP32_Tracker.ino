#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Wire.h>               
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
//#include <BlynkSimpleEsp32.h>

float latitude , longitude  ;
int   month , day , year , hour, minute, second, centisecond;
String  latitude_string , longitiude_string, month_string , day_string, year_string ;
String   hour_string, minute_string, second_string, centisecond_string;
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
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
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
     for(;;);
  }
  delay(100);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Start up ");
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
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
 // Blynk.begin(auth, ssid, pass);
 // Blynk.virtualWrite(V0, "clr"); 
}
void loop()
{
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read()))
    {
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
        
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Lat: ");
  display.setCursor(40, 0);
  display.print(latitude_string);
  display.setCursor(0, 15);
  display.print("Long: ");
  display.setCursor(40, 15);
  display.print(longitiude_string);
      
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
     display.setCursor(0, 30);
     display.print("Date: ");
     display.setCursor(40, 30);
     display.print(day_string);
     display.print(F("/"));
     display.print(month_string);
     display.print(F("/"));
     display.print(year_string);
  }

   if (gps.time.isValid())
  { 
    display.setCursor(0, 45);
    display.print("Time: ");
    display.setCursor(40, 45);
    if (gps.time.hour() < 10) 
          Serial.print(F("0")),
          display.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    hour = gps.time.hour();
        hour_string = String(hour);
        display.print(hour_string);
        display.print(F(":"));
    if (gps.time.minute() < 10) 
          Serial.print(F("0")),
          display.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    minute = gps.time.minute();
        minute_string = String(minute);
        display.print(minute_string);
        display.print(F(":"));
    
    if (gps.time.second() < 10) Serial.print(F("0")), display.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    second = gps.time.second();
        second_string = String(second);
        display.print(second_string);
        display.print(F("."));
    
    if (gps.time.centisecond() < 10) Serial.print(F("0")), display.print(F("0"));
    Serial.print(gps.time.centisecond());
    centisecond = gps.time.centisecond();
        centisecond_string = String(centisecond);
        display.print(centisecond_string);
   }
 
       // Blynk.virtualWrite(V0, 1, latitude, longitude, "Location");
        display.display();
      }
     delay(1000);
     Serial.println();  
    }
  }  
 // Blynk.run();
}
