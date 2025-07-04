#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

const int I2C_SDA = 5;
const int I2C_SCL = 6;

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int HALL_SENSOR_PIN = 3;

const float VAL_1 = 7948.8f;
const float WHEEL_CIRCUMFERENCE_M = 2.208f;
volatile unsigned long lastHallTriggerTime = 0;
volatile unsigned long currentTime = 0;
volatile unsigned long rotationTime = 0;
volatile float currentSpeedKmh = 0.0;
volatile float totalMileageM = 0.0;

void IRAM_ATTR hallSensorISR() 
{
  currentTime = millis();
  rotationTime = currentTime - lastHallTriggerTime;
  if (rotationTime > 80)
  {
    currentSpeedKmh = VAL_1 / (float)rotationTime;
    lastHallTriggerTime = currentTime;
    totalMileageM += WHEEL_CIRCUMFERENCE_M;
  }
}

void setup() 
{
  //Serial.begin(115200);
  WiFi.mode(WIFI_OFF);
  btStop();
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(0x3C, true)) 
  {
    Serial.println(F("SH110X allocation failed or not found"));
    for (;;) ;
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Bike Comp");
  display.display();
  delay(1000);

  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), hallSensorISR, FALLING);
}

void loop() 
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 12);
  char speedBuffer[10];
  char mileageBuffer[12];
  noInterrupts();
  dtostrf(currentSpeedKmh, 4, 1, speedBuffer);
  dtostrf(totalMileageM * 0.001f, 5, 2, mileageBuffer);
  interrupts();
  display.print(speedBuffer); 
  display.setTextSize(1);
  display.setCursor(30, 42);
  display.print(mileageBuffer);
  display.print(" km");
  display.display();
  delay(500);
}
