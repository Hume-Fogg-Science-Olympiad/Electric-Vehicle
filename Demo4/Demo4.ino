#include <avr/wdt.h>
#include "DeviceDriverSet_xxx0.h"
#include "ApplicationFunctionSet_xxx0.cpp"

DeviceDriverSet_Motor AppMotor;
Application_xxx Application_ConquerorCarxxx0;
MPU6050_getdata AppMPU6050getdata;
int time1 = 0;
int time2 = 0;
ConquerorCarMotionControl status = Forward;

int distance = 100; //IN CENTIMETERS
int height = 10; //IN CENTIMETERS
int targetTime =  20; //IN SECONDS

// Constant for steps in disk
float stepcount = 20.00;  // 20 Slots in disk, change if different

// Constant for wheel diameter
float wheeldiameter = 50.8; // Wheel diameter in millimeters, change if different

//Optical Interruptor Pins
byte MOTOR_FL = 18;
byte MOTOR_FR = 19;

// Integers for pulse counters
int counter_FL = 0;
int counter_FR = 0;

int counter = 0;

unsigned long start = 0;
unsigned long end = 0;
unsigned long currentTime = millis();
long turnTime = 0;

double delayTime = 0;

bool delayBool = false;
bool firstTime = false;

// Interrupt Service Routines

void ISR_countFL()  
{
  counter_FL++;
} 

void ISR_countFR()  
{
  counter_FR++;
}

void setup() {
  Serial.begin(9600);
  AppMotor.DeviceDriverSet_Motor_Init();
  AppMPU6050getdata.MPU6050_dveInit();
  delay(2000);
  AppMPU6050getdata.MPU6050_calibration();

  // Attach the Interrupts to their ISR's
  attachInterrupt(digitalPinToInterrupt (MOTOR_FR), ISR_countFR, RISING);
  attachInterrupt(digitalPinToInterrupt (MOTOR_FL), ISR_countFL, RISING);
  
  counter_FL = 0;
  counter_FR = 0;
}

int CMtoSteps(float cm) {
  int result;  // Final calculation result
  float circumference = (wheeldiameter * 3.14) / 10; // Calculate wheel circumference in cm
  float cm_step = circumference / stepcount;  // CM per Step
  
  float f_result = cm / cm_step;  // Calculate result as a float
  result = (int) f_result; // Convert to an integer (note this is NOT rounded)

  return result;  // End and return result
}

void turn(int degree) {
  AppMPU6050getdata.MPU6050_dveGetEulerAngles(&Yaw);

  int desiredYaw = degree;

  bool turnDirection = Yaw < desiredYaw;

  double m_kP = 0.25;
  int lowerBound = 75;
  int upperBound = 150;

  //abs(Yaw - desiredYaw) > 0.3
  while (abs(Yaw - desiredYaw) > 0.3) {
    Serial.println(Yaw);
    int speed = lowerBound + abs((Yaw - desiredYaw) / m_kP);

    if (speed < lowerBound) {
      speed = lowerBound;
    } else if (speed > upperBound) {
      speed = upperBound;
    }

    turnDirection = Yaw < desiredYaw;

    if (turnDirection) { //Right
      AppMotor.DeviceDriverSet_Motor_control(/*direction_A*/ direction_back, /*speed_A*/ speed,
                                             /*direction_B*/ direction_just, /*speed_B*/ speed, /*controlED*/ control_enable); //Motor control
    } else if (!turnDirection) { //Left
      AppMotor.DeviceDriverSet_Motor_control(/*direction_A*/ direction_just, /*speed_A*/ speed,
                                             /*direction_B*/ direction_back, /*speed_B*/ speed, /*controlED*/ control_enable); //Motor control
    }
    AppMPU6050getdata.MPU6050_dveGetEulerAngles(&Yaw);
  }

  AppMotor.DeviceDriverSet_Motor_control(/*direction_A*/ direction_void, /*speed_A*/ 0,
                                         /*direction_B*/ direction_void, /*speed_B*/ 0, /*controlED*/ control_enable); //Motor control

  counter_FL = 0;
  counter_FR = 0;
}

void loop() {
  int stepNum = 0;

  if (!delayBool) {
    switch (counter) {
      case 0:
        start = millis();
        turn(45);
        end = millis();

        turnTime = end - start;

        counter++;
        break;
      case 1:
        if (!firstTime) {
          firstTime = true;
          start = millis();
        }

        stepNum = CMtoSteps((int) (height * sqrt(2)));
        ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 225 /*speed*/);
        if (counter_FR >= stepNum && counter_FL >= stepNum) {
          end = millis();
          long deltaTime = end - start;
          long speed = ((long) height * sqrt(2)) / deltaTime;
          long timeElapsed = (((distance - 2 * height) + 2 * ((long) height * sqrt(2))) / speed) + (3 * turnTime);
          delayTime = (targetTime * 1000 - timeElapsed) / 4;

          if (delayTime < 0) delayTime = 0;
          else if (delayTime > 3000) delayTime = 2500;

          currentTime = millis();
          delayBool = true;

          counter++;
          ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 0 /*speed*/); 
          counter_FL = 0;
          counter_FR = 0;
        }
        break;
      case 2:
        turn(0);
        counter++;

        currentTime = millis();
        delayBool = true;
        break;
      case 3:
        stepNum = CMtoSteps(distance - 2 * height);
        ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 225 /*speed*/);
        if (counter_FR >= stepNum && counter_FL >= stepNum) {
          counter++;
          ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 0 /*speed*/); 
          counter_FL = 0;
          counter_FR = 0;

          currentTime = millis();
          delayBool = true;
        }
        break;
      case 4:
        turn(-45);
        counter++;
        
        currentTime = millis();
        delayBool = true;
        break;
      case 5:
        stepNum = CMtoSteps((int) (height * sqrt(2)));
        ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 150 /*speed*/);
        if (counter_FR >= stepNum && counter_FL >= stepNum) {
          counter++;
          ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 0 /*speed*/); 
          counter_FL = 0;
          counter_FR = 0;
        }
        break;
      default:
        ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 0 /*speed*/); 
        Serial.println("finished");
        break;
    }
  } else if (abs(millis() - currentTime) > delayTime) {
    delayBool = false;
  }
}
