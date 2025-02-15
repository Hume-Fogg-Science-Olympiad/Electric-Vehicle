#include <avr/wdt.h>
#include "DeviceDriverSet_xxx0.h"
#include "ApplicationFunctionSet_xxx0.cpp"

DeviceDriverSet_Motor AppMotor;
Application_xxx Application_ConquerorCarxxx0;
MPU6050_getdata AppMPU6050getdata;
int time1 = 0;
int time2 = 0;
ConquerorCarMotionControl status = Forward;

int distance = 850; //IN CENTIMETERS


int stepNum = 0;
// Constant for steps in disk
float stepcount = 20.00;  // 20 Slots in disk, change if different

// Constant for wheel diameter
float wheeldiameter = 66.50; // Wheel diameter in millimeters, change if different

//Optical Interruptor Pins
byte MOTOR_FL = 18;
byte MOTOR_FR = 19;

// Integers for pulse counters
int counter_FL = 0;
int counter_FR = 0;

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

  stepNum = CMtoSteps(distance);
  
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

void loop() {
  while (counter_FR < stepNum) {

    Serial.println(counter_FR);
    ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 225 /*speed*/); 
  }
  
  ApplicationFunctionSet_ConquerorCarMotionControl(status /*direction*/, 0 /*speed*/); 
}
