
#include "steppers.h"

unsigned long steppersMainLastMs;

volatile unsigned long stepCount;

volatile long speedCounter;
volatile long speedValue;

#define _MIN_SPEED 30


char currentDir;
char accelerationVector;

volatile char reportStepperStop;

int steppersInit()
{
  
  pinMode(FAN_PIN , OUTPUT);
  pinMode(HEATER_0_PIN , OUTPUT);
  pinMode(HEATER_1_PIN , OUTPUT);

  pinMode(LED_PIN  , OUTPUT);
  
  pinMode(X_STEP_PIN  , OUTPUT);
  pinMode(X_DIR_PIN    , OUTPUT);
  pinMode(X_ENABLE_PIN    , OUTPUT);
  
  pinMode(Y_STEP_PIN  , OUTPUT);
  pinMode(Y_DIR_PIN    , OUTPUT);
  pinMode(Y_ENABLE_PIN    , OUTPUT);
  
  pinMode(Z_STEP_PIN  , OUTPUT);
  pinMode(Z_DIR_PIN    , OUTPUT);
  pinMode(Z_ENABLE_PIN    , OUTPUT);
  
  pinMode(E_STEP_PIN  , OUTPUT);
  pinMode(E_DIR_PIN    , OUTPUT);
  pinMode(E_ENABLE_PIN    , OUTPUT);
  
  pinMode(Q_STEP_PIN  , OUTPUT);
  pinMode(Q_DIR_PIN    , OUTPUT);
  pinMode(Q_ENABLE_PIN    , OUTPUT);
  
  digitalWrite(X_ENABLE_PIN    , LOW);
  digitalWrite(Y_ENABLE_PIN    , LOW);

  cli();  //disable irq

//set timer2 interrupt at 16kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0

  OCR2A = 125;// = (16*10^6) / (16000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);


   stepCount=0;
   speedCounter=0;
   speedValue=20;
   
   currentDir=0;
   accelerationVector=0;

   steppersMainLastMs=0;
  reportStepperStop=0;
    

  sei();  //enable irq
  
  return 0;  
}

ISR(TIMER2_COMPA_vect)
{
  volatile unsigned long j;
  

  if (speedCounter)
  {
    speedCounter--;
    return;
  }


  speedCounter=speedValue;
  
  if (stepCount==1) reportStepperStop=1;
  
  if (stepCount)
  {
    stepCount--; 
  
    digitalWrite(X_STEP_PIN    , HIGH);
    digitalWrite(Y_STEP_PIN    , HIGH);

    for(j=0;j<20;j++);
    
    digitalWrite(X_STEP_PIN    , LOW);
    digitalWrite(Y_STEP_PIN    , LOW);
  }
  
}

int steppersGo(int dir,unsigned long steps)
{

 
  cli();


  if (dir>0)
  {
    digitalWrite(X_DIR_PIN    , HIGH);
    digitalWrite(Y_DIR_PIN    , LOW);
  }
  else {
    digitalWrite(X_DIR_PIN    , LOW);
    digitalWrite(Y_DIR_PIN    , HIGH);
  } 

  currentDir=dir;
  
  stepCount=steps;

  speedCounter=0;
  speedValue=_MIN_SPEED;
  accelerationVector=1;
  reportStepperStop=0;

  digitalWrite(X_ENABLE_PIN    , LOW);
  digitalWrite(Y_ENABLE_PIN    , LOW);
  
  sei();

}

int emergencyStop()
{
  cli();
  
  stepCount=0;
  accelerationVector=0;
  reportStepperStop=1;

  sei(); 
  
  return 0;
}

int steppersStop()
{
  cli();

  if (stepCount>3000)
  {
    stepCount=3000;
  }
  
  sei();

  return 0;
}

int steppersMain()
{
  unsigned long currentMs;
  
  //every 100ms

  currentMs=millis();

  if (currentMs<steppersMainLastMs)
  {
    steppersMainLastMs=0;
  }

  if (!(currentMs-steppersMainLastMs>=100))
  {
    return 0;
  }


  steppersMainLastMs=currentMs;

  cli();
  
  if (stepCount==0)
  {
    digitalWrite(X_ENABLE_PIN    , HIGH);
    digitalWrite(Y_ENABLE_PIN    , HIGH);
    
  }

  if (stepCount<3000)
  {
    accelerationVector=-1;
  }
  
  if (accelerationVector==1)
  {
    if (speedValue>1)
    {
      speedValue--;
    }else{
      //full speed
      accelerationVector=0;
      speedValue=1;
    }
    
  }

  if (accelerationVector==-1)
  {
    if (speedValue<_MIN_SPEED)
    {
      speedValue++;
    }
  }

   
  sei();
  
  return 0;
}

