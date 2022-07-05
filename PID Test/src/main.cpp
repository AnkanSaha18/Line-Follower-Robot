#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <QTRSensors.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//-----------------for PID control--------------
float P, I, D;            //60
float kp = 3.5;            //3.5
float kd = 2.0;           //2.0
float ki = 0.5;           //0.5
float maxError = 3.5;//3.5
float PID_delay = 0;

int lastLine = 0;
float prevError = 0;

//-----------Motor---------------

// for motor pin configeration
int eLeft = 5;
int eRight = 10;
int pLeft = 7;
int nLeft = 6;
int pRight = 8;
int nRight = 9;

#define leftMotroBaseSpeed 40
#define rightMotroBaseSpeed 40
#define maxMotorSpeed 180

#define leftMotorCoFector 1.20
#define rightMotorCoFector 1.20

//------------------Display----------------

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//---------------------IR sensor-----------------

// for QTRSensor read
QTRSensors qtr;
const uint8_t number_of_IR = 10;
uint16_t sensorValues[number_of_IR];

// thrational value declaretion
int threationalValue[10] = {90, 80, 80, 80, 80, 80, 80, 80, 80, 90};

void wheel(int left, int right)
{
  // display.clearDisplay();
  display.setCursor(0, 16);
  display.setTextSize(2);
  display.setTextColor(1);

  Serial.print("left ::  ");
  Serial.print(left);
  Serial.print("            right ::  ");
  Serial.println(right);

  display.print(right);
  display.print("    ");
  display.println(left);
  display.display();

  left > maxMotorSpeed ? left = maxMotorSpeed : 1;
  right > maxMotorSpeed ? right = maxMotorSpeed : 1;

  left < -maxMotorSpeed ? left = -maxMotorSpeed : 1;
  right < -maxMotorSpeed ? right = -maxMotorSpeed : 1;

  left = left * leftMotorCoFector;
  right = right * rightMotorCoFector;
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

void readIRSensor(int IRValue[], int callibreat = 0)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(1);
  display.setCursor(0, 0);

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
    // callibration  parpous
    if (callibreat)
    {
      IRValue[i] = a;
    }
    Serial.print("   ");
  }
  for (int i = number_of_IR - 1; i >= 0; i--)
  {
    display.print(IRValue[i]);
  }
  Serial.println();
  display.println();
  display.display();
}

void callibration(int color = 1, int callibrationTime = 5) // black=1 and white=0;
{
  wheel(0, 0);
  int IRValue[number_of_IR];

  int requiredValue[number_of_IR];
  for (int i = 0; i < number_of_IR; i++)
  {
    requiredValue[i] = 2500;
  }

  color == 1 ? wheel(150, -150) : wheel(-150, 150);

  long long presentTime = millis();
  while (millis() - presentTime < callibrationTime * 1000)
  {
    display.clearDisplay();
    readIRSensor(IRValue, 1);
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.setTextColor(1);
    for (int i = 0; i < number_of_IR; i++)
    {
      if (color == 1)
      {
        if (IRValue[i] < requiredValue[i] && IRValue[i] > 70)
        {
          requiredValue[i] = IRValue[i];
        }
      }
      else
      {
        if (IRValue[i] < requiredValue[i])
        {
          requiredValue[i] = IRValue[i];
        }
      }
    }
    for (int i = 0; i < number_of_IR; i++)
    {
      display.print(requiredValue[i]);
      display.print("    ");
      display.println(requiredValue[++i]);
    }
    display.display();
    delay(50);
  }
  wheel(0, 0);
  display.clearDisplay();
  display.setCursor(0, 16);
  display.setTextSize(1);
  color == 1 ? display.println("Black") : display.println("White");
  for (int i = 0; i < number_of_IR; i++)
  {
    display.print(requiredValue[i]);
    display.print("    ");
    display.println(requiredValue[++i]);
  }
  display.display();
  return;
}

void nodeCheak(int *);

float errorCalculation()
{
  int IRValue[number_of_IR];
  double total = 0;
  double weightValue = 0;
  double error = 0;

  readIRSensor(IRValue);
  for (int i = 0; i < number_of_IR; i++)
  {
    total += IRValue[i];
    weightValue += IRValue[i] * (i + 1);
  }
  // if (total >= 6)
  // {
  //   nodeCheak(IRValue);
  //   //    return errorCalculation();
  //   return 0;
  // }

  if (total == 0)
  {
    return 0;
  }

  double desiredPosition = 5.5;
  double linePosotion;

  linePosotion = weightValue / total;
  error = linePosotion - desiredPosition;
  return error;
}

void wait()
{
  int IRValue[number_of_IR];
  readIRSensor(IRValue);
  Serial.println("wait in");
  while (IRValue[2] + IRValue[3] + IRValue[4] + IRValue[5] + IRValue[6] + IRValue[7] < 2)
  {
    readIRSensor(IRValue);
    Serial.println("waitting");
  }
  Serial.print("wait out");
  return;
}

void lineFollow()
{
  double error, delSpeed;

  error = errorCalculation();
  display.setCursor(35, 32);
  display.println(error);
  display.display();


  float P, D, I;
  P = error;
  D = error - prevError;
  I = I + error;

  delSpeed = kp * P + kd * D + ki * I;
  wheel(leftMotroBaseSpeed + delSpeed, rightMotroBaseSpeed - delSpeed);
  prevError = error;

  //  display.setCursor(35, 32);
  //  display.println(error);
  //  display.display();
  Serial.print("error is  ::  ");
  Serial.println(error);
  Serial.print("delspeed is :: ");
  Serial.println(delSpeed);
  delay(PID_delay);

  if (error >= maxError || error <= -maxError)
  {
    wait();
    return;
  }
}

void sideLineCheck(int lines[])
{
  lines[0] = 0;
  lines[1] = 0;
  lines[2] = 0;
  int ti = 140; ///(how much time will go, after detecting any node)..........changeable.........
  // wheel(0, 0);
  wheel(leftMotroBaseSpeed, rightMotroBaseSpeed);
  int lineCrossDelay = 30;
  delay(lineCrossDelay);
  long long past = millis();
  while (millis() - past < ti)
  {
    int IRValue[10];
    readIRSensor(IRValue);

    if (IRValue[4] + IRValue[5] >= 1)
    {
      lines[1] = 1;
      // delay(30);
    }

    if (IRValue[0] + IRValue[1] + IRValue[2] >= 2 && IRValue[7] + IRValue[8] + IRValue[9] >= 2)
    {
      lines[0] = 1;
      lines[2] = 1;
      delay(lineCrossDelay + 20);
    }

    else if (IRValue[0] + IRValue[1] + IRValue[2] >= 2)
    {
      lines[0] = 1;
      delay(lineCrossDelay + 20);
    }
    else if (IRValue[7] + IRValue[8] + IRValue[9] >= 2)
    {
      lines[2] = 1;
      delay(lineCrossDelay + 20);
    }
  }
  wheel(0, 0);
  display.clearDisplay();
  display.setCursor(0, 32);
  display.setTextSize(2);
  display.setTextColor(1);
  display.print("Lines ");
  display.print(lines[0]);
  display.print(lines[1]);
  display.println(lines[2]);
  display.display();
  delay(500); /// change it later............
  return;
}

int nodeIdentifire(int IRValue[])
{
  // int IRValue[10];
  // readIRSensor(IRValue);

  // 01~02~03~ serise......
  //                     |
  //  _______        ____|____      .....
  //     |               |

  if (IRValue[0] == 1 && IRValue[1] == 1 && IRValue[4] == 1 && IRValue[5] == 1 && IRValue[8] == 1 && IRValue[9] == 1)
  {
    int lines[3];
    sideLineCheck(lines);
    if (lines[1] == 1)
      return 2;
    else
      return 1;
  }

  // 11~12~13~ serise......
  //                    |
  //  ____          ____|      .....
  //     |              |
  if (IRValue[0] == 1 && IRValue[1] == 1 && IRValue[4] == 1 && IRValue[5] == 1)
  {
    int lines[3];
    sideLineCheck(lines);
    if (lines[1] == 1)
      return 12;
    else
      return 11;
  }

  // 21~22~23~ serise......
  //              |
  //  ____        |____      .....
  //  |           |
  if (IRValue[4] == 1 && IRValue[5] == 1 && IRValue[8] == 1 && IRValue[9] == 1)
  {
    int lines[3];
    sideLineCheck(lines);
    if (lines[1] == 1)
      return 22;
    else
      return 21;
  }
  return 0;
}

void nodeCheak(int IRValue[])
{
  int whichNode = nodeIdentifire(IRValue);

  Serial.print("\n\n\n bal bal bal bal which node  ::: ");
  Serial.println(whichNode);
  Serial.println("\n\n");

  int lineout = 350; //.......temporary......
  switch (whichNode)
  {
  case 11:
    wheel(-(leftMotroBaseSpeed - 5), (rightMotroBaseSpeed - 5));
    wait();
    break;
  case 21:
    wheel((leftMotroBaseSpeed - 5), -(rightMotroBaseSpeed - 5));
    wait();
    break;

  case 12:
    wheel(-(leftMotroBaseSpeed - 5), (rightMotroBaseSpeed - 5));
    delay(lineout);
    wait();
    break;
  case 22:
    wheel((leftMotroBaseSpeed - 5), -(rightMotroBaseSpeed - 5));
    delay(lineout);
    wait();
    break;

  case 1:
    wheel((leftMotroBaseSpeed - 5), -(rightMotroBaseSpeed - 5));
    //      wheel(leftMotroBaseSpeed, -rightMotroBaseSpeed);
    wait();
    break;
  case 2:
    wheel(-(leftMotroBaseSpeed - 5), (rightMotroBaseSpeed - 5));
    //      wheel(-leftMotroBaseSpeed, rightMotroBaseSpeed);
    delay(lineout);
    wait();
    break;

  default:
    wheel(leftMotroBaseSpeed, rightMotroBaseSpeed);
    break;
  }
  return;
}

void setup()
{
  // ------ For IR pin setup----------
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){
                        A0, A1, A2, A3, A4, A5, A6, A7, A8, A9},
                    number_of_IR);
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

  Serial.begin(9600);
}

void loop()
{
  // callibration(1);
  // delay(7000);
  // callibration(0);
  // delay(7000);

  // nodeCheak();
  lineFollow();
}