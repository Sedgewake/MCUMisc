int v1 = 0;
float f1 = 0;
float f2 = 0;

void setup()
{
  pinMode(9, OUTPUT);
  TCCR1B = (TCCR1B & 0b11111000) | 2; //4k
  //Serial.begin(9600);
}

void loop()
{
   v1 = (analogRead(A0) - 220) * 1.75f;
   if (v1 > 1225)
   {
    analogWrite(9, 0);
    return;
   }
   if (v1 > 1000) v1 = 1000;
   if (v1 < 0) v1 = 0;
   f1 = (float)v1 * 0.001f;
   if (f2 > f1) f2 = f2 + ((f1 - f2) * 0.25f);
   else f2 = f2 + ((f1 - f2) * 0.15f);
   v1 = (int)(f2 * 255.0f);
   analogWrite(9, v1);
   //Serial.println(v1);
   delay(15);
}
