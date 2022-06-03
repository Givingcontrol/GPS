/*
 * V0 change struct mesaage and show on display
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// These pins will also work for the 1.8" TFT shield.
  //***********USING THIS**************
 //#define TFT_MOSI 23  // Data out
 //#define TFT_SCLK 18  // Clock out 
  #define TFT_CS        15
  #define TFT_RST        4 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         2

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);



// Define variables to store incoming readings
float incomingTemp;
float incomingPres;

// Define variables to store incoming GPS readings
float incomingLat, incomingLong, incoming_m;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    float temp;
    float pres;
    float  lat_ , long_, meters_ ;
    int   hr_, min_, sec_, centisec_, mth_ , day_, yr_ ;

} struct_message;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

/*
//Structure example to send GPS data
//Must match the receiver structure
typedef struct struct_message_GPS {
   String  lat_string , long_string, m_string , d_string, y_string ;
   String   h_string, min_string, sec_string, centisec_string, meters_string;
} struct_message_GPS;

// Create a struct_message called GPS_Readings to hold sensor readings
struct_message_GPS GPS_incomingReadings;


esp_now_peer_info_t peerInfo;

*/

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingTemp = incomingReadings.temp;
  incomingPres = incomingReadings.pres;
  incomingLat = incomingReadings.lat_;
  incomingLong = incomingReadings.long_ ;
  incoming_m = incomingReadings.meters_ ;
}

/*
// Callback when data is received
void OnGPSDataRecv(const uint8_t * mac, const uint8_t *incomingGPSData, int len) {
  memcpy(&GPS_incomingReadings, incomingGPSData, sizeof(GPS_incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingLat = GPS_incomingReadings.lat_string;
  incomingLong = GPS_incomingReadings.long_string;
  incoming_m = GPS_incomingReadings.m_string;
  }
*/
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // OR use this initializer if using a 1.8" TFT screen with offset such as WaveShare:
  // tft.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab

 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

 
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
 // esp_now_register_recv_cb(OnGPSDataRecv);

  // brief tft start up message 
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  delay(1000);
 // brief tft start up message
}
 
void loop() {
  
  updateDisplay();
  delay(1000);
}


void updateDisplay(){
  // Display Readings on OLED Display
  // tft Readings on OLED tft
  //**************************
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 10);
  tft.print("T: ");
  tft.setCursor(50, 10);
  tft.print(incomingTemp);
  tft.cp437(true);
  tft.write(248);
  tft.println("C");
  tft.setCursor(0, 30);
  tft.print("Pres : ");
   tft.setCursor(50, 30);
  tft.print(incomingPres);
  tft.println("hPa");
  
  //********
  tft.setCursor(0, 70);
  tft.print("Lat: ");
  tft.setCursor(50, 70);
   tft.setTextColor(ST77XX_YELLOW);
  tft.print(incomingLat);
   tft.setTextColor(ST77XX_WHITE);
   tft.setCursor(0, 90);
  tft.print("Long: ");
  tft.setCursor(50, 90);
   tft.setTextColor(ST77XX_YELLOW);
  tft.print(incomingLong);
   tft.setTextColor(ST77XX_WHITE);
   tft.setCursor(0, 110);
  tft.print("Alt: ");
  tft.setCursor(50, 110);
   tft.setTextColor(ST77XX_YELLOW);
   tft.print(incoming_m);
    tft.print("m");


  //****************************
  
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Temperature: ");
  Serial.print(incomingReadings.temp);
  Serial.println(" ºC");
  Serial.print("Pressure: ");
  Serial.print(incomingReadings.pres);
  Serial.println(" hPa");
  Serial.println();
}
