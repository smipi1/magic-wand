#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "Wire.h"
#include "whatever.h"
#include "Adafruit_TCS34725.h"

int hueDegree = 182;
int userStringLenght = 40;
String user = "FFMjW08OQiwcSFS24TIRybSJ5nVZQT2BEermgfvU";
String bridgeIpAddress = "192.168.88.253";

static hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

static int clipPut(char clip[], size_t clipStringLength)
{
  if(WiFi.status() == WL_CONNECTED){   
                
    HTTPClient http;   
    http.begin("http://" + bridgeIpAddress + "/api/" + user + "/lights/1/state");          
    http.addHeader("Content-Type", "text/plain");       
       
    int httpCode = http.sendRequest("PUT", (uint8_t*)clip, clipStringLength);
    String payload = http.getString();                               
    
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    http.end();  //Close connection
    
    return 0;
  }
  
  return 1;
}

static int setLampToRgbColor(rgb val)
{
  hsv value = rgb2hsv(val);
  Serial.println("h " +  String(value.h) + "s " + String(value.s) + "v " + String(value.v));
  
  int Hue = value.h * hueDegree;
  int Sat = value.s * 255;
  int Bri = value.v;
  Serial.println("Hue " +  String(Hue) + " Sat " + String(Sat) + " Bri " + String(Bri));
  
  String clipBody =  "{\"on\":true,\"bri\":" + String(Bri) + ", \"hue\":" +  String(Hue) + " ,\"sat\":" + String(Sat) + "}";
  Serial.println("clipBody " +  clipBody);
  
  char clip[clipBody.length() + 1];
  clipBody.toCharArray(clip, clipBody.length() + 1);
  Serial.println("clip " + String(clip));

  if (clipPut(clip, sizeof(clip)))
  {
    Serial.println("http PUT failed");
    return 1;
  }

  return 0;
}

void setup() 
  {
  Serial.begin(115200);                                 //Serial connection
  WiFi.begin("MikroTik-Kevin", "12345678");             //WiFi connection

  while (WiFi.status() != WL_CONNECTED)
  {  //Wait for the WiFI connection completion
    delay(500);
    Serial.println("Waiting for connection...");
  }

  Serial.println("connected");
}

void loop() {
  if (Serial.available()) {                     

    String action = Serial.readString();         
    Serial.println(action);  
 
    if (action.equals("true"))
    {
      char clip[] = "{\"on\":true}";
      if (clipPut(clip, sizeof(clip)))
      {
        Serial.println("failed to send clip command");
      }
    }
    else if (action.equals("false"))
    {
      char clip[] = "{\"on\":false}";
      if (clipPut(clip, sizeof(clip)))
      {
        Serial.println("failed to send clip command");
      }
    }
    if (action.startsWith("set"))
    {
      if (action.length() == (userStringLenght + 3))
      {
        user = action.substring(3,43);
        Serial.println("user set to " + user);
      }  
    }
    else if (action.startsWith("rgb"))
    {
      rgb val;
      String hexstring = action.substring(3,9);
      Serial.println("hexstring " + hexstring);
      
      long number = strtol( &hexstring[0], NULL, 16);
      Serial.println("number " + String(number));

      val.r = number >> 16 & 0xFF;
      val.g = number >> 8 & 0xFF;
      val.b = number & 0xFF;
      Serial.println("r " +  String(val.r) + " g " + String(val.g) + " b " + String(val.b));

      if (setLampToRgbColor(val))
      {
        Serial.println("rgb color not send to lamp succesfull");
      }
    }
  }
}
