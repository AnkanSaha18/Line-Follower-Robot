// Make sure to install the library
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <QTRSensors.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//----------------QTR Sensor----------------
QTRSensors qtr;
const uint8_t SensorCount = 10;
uint16_t sensorValues[SensorCount];

// thrational value declaretion
int threationalValue[10] = {100, 80, 80, 80, 80, 80, 80, 80, 80, 100};
int lastPosition = 0;

//-------------------------For Motor Driver and Motor-----------------

// Set up the drive motor carrier pins
int eLeft = 5;
int eRight = 10;
int pLeft = 7;
int nLeft = 6;
int pRight = 8;
int nRight = 9;

#define leftMotroBaseSpeed 43
#define rightMotroBaseSpeed 43
#define maxMotorSpeed 80

void wheel(int left, int right)
{

    Serial.print("left ::  ");
    Serial.print(left);
    Serial.print("            right ::  ");
    Serial.println(right);

    left > maxMotorSpeed ? left = maxMotorSpeed : 1;
    right > maxMotorSpeed ? right = maxMotorSpeed : 1;

    left < -maxMotorSpeed ? left = -maxMotorSpeed : 1;
    right < -maxMotorSpeed ? right = -maxMotorSpeed : 1;

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
}

void readIRSensor(int IRValue[])
{

    qtr.read(sensorValues);
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        int a = sensorValues[i];
        if (a < threationalValue[i])
        {
            IRValue[i] = 0;
            Serial.print(a);
        }
        else
        {
            IRValue[i] = 1;
            Serial.print(a);
        }
        Serial.print("  ");
    }
    Serial.println();
}

void wait()
{
    int IRValue[SensorCount];
    readIRSensor(IRValue);
    Serial.println("wait in");
    while (IRValue[2] + IRValue[3] + IRValue[4] + IRValue[5] + IRValue[6] + IRValue[7] < 1)
    {
        readIRSensor(IRValue);
        Serial.println("waitting");
    }
    Serial.print("wait out");

    return;
}

void lineFollow()
{
    int IRValue[SensorCount];
    readIRSensor(IRValue);

    int leftSensors = IRValue[0] + IRValue[1];
    int middleSensors = IRValue[4] + IRValue[5];
    int rightSensors = IRValue[8] + IRValue[9];

    
    if (leftSensors && rightSensors == 0)
    {
        wheel(-52, 52);
    }
    else if (leftSensors == 0 && rightSensors)
    {
        wheel(52, -52);
    }
    else
    {
        wheel(leftMotroBaseSpeed, rightMotroBaseSpeed);
    }
}

void setup()
{
    Serial.begin(9600);

    // ------------ For IR pin setup ----------------
    qtr.setTypeRC();
    qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7, A8, A9}, SensorCount);
    qtr.setEmitterPin(12);

    // ------------- for motor setup ----------------
    for (int i = 5; i < 11; i++)
    {
        pinMode(i, OUTPUT);
    }
}

void loop()
{
    lineFollow();
}
