#include <ESP8266WiFi.h>
int v1 = 0;
float f1 = 0;
float f2 = 0;

void setup()
{
  // put your setup code here, to run once:
  //Serial.begin(115200);
  pinMode(16, OUTPUT);
  WiFi.mode(WIFI_OFF);
}

void loop() 
{
  v1 = (analogRead(A0) - 277) * 1.733f;
  if (v1 > 1000) v1 = 1000;
  if (v1 < 0) v1 = 0;
  f1 = (float)v1 * 0.001f;
  f2 = f2 + ((f1 - f2) * 0.05f);
  v1 = (int)(f2 * 1000.0f);
  digitalWrite(16, 1);
  delayMicroseconds(1000 + v1);
  digitalWrite(16, 0);
  delay(18);
  //Serial.println(v1);
  //delay(700);
}
