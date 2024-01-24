#include <Servo.h>

//http://reprap.org/wiki/RAMPS_1.4

//Bateria - 570 - max

#include "steppers.h"
#include "comm.h"

#define SERVO_PIN 11

Servo steeringServo;


//0 - no power
//1 - battery low
//2 - battery ok
int powerStatus;


void checkPower()
{
  int batVolts;

  batVolts = analogRead(A13);

  if (batVolts < 100)
  {
    //no power
    powerStatus = 0;
  } else if (batVolts < 445)
  {
    //bat low
    powerStatus = 1;
  } else {
    //bat ok
    powerStatus = 2;
  }
}


void setup()
{
  steppersInit();
  commInit();

  steeringServo.attach(SERVO_PIN);
  steeringServo.write(90);
  checkPower();

}


void loop()
{
  commMain();

  steppersMain();

  if ((millis() % 1000) == 0)
  {
    checkPower();
  }

  switch (powerStatus)
  {
    case 0:
      if ((millis() % 1000) < 50)
      {
        digitalWrite(LED_PIN, HIGH);
      }
      else
      {
        digitalWrite(LED_PIN, LOW);
      }
      break;

    case 1:

      if (millis() % 100 < 50)
      {
        digitalWrite(LED_PIN, HIGH);
      }
      else
      {
        digitalWrite(LED_PIN, LOW);
      }
      break;


    case 2:
      if (millis() % 1000 < 500)
      {
        digitalWrite(LED_PIN, HIGH);
      }
      else
      {
        digitalWrite(LED_PIN, LOW);
      }
      break;
  }
}

