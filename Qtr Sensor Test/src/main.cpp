#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <QTRSensors.h>



// for QTRSensor read
QTRSensors qtr;
const uint8_t number_of_IR = 10;
uint16_t sensorValues[number_of_IR];

// thrational value declaretion
int threationalValue[10] = {160, 150, 150, 150, 150, 150, 150, 150, 150, 160};



void readIRSensor(bool IRValue[])
{
  qtr.read(sensorValues);
  for (uint8_t i = 0; i < number_of_IR; i++)
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

    Serial.print("\t");
  }
  Serial.println();

}

void setup()
{
  // ------ For IR pin setup----------
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){
                        A0, A1, A2, A3, A4, A5, A6, A7, A8, A9},
                    number_of_IR);
  qtr.setEmitterPin(12);

  Serial.begin(9600);
}

void loop()
{
  bool irValue[10];
  readIRSensor(irValue);
  for (int i = 0; i < number_of_IR; i++)
  {
    Serial.print(irValue[i]);
    Serial.print("\t");
  }
  Serial.println("\n\n");
  delay(1000);
}