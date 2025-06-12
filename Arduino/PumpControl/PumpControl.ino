int work = 0;
int count = 0;

void setup() 
{
  pinMode (3, INPUT_PULLUP);
  pinMode (4, INPUT_PULLUP);
  pinMode (7, OUTPUT);
  work = 0;
  count = 0;
}

void loop() 
{
  if (digitalRead(4) == LOW)
  {
    if (digitalRead(3) == LOW)
    {
      digitalWrite(7, HIGH);
      work = 1;
    }
    else
    {
      if (work == 1)
      {
        count++;
        if (count > 35)
        {
          digitalWrite(7, LOW);
          count = 0;
          work = 0;
        }
      }
    }
  }
  else
  {
    digitalWrite(7, LOW);
    work = 0;
    count = 0;
  }
  delay(500);
}
