#include <Arduino.h>





int same = 80; // just for test
#define leftMotroBaseSpeed same
#define rightMotroBaseSpeed same
#define maxMotorSpeed 250




// for motor pin configeration
int eLeft = 5;
int eRight = 10;
int pLeft = 7;
int nLeft = 6;
int pRight = 8;
int nRight = 9;

void wheel(int left, int right)
{
  if (left >= maxMotorSpeed)
    left = maxMotorSpeed;
  if (right >= maxMotorSpeed)
    right = maxMotorSpeed;
  if (left <= -maxMotorSpeed)
    left = -maxMotorSpeed;
  if (right <= -maxMotorSpeed)
    right = -maxMotorSpeed;

  if (left >= 0)
  {
    digitalWrite(pLeft, HIGH);
    digitalWrite(nLeft, LOW);
    analogWrite(eLeft, left);
  }
  else
  {
    digitalWrite(pLeft, LOW);
    digitalWrite(nLeft, HIGH);
    analogWrite(eLeft, -left);
  }

  if (right >= 0)
  {
    digitalWrite(pRight, HIGH);
    digitalWrite(nRight, LOW);
    analogWrite(eRight, right);
  }
  else
  {
    digitalWrite(pRight, LOW);
    digitalWrite(nRight, HIGH);
    analogWrite(eRight, -right);
  }

  Serial.print("left speed :: ");
  Serial.print(left);
  Serial.print("      ");
  Serial.print("right speed :: ");
  Serial.println(right);

  return;
}




void setup()
{
  for (int i = 5; i < 11; i++) // for motor setup
  {
    pinMode(i, OUTPUT);
  }

  Serial.begin(9600);
}

void loop()
{
  wheel(100, 130);
  delay(200);
}