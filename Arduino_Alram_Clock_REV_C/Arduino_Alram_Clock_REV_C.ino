/*
   Arduino Clock with Coloer SPI oled
   Tempature reading that changes color based on tempature
   Battery level that changes color based on capasity
   BY: C4PT41ND34DP00L
   
   OLED: sclk = 13, data = 11, cd = 8, cs = 10, reset = 9
   Bat: Positive = A1, GND = GND (if 5v arduino must connect + of bat to A1 before any boost converter)
   TMP36: Sense Pin = A0, GND = GND, Positive = 3.3v
   DS3113: GND = GND,  VCC = 5V, SDA = A4, SCL = A5
*/

#include <SPI.h>
#include "Ucglib.h"
#include <Wire.h>
#include "RTClib.h"
#include <Battery.h>
//#include <SFE_BMP180.h>

// Setup RTC
RTC_DS1307 RTC;
char monthString[37] = {"JanFebMarAprMayJunJulAugSepOctNovDec"};
int  monthIndex[122] = {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33};
char daysOfTheWeek[7][12] = {"Sun","Mon", "Tues", "Wend", "Thu", "Fri", "Sat"};

/***********************************************************************************************/

//Setup bat mon (Min Voltage, Max Voltage, Sinsor pin)
Battery battery(3000, 4200, A1);

/***********************************************************************************************/
Ucglib_SSD1331_18x96x64_UNIVISION_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 8, /*cs=*/ 10, /*reset=*/ 9);

//TMP36 Pin Variables
boolean useTMP36 = true;  // set this to false if you don not use TMP36
//boolean useTMP36 = false;
#define aref_voltage 1.1 // we tie to teh internal ref (1.1 V)
int tempPin = 0; //the analog pin that the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures
int tempReading; // the analog reading from the sensor

/***********************************************************************************************/

void setup(void)
{
  analogReference(INTERNAL);
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  delay(1000);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  battery.begin(3700, 1.0); //Set first number to batteries mV
  // init done
  ucg.clearScreen();
}

void loop(void)
{
  DateTime now = RTC.now();
  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  ucg.setColor(0, 204, 0, 204);
  ucg.setColor(1, 0, 0, 0);
  ucg.setFont(ucg_font_u8glib_4_hf);
  ucg.setPrintPos(15, 54);
  ucg.print(daysOfTheWeek[now.dayOfTheWeek()]);
  ucg.print(" ");
  for (int i = 0; i <= 2; i++) {
    ucg.print(monthString[monthIndex[now.month() - 1] + i]);
  }
  ucg.print('/');
  ucg.print(now.day(), DEC);
  ucg.print('/');
  ucg.print(now.year(), DEC);

  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  ucg.setColor(0, 0, 51, 204);
  ucg.setColor(1, 0, 0, 0);
  ucg.setFont(ucg_font_u8glib_4_hf);
  ucg.setPrintPos(20, 63);
  ucg.print("=/\\=");
  ucg.print("  ");
  ucg.print(now.year() + 100);
  ucg.print(".");
  ucg.println(now.dayOfYear());

  /***********************************************************************************************/
  // display time in digital format
  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  ucg.setColor(0, 255, 255, 255);
  ucg.setColor(1, 0, 0, 0);
  ucg.setFont(ucg_font_helvB24_hf);
  ucg.setPrintPos(6, 42);
  const uint8_t h = now.hour();
  const uint8_t hr_12 = h % 12;


  if (hr_12 < 10) {              
    ucg.print("");
    ucg.print((h > 12) ? h - 12 : ((h == 0) ? 12 : h), DEC); // Conversion to AM/PM
  }
  else {
    ucg.print((h > 12) ? h - 12 : ((h == 0) ? 12 : h), DEC); // Conversion to AM/PM
  }

  printDigits(now.minute());
  if (h < 12) {                // Adding the AM/PM sufffix
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setFont(ucg_font_04b_03b_hf);
    ucg.setColor(0, 255, 255, 255);
    ucg.setColor(1, 0, 0, 0);
    ucg.setPrintPos(90, 30);
    ucg.print("A");
  }
  else {
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setFont(ucg_font_04b_03b_hf);
    ucg.setColor(0, 255, 255, 255);
    ucg.setColor(1, 0, 0, 0);
    ucg.setPrintPos(90, 30);
    ucg.print("P");
  }
  /***********************************************************************************************/

  // now add temperature if needed

  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  ucg.setFont(ucg_font_04b_03b_hf);
  ucg.setColor(0, 51, 204, 51);
  ucg.setColor(1, 0, 0, 0);
  ucg.setPrintPos(70, 10);
  if (useTMP36 == true) {
    //getting the voltage reading from the temperature sensor
    tempReading = analogRead(tempPin);
    // converting that reading to voltage, for 3.3v arduino use 3.3
    float voltage = tempReading * aref_voltage;
    voltage /= 1024.0;
    // now print out the temperature
    int temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree with 500 mV offset
    float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
    //Setting the color of the text
    if (temperatureF > 80) {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_04b_03b_hf);
      ucg.setColor(0, 255, 0, 0);
      ucg.setColor(1, 0, 0, 0);
    }
    else if (temperatureF < 65) {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_04b_03b_hf);
      ucg.setColor(0, 0, 102, 255);
      ucg.setColor(1, 0, 0, 0);
    }

    else {
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(ucg_font_04b_03b_hf);
      ucg.setColor(0, 51, 204, 51);
      ucg.setColor(1, 0, 0, 0);
    }
    ucg.print(temperatureF);
    ucg.print("F");

  }

  //Battery Level Display
  //Set up color of battery level
  if (battery.level() < 80) {
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setFont(ucg_font_04b_03b_hf);
    ucg.setColor(0, 255, 255, 0);
    ucg.setColor(1, 0, 0, 0);
  }
  else if (battery.level() < 60) {
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setFont(ucg_font_04b_03b_hf);
    ucg.setColor(0, 255, 153, 0);
    ucg.setColor(1, 0, 0, 0);
  }
  else if (battery.level() < 40) {
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setFont(ucg_font_04b_03b_hf);
    ucg.setColor(0, 255, 51, 0);
    ucg.setColor(1, 0, 0, 0);
  }
  else if (battery.level() < 25) {
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setFont(ucg_font_04b_03b_hf);
    ucg.setColor(0, 255, 0, 0);
    ucg.setColor(1, 0, 0, 0);
  }
  else {
    ucg.setFontMode(UCG_FONT_MODE_SOLID);
    ucg.setFont(ucg_font_04b_03b_hf);
    ucg.setColor(0, 0, 204, 0);
    ucg.setColor(1, 0, 0, 0);
  }
  ucg.setPrintPos(0, 10);
  ucg.print(battery.level());
  ucg.print("%");
  ucg.print("  ");

  //delay(500);
  // **************** End Main Loop *****************
}
void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  ucg.print(":");
  if (digits < 10)
    ucg.print('0');
  ucg.print(digits);

}
