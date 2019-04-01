/*
Example, how to read data from a BME280 (here: GY-BME280 5V/3.3V) and display the values on a HELTEC WIFI Kit 32 display
by 3KU_Delta (https://github.com/3KUdelta)

CREDITS:
Basic solution provided by itofficeeu https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62 because Pin21 and Pin22
do not work for this purpose.

Adafrit´s library: 

  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution


Libraries needed:
ThingPulse SSD1306 (https://github.com/ThingPulse/esp8266-oled-ssd1306)
General Adafruit Sensor (Arduino Library Manager)
Adafruit BME280 (Arduino Library Manager)
SoftwWire Steve Marple (Arduino Library Manager)
AsyncDelay Steve Marple (Arduino Library Manager)

Hardware settings Mac:
Heltec_WFIF_Kit_32,80MHz,921600 on /dev/cu.SLAB_USBtoUART
*/

#include <SoftWire.h>
#include <AsyncDelay.h>
#include "SSD1306.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// BME280
#define SEALEVELPRESSURE_HPA (1019) //Check Internet for MSL pressure at our place to calc your elevation mASL
#define TEMP_CORR (-2)              //Manual correction of temp sensor (mine reads 2 degrees C too high)
#define ELEVATION (505)             //Enter your elevation mASL to calculate MSL pressure (QNH) at your place

Adafruit_BME280 bme; // I2C

#define PIN_SDA 4
#define PIN_SCL 15

SoftWire sw(PIN_SDA, PIN_SCL);

SSD1306 display(0x3C, PIN_SDA, PIN_SCL);

float SLpressure_hPa;


void setup() 
{
  Serial.begin(9600);
  Serial.printf("Starting...\r\n");

  pinMode(16,OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);
  delay(50);
  display.init();
  display.setFont(ArialMT_Plain_10);
  delay(50);
  display.drawString( 0, 0, "Starting up ...");
  display.drawString( 0,12, "- and initializing.");
  display.display();
  delay(500);

  bool bme_status;
  bme_status = bme.begin(0x76);  //address either 0x76 or 0x77
  if (!bme_status) {
      display.clear();
      display.drawString( 0, 0, "No valid BME280 found");
      display.drawString( 0, 12, "Please check wiring!");
      display.display();
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }

  //my best way to get correct indoor temperatures
  
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X16,  // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1,  // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5 );
}
void loop() 
{
  bme.takeForcedMeasurement();
  // Print temperature
  float temp = bme.readTemperature();
  temp = temp + TEMP_CORR;
  // print on serial monitor
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("; ");
  // print on display
  display.clear();  // clear display buffer
  String tempvar = String("Temp: ") + (temp) + (" °C");
  display.drawString( 0, 0, tempvar);

  // Print humidity
  float humi = bme.readHumidity();
  // print on serial monitor
  Serial.print("Humidity: ");
  Serial.print(humi);
  Serial.print("; ");
  // print on display
  tempvar = String("Hum: ") + (humi) + (" %");
  display.drawString( 0, 10, tempvar);

  // Print altitude
  float alti = bme.readAltitude(SEALEVELPRESSURE_HPA);
  // print on serial monitor
  Serial.println("Altitude: ");
  Serial.println(alti);
  Serial.print("; ");
  // print on display
  tempvar = String("Alt: ") + (alti) + (" m ASL");
  display.drawString( 0, 20, tempvar); 

  // Print pressure
  float pres = bme.readPressure() / 100.0F;
  // print on serial monitor
  Serial.print("Pressure: ");
  Serial.print(pres);
  Serial.print("; ");
  // print on display
  tempvar = String("Pres: ") + (pres) + (" hPa");
  display.drawString( 0, 30, tempvar);

  // Print QNH
  SLpressure_hPa = (((pres * 100.0)/pow((1-((float)(ELEVATION))/44330), 5.255))/100.0);
  int hPa_rounded=(int)(SLpressure_hPa+.5);
  // print on serial monitor
  Serial.print("Pressure MSL: ");
  Serial.print(hPa_rounded);
  Serial.print("; ");
  // print on display
  tempvar = String("Pres MSL: ") + (hPa_rounded) + (" hPa");
  display.drawString( 0, 40, tempvar);
  
  display.display(); // print display buffer
  delay(1000);
}
