#include <Arduino.h>
#include <QTRSensors.h>
#include <SPI.h>
#include <Wire.h>


/*------------------------------For Motor Drive----------------------------*/
int initialSpeed = 70; // just for test
#define leftMotroBaseSpeed initialSpeed
#define rightMotroBaseSpeed initialSpeed
#define maxMotorSpeed 250

// for motor pin configeration
int eLeft = 5;
int eRight = 10;
int pLeft = 7;
int nLeft = 6;
int pRight = 8;
int nRight = 9;



/*-----------------------------For SOner Sensor-----------------------------*/
#define echoPinL 14 // left        // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPinL 15 // left        // attach pin D3 Arduino to pin Trig of HC-SR04

#define echoPinM 16      // middle
#define trigPinM 17      // middle

#define echoPinR 18      // right
#define trigPinR 19      // right

// defines variables
long duration; // variable for the duration of sound wave travel
int distance;  // variable for the distance measurement

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
  Serial.begin(9600);
  // for motor setup
  for (int i = 5; i < 11; i++) 
  {
    pinMode(i, OUTPUT);
  }

  //for soner setup
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinL, INPUT);                          
  pinMode(trigPinM, OUTPUT);
  pinMode(echoPinM, INPUT);                          
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinR, INPUT);

 
}

double distanceL()
{
  digitalWrite(trigPinL, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinL, LOW);
  duration = pulseIn(echoPinL, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}
double distanceM()
{
  digitalWrite(trigPinM, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinM, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinM, LOW);
  duration = pulseIn(echoPinM, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}
double distanceR()
{
  digitalWrite(trigPinR, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinR, LOW);
  duration = pulseIn(echoPinR, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}


void loop()
{
  int frontDistance = distanceM();
  int leftDistance = distanceL();
  int rightDistance = distanceR();
  if (frontDistance < 30)
  {
    wheel(0, 0);
    delay(500);
    int leftDistance = distanceL();
    int rightDistance = distanceR();
    if (leftDistance > rightDistance)
    {
      wheel(-initialSpeed, initialSpeed);
      delay(500); //200
    }
    else
    {
      wheel(initialSpeed, -initialSpeed);
      delay(500);  //200
    }
  }
  else if (leftDistance<15)
  {
    wheel(initialSpeed, 0);
  }
  else if (rightDistance<15)
  {
    wheel(0, initialSpeed);
  }
  else
  {
    wheel(initialSpeed, initialSpeed);
  }
}