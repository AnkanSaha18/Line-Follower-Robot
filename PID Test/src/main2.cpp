// Make sure to install the library
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <QTRSensors.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//------------------Display----------------

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//----------------QTR Sensor----------------
QTRSensors qtr;
const uint8_t SensorCount = 10;
uint16_t sensorValues[SensorCount];

// thrational value declaretion
int threationalValue[10] = {120, 100, 100, 100, 100, 100, 100, 100, 100, 120};
int lastPosition = 0;

//-------------------PID---------------------
float Kp = 0.001; // set up the constants value
float Ki = 0.0005;
float Kd = 0.000;
int P;
int I;
int D;

int lastError = 0;
boolean onoff = false;

//-------------------------For Motor Driver and Motor-----------------
// Increasing the maxspeed can damage the motors - at a value of 255 the 6V motors will receive 7,4 V
const uint8_t maxspeeda = 90;
const uint8_t maxspeedb = 90;
const uint8_t basespeeda = 60; // Right Motor Speed
const uint8_t basespeedb = 60; // Left Motor Speed

// Set up the drive motor carrier pins
int eLeft = 5;
int eRight = 10;
int pLeft = 7;
int nLeft = 6;
int pRight = 8;
int nRight = 9;

// Set up the buttons pins
int buttoncalibrate = 17; // pin A3
int buttonstart = 2;

void calibration();
void forward_brake(int, int);
void PID_control();

void setup()
{
    Serial.begin(9600);

    // ------ For IR pin setup----------
    qtr.setTypeRC();
    qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7, A8, A9}, SensorCount);
    qtr.setEmitterPin(12);

    //------------For Display setup-----------
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.display();
    delay(200);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(1);
    // ------------- for motor setup ----------------
    for (int i = 5; i < 11; i++)
    {
        pinMode(i, OUTPUT);
    }

    forward_brake(0, 0);
}

void calibration()
{
    digitalWrite(LED_BUILTIN, HIGH);
    for (uint16_t i = 0; i < 400; i++)
    {
        qtr.calibrate();
    }
    digitalWrite(LED_BUILTIN, LOW);
}

void forward_brake(int posa, int posb)
{
    // set the appropriate values for aphase and bphase so that the robot goes straight
    // digitalWrite(aphase, LOW);
    // digitalWrite(bphase, HIGH);
    // analogWrite(aenbl, posa);
    // analogWrite(benbl, posb);

    display.setCursor(0, 16);
    display.setTextSize(2);
    display.setTextColor(1);

    display.print(posb);
    display.print("    ");
    display.println(posa);
    display.display();

    Serial.print("left ::  ");
    Serial.print(posa);
    Serial.print("            right ::  ");
    Serial.println(posb);

    if (posa >= 0)
    {
        digitalWrite(pLeft, HIGH);
        digitalWrite(nLeft, LOW);
        analogWrite(eLeft, posa);
    }
    else
    {
        digitalWrite(pLeft, LOW);
        digitalWrite(nLeft, HIGH);
        analogWrite(eLeft, -posa);
    }

    if (posb >= 0)
    {
        digitalWrite(pRight, HIGH);
        digitalWrite(nRight, LOW);
        analogWrite(eRight, posb);
    }
    else
    {
        digitalWrite(pRight, LOW);
        digitalWrite(nRight, HIGH);
        analogWrite(eRight, -posb);
    }
}
void readIRSensor(int IRValue[], int callibreat = 0)
{

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(1);
    display.setCursor(0, 0);

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
        // callibration  parpous
        if (callibreat)
        {
            IRValue[i] = a;
        }
        Serial.print("   ");
    }
    Serial.println();

    for (int i = SensorCount - 1; i >= 0; i--)
    {
        display.print(IRValue[i]);
    }
    Serial.println();
    display.println();
    display.display();
}

float getPosition()
{
    int IRValue[SensorCount];
    double total = 0;
    double weightValue = 0;

    readIRSensor(IRValue);
    for (int i = 0; i < SensorCount; i++)
    {
        total += IRValue[i];
        weightValue += IRValue[i] * (i);
    }

    if (total == 0)
    {
        // If it last read to the left of center, return 0.
        if (lastPosition < (SensorCount - 1) * 1000 / 2)
        {
            return 0;
        }
        // If it last read to the right of center, return the max.
        else
        {
            return (SensorCount - 1) * 1000;
        }
    }

    double linePosotion = weightValue / total;
    return linePosotion * 1000;
}

void PID_control()
{
    double desiredPosition = 4500;
    uint16_t position = getPosition();
    lastPosition = position;
    int error = desiredPosition - position;

    display.setCursor(35, 32);
    display.println(error);
    display.display();

    P = error;
    I = I + error;
    D = error - lastError;
    lastError = error;
    int motorspeed = P * Kp + I * Ki + D * Kd;

    int motorspeeda = basespeeda - motorspeed;
    int motorspeedb = basespeedb + motorspeed;

    if (motorspeeda > maxspeeda)
    {
        motorspeeda = maxspeeda;
    }
    if (motorspeedb > maxspeedb)
    {
        motorspeedb = maxspeedb;
    }
    if (motorspeeda < 0)
    {
        motorspeeda = 0;
    }
    if (motorspeedb < 0)
    {
        motorspeedb = 0;
    }
    // Serial.print(motorspeeda);Serial.print(" ");Serial.println(motorspeedb);
    forward_brake(motorspeeda, motorspeedb);
}

void loop()
{
    PID_control();

    //    forward_brake(30, 30);

    //    int IRValue[SensorCount];
    //    readIRSensor(IRValue);
}