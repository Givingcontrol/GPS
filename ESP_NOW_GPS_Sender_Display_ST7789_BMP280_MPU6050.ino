

/*
 * V0 Adding GPS config, start up and tft to screen
 * V0 Adding MPU6050 accel/gyro to send data via espnow
 * V1 Put MPU reading into seperate function
 * 
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  // ESP32 Guide: https://RandomNerdTutorials.com/esp32-mpu-6050-accelerometer-gyroscope-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
   The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
     Check out the links above for our tutorials and wiring diagrams.
  These tfts use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************


*/
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
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
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

//GPS TX >ESP32 RX(16), GPS RX >ESP32 TX(17)
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

 
//ESP32:: SCL = GPIO22 SDA= GPIO21
Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;


// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x34, 0x94, 0x54, 0x35, 0x39, 0x8C};// ESP32 #4

//GPS Variables
float latitude , longitude  ;
int   month , day , year , hour, minute, second, centisecond, meters;
float  latitude_string , longitiude_string, meters_string  ;
int   hour_string, minute_string, second_string, centisecond_string,month_string , day_string, year_string ;

//  variables to store bmp280 readings to be sent
float temperature;
float pressure;

//MPU variables
float AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ, Temp6050;


// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    float temp;
    float pres;
    float  lat_ , long_, meters_ ;
    int   hr_, min_, sec_, centisec_, mth_ , day_, yr_ ;
    float accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temp_6050;

} struct_message;

// Create a struct_message called bmp280Readings to hold sensor readings
struct_message Readings;

/*
//Structure example to send GPS data
//Must match the receiver structure
typedef struct struct_message_GPS {
   String  lat_string , long_string, m_string , d_string, y_string ;
   String   h_string, min_string, sec_string, centisec_string, meters_string;

} struct_message_GPS;

// Create a struct_message called GPS_Readings to hold sensor readings
struct_message_GPS GPS_Readings;
*/

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}


 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

   SerialGPS.begin(9600, SERIAL_8N1, 16, 17); //GPS TX >ESP32 RX(16), GPS RX >ESP32 TX(17)
 

  // Init bmp280 sensor
  bool status = bmp.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid bmp280 sensor, check wiring!");
    while (1);
  }

   if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU-6050 sensor");

 // Use this initializer if using a 1.8" TFT screen:
  //tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // OR use this initializer if using a 1.8" TFT screen with offset such as WaveShare:
  // tft.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab
// OR use this initializer (uncomment) if using a 1.3" or 1.54" 240x240 TFT:
  tft.init(240, 240);           // Init ST7789 240x240

   
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
 
  // brief tft start up message 
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.println("Hello World!");
  delay(1000);
 // brief tft start up message
}
 
void loop() {
   
  get_BMP_Readings();
  get_GPS_Readings();
  get_MPU_Readings();
  // Set values to send
  Readings.temp = temperature;
  Readings.pres = pressure;
  Readings.lat_ = latitude_string;
  Readings.long_  = longitiude_string;
  Readings.meters_ = meters_string;

  // Send BMP280 message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &Readings, sizeof(Readings));

 
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
     }
/*
      // Send GPS message via ESP-NOW
   esp_err_t resultGPS = esp_now_send(broadcastAddress, (uint8_t *) &GPS_Readings, sizeof(GPS_Readings));

  if (resultGPS == ESP_OK) {
    Serial.println("Sent GPS with success");
  }
  else {
    Serial.println("Error sending the GPS data");
     }
 */ 
  updatetft();
  delay(1000);
}

void get_GPS_Readings(){
  while (SerialGPS.available() > 0) {
  if (gps.encode(SerialGPS.read()))
    {if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        latitude_string = float(latitude);
        longitude = gps.location.lng();
        longitiude_string = float(longitude);
        meters = gps.altitude.meters();
        meters_string = float(meters);
        Serial.print("Latitude = ");
        Serial.println(latitude_string);
        Serial.print("Longitude = ");
        Serial.println(longitiude_string);
        
      }
}
}
}

void get_BMP_Readings(){
  temperature = bmp.readTemperature();
  pressure = (bmp.readPressure() / 100.0F);
}

void get_MPU_Readings(){
   sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float f_AccelX = a.acceleration.x;
  AccelX = f_AccelX;
  float  f_AccelY = a.acceleration.y; 
  AccelY = f_AccelY;
  float f_AccelZ =  a.acceleration.z;
  AccelZ = f_AccelZ;
  
  float f_GyroX = g.gyro.x;
  GyroX = f_GyroX;
  float f_GyroY = g.gyro.y;
  GyroY  = f_GyroY;
  float  f_GyroZ = g.gyro.z;
  GyroZ = f_GyroZ;
  //Temp6050;

/*
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);
  
 Serial.println("Accelerometer Test ");
  Serial.println(f_GyroX);
  
  Serial.print("Accelerometer ");
  Serial.print("X: ");
  Serial.print(a.acceleration.x, 1);
  Serial.print(" m/s^2, ");
  Serial.print("Y: ");
  Serial.print(a.acceleration.y, 1);
  Serial.print(" m/s^2, ");
  Serial.print("Z: ");
  Serial.print(a.acceleration.z, 1);
  Serial.println(" m/s^2");

  tft.println("Accelerometer - m/s^2");
  tft.print(a.acceleration.x, 1);
  tft.print(", ");
  tft.print(a.acceleration.y, 1);
  tft.print(", ");
  tft.print(a.acceleration.z, 1);
  tft.println("");

  Serial.print("Gyroscope ");
  Serial.print("X: ");
  Serial.print(g.gyro.x, 1);
  Serial.print(" rps, ");
  Serial.print("Y: ");
  Serial.print(g.gyro.y, 1);
  Serial.print(" rps, ");
  Serial.print("Z: ");
  Serial.print(g.gyro.z, 1);
  Serial.println(" rps");

  tft.println("Gyroscope - rps");
  tft.print(g.gyro.x, 1);
  tft.print(", ");
  tft.print(g.gyro.y, 1);
  tft.print(", ");
  tft.print(g.gyro.z, 1);

  //************
  // test
  GyroX = f_GyroX;
  tft.setTextColor(ST77XX_RED);
  tft.println(" ");
   tft.println("Gyroscope TEST ");
  tft.print(GyroX, 1);
  tft.print(", ");
  tft.print(GyroY);
  tft.print(", ");
  tft.print(GyroZ);
  //************************
 */
}


void updatetft(){
  
  // tft Readings on OLED tft
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);
  tft.print("T: ");
  tft.setCursor(100, 0);
  tft.print(temperature);
  tft.cp437(true);
  tft.write(248);
  tft.println("C");
  tft.setCursor(0, 40);
  tft.print("Pres : ");
   tft.setCursor(100, 40);
  tft.print(pressure);
  tft.print("hPa");
  
  //********
  //GPS
  tft.setCursor(0, 70);
  tft.print("Lat: ");
  tft.setCursor(100, 70);
   tft.setTextColor(ST77XX_YELLOW);
  tft.print(latitude_string);
   tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 90);
  tft.print("Long: ");
  tft.setCursor(100, 90);
   tft.setTextColor(ST77XX_YELLOW);
  tft.print(longitiude_string);
   tft.setTextColor(ST77XX_WHITE);
   tft.setCursor(0, 110);
  tft.print("Alt: ");
  tft.setCursor(100, 110);
   tft.setTextColor(ST77XX_YELLOW);
   tft.print(meters_string);
    tft.print("m");
   
  //********
  // MPU6050
  tft.setTextColor(ST77XX_WHITE);
   tft.setCursor(0, 130);
  tft.print("Accelerometer - m/s^2");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(0, 150);
  tft.print(AccelX, 1);
  tft.print(", ");
  tft.print(AccelY, 1);
  tft.print(", ");
  tft.print(AccelZ, 1);
  tft.print(" m/s^2");
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 170);
   tft.print("Gyroscope - rps");
   tft.setTextColor(ST77XX_YELLOW);
   tft.setCursor(0, 190);
  tft.print(GyroX, 1);
  tft.print(", ");
  tft.print(GyroY, 1);
  tft.print(", ");
  tft.print(GyroZ, 1);
   tft.print("rps");
  tft.setCursor(15, 210);
   tft.setTextColor(ST77XX_GREEN);
  tft.print(success);
  
  
  // tft Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" ??C");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");
  Serial.println();
 
}
