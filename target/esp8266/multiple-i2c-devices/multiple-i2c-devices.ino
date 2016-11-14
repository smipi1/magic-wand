#include <Wire.h>
#include "Adafruit_TCS34725.h"

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */
   
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS  ,  TCS34725_GAIN_1X );

/* Initialise with specific int time and gain values */
//Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_1X);

const uint8_t MPU=0x68;  // I2C address of the MPU-6050


bool initGt521(void)
{
  Wire.begin(4, 5); // sda, scl
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  return 1;
}

void readGt521(void)
{
  int16_t AcX,AcY,AcZ,Tmp;
  
  size_t numberOfRegisters = 14;
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);                                 // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, numberOfRegisters, true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  
  Serial.print(AcX);
  Serial.print("  "); Serial.print(AcY);
  Serial.print("  "); Serial.println(AcZ);
}

void setup(void) {
  Serial.begin(115200);

  if (initGt521()) {
    Serial.println("Found GY-521");
  } else {
    Serial.println("No GY-521 found ... check your connections");
    while (1);
  }
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  pinMode(16, OUTPUT);
  digitalWrite(16, LOW); // @gremlins Bright light, bright light!
  // Now we're ready to get readings!
}

void loop(void) {
//  uint16_t r, g, b, c;
//  
//  delay(10); 
//  tcs.getRawData(&r, &g, &b, &c);
//  delay(10); 
//  digitalWrite(16, HIGH);   // turn the LED on (HIGH is the voltage level)
//  
//  Serial.print(r, DEC); Serial.print(", ");
//  Serial.print(g, DEC); Serial.print(", ");
//  Serial.print(b, DEC); Serial.print(", ");
//  Serial.print(c, DEC); Serial.print(", ");
////  delay(100);  
//  delay(10); 
//  tcs.getRawData(&r, &g, &b, &c);
//  delay(10); 
//  digitalWrite(16, LOW);   // turn the LED on (HIGH is the voltage level)
//  Serial.print(r, DEC); Serial.print(", ");
//  Serial.print(g, DEC); Serial.print(", ");
//  Serial.print(b, DEC); Serial.print(", ");
//  Serial.print(c, DEC); Serial.println("");

  readGt521();
  delay(50);
}
