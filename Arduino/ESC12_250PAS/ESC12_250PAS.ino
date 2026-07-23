int v1 = 0;
float f1 = 0;
float f2 = 0;
float f3 = 0;
float v5 = 0;

const int HALL_SENSOR_PIN = 3;
const float VAL_1 = 7500.0f;

volatile unsigned long currentTime = 0;
volatile unsigned long currentTick = 0;
volatile unsigned long tickTime = 0;
volatile unsigned long tickTimer = 0;
volatile unsigned long lastHallTriggerTime = 0;
volatile float currentRPM = 0.0;

void IRAM_ATTR hallSensorISR()
{
  currentTick = millis();
  tickTime = currentTick - lastHallTriggerTime;
  if (tickTime > 70)
  {
    currentRPM = VAL_1 / (float)tickTime;
    lastHallTriggerTime = currentTick;
  }
}

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  TCCR1B = (TCCR1B & 0b11111000) | 2; //4k
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), hallSensorISR, FALLING);
  //Serial.begin(9600);
}

void loop()
{
   currentTime = millis();
   tickTimer = currentTime - lastHallTriggerTime;
   if (tickTimer > 650)
   {
	   noInterrupts();
	   currentRPM = 0.0;
	   interrupts();
   }
   
   noInterrupts();
   f3 = currentRPM * 0,022f;
   interrupts();
   if (f3 > 1.0f) f3 = 1.0f;
	
   v1 = (analogRead(A0) - 220) * 1.75f;
   if (v1 > 1225)
   {
    analogWrite(9, 0);
    return;
   }
   
   if (v1 > 1000) v1 = 1000;
   if (v1 < 0) v1 = 0;
   f1 = (float)v1 * 0.001f;
   
   if (f1 > f3) v5 = f1;
   else v5 = f3;
   
   if (f2 > v5) f2 = f2 + ((v5 - f2) * 0.20f);
   else f2 = f2 + ((v5 - f2) * 0.12f);
   v1 = (int)(f2 * 255.0f);
   analogWrite(9, v1);
   //Serial.println(v1);
   delay(15);
}