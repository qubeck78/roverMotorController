#include "comm.h"

extern Servo steeringServo;
extern volatile char reportStepperStop;

//0 - no power
//1 - battery low
//2 - battery ok
extern int powerStatus;

int commInit()
{
  Serial.begin(115200);
  Serial.setTimeout(500);
  

  while(Serial.available())
  {
    Serial.read();
  }
  
}

int commMain()
{
  String rxCommand,rxParameter;
  int i,j;
  
  if (Serial.available())
  {
    rxCommand=Serial.readStringUntil(10);    
  }else{
    if (reportStepperStop)
    {
      reportStepperStop=0;
      Serial.print("STOP\n");
    }
  }

  if (rxCommand==NULL) return 1;
  if (rxCommand=="") return 1;

  if ((rxCommand[0]=='S')&&(rxCommand.length()>=3))
  {
      if (powerStatus==1)
      {
        Serial.print("LOWBATT\n");
        return 1;
      }
      if (powerStatus==0)
      {
        Serial.print("NOPOWER\n");
        return 1;
      }
      

           
      rxParameter="";
      rxParameter+=rxCommand[1];
      rxParameter+=rxCommand[2];
      rxParameter+=rxCommand[3];

      i=rxParameter.toInt();

      if ((i>=60)&&(i<=120))
      {
        steeringServo.write(i);  //servo horn angle correction
        Serial.print("OK\n");
        return 0;
      }
    
  }else if((rxCommand[0]=='D')&&(rxCommand.length()>=3))
  {
      if (powerStatus==1)
      {
        Serial.print("LOWBATT\n");
        return 1;
      }
      if (powerStatus==0)
      {
        Serial.print("NOPOWER\n");
        return 1;
      }
      

    
    i=0;
    
    if (rxCommand[1]=='+')
    {
      i=1;
    }
    if(rxCommand[1]=='-')
    {
      i=-1;
    }

    if (i!=0)
    {
      rxParameter=rxCommand.substring(2);
      j=rxParameter.toInt();

      //376.8 - 1 rotation = 3200steps

      //1mm = 8,4925690021231422505307855626327steps

      //geared down to 1/4
      
      steppersGo(i,j*33.970276008492569002123142250531);

      Serial.print("OK\n");
      return 0;
    }
  }else if (rxCommand[0]=='!')
  {
    emergencyStop();
    return 0;
  }else if (rxCommand[0]=='H')
  {
    steppersStop();
    return 0;
  }else if (rxCommand[0]=='B')
  {
    Serial.print(analogRead(A13));
    Serial.print("\n");
    return 0;
  }

  Serial.print("ERROR\n");
  return 0;
}

