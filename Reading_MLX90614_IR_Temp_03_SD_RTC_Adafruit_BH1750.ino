/*************************************************** 
  This is a library example for the MLX90614 Temp Sensor
  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1748
  ----> https://www.adafruit.com/products/1749
  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <BH1750FVI.h> // Sensor Library  //N

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const int chipSelect = 4;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_DS1307 rtc;

BH1750FVI LightSensor;  //N

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup() {
  Serial.begin(9600);

//  Serial.println("Adafruit MLX90614 test");  
  mlx.begin();  

  Serial.print("Initializing SD Card... ");
// see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
     Serial.println("Card failed, or not present");
     return;
  }
  Serial.println("Card initialized.");

  File dataFile = SD.open("IRdata.txt", FILE_WRITE);
  if (dataFile) {
      dataFile.println("DateTime; TAmbient; TObject; Light");
      dataFile.close();    
  } 
  else {
      Serial.println("ERROR Opening Log File");
  }

#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  LightSensor.begin();  //N
  /* Set the address for this sensor
     you can use 2 different address
     Device_Address_H "0x5C"
     Device_Address_L "0x23"
     you must connect Addr pin to A3 .
  */
  LightSensor.SetAddress(Device_Address_H);//Address 0x5C
  // To adjust the slave on other address , uncomment this line
  // lightMeter.SetAddress(Device_Address_L); //Address 0x5C
  /*
     set the Working Mode for this sensor
     Select the following Mode:
     Continuous_H_resolution_Mode
     Continuous_H_resolution_Mode2
     Continuous_L_resolution_Mode
     OneTime_H_resolution_Mode
     OneTime_H_resolution_Mode2
     OneTime_L_resolution_Mode
   
     The data sheet recommanded To use Continuous_H_resolution_Mode
  */
  LightSensor.SetMode(Continuous_H_resolution_Mode);
}

void loop() {
  uint16_t lux = LightSensor.GetLightIntensity();// Get Lux value  //N

  DateTime now = rtc.now();
  Serial.print(now.day(),DEC);Serial.print('/');
  Serial.print(now.month(),DEC);Serial.print('/');Serial.print(now.year(),DEC);
  Serial.print(" ");
  Serial.print(now.hour(),DEC);Serial.print(':');
  Serial.print(now.minute(),DEC);Serial.print(':');Serial.print(now.second(),DEC);  
  Serial.print(",  ");
//  Serial.println();

  Serial.print("TAmbient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print(" C,  TObject = "); Serial.print(mlx.readObjectTempC()); 
  Serial.print(" C,  Light = "); Serial.print(lux); Serial.println(" Lux");

// make a string for assembling the data to log:
  String AmbTempString = "";
  String ObjTempString = "";

  File dataFile = SD.open("IRdata.txt", FILE_WRITE);

  AmbTempString += String(mlx.readAmbientTempC());
  ObjTempString += String(mlx.readObjectTempC());

  String DateTime = "";
  
  DateTime = String(now.day())+String("/")+String(now.month())+String("/")+
    String(now.year())+String(" ")+String(now.hour())+String(":")+
    String(now.minute())+String(":")+String(now.second())+String(";  ");

  // if the file is available, write to it:
  if (dataFile) {
//      dataFile.println("DateTime", "TAmbient, TObject, Lux");
      dataFile.print(DateTime);
      dataFile.print(AmbTempString);
      dataFile.print(";  ");
      dataFile.print(ObjTempString);
      dataFile.print(";  ");
      dataFile.println(lux);
      dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
      Serial.println("ERROR Opening Log File");
  }

  delay(2000);
}
