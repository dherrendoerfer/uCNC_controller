/*
 * This file is part of uCNC_controller.
 *
 * Copyright (C) 2014  D.Herrendoerfer
 *
 *   uCNC_controller is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   uCNC_controller is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with uCNC_controller.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Servo.h>
#include "uCNC.h"

/* Version of this progam */
float uCNC_Version = 1.99;

/* Development functions - broken code*/
//#define BUILTIN 1
//#define BROKEN 1

/* If you are trying to run this on a 16k Arduino
   comment out this, you will loose some functions
   but it will fit with max 64k step positions */
#define MEM32K 1

/* Define this if you have composite axis */
//#define COMPLEX_MOVE 1

/* Define this if you want the machine to perform
   a reset with the first G28 command, else, the 
   first set_home will initialize it.*/
#define DO_RESET 1

/* Conversion factor of steps per millimeter 
   use the slack/backlash variable, if you have
   gear or drive issues.
   The RampMax value is added on top of the step
   freqency, which is the start step rate in HZ.
   The ramp rise is set with the Ramp value */
float stepsPerMillimeter_X = -10000L / 49.5; 
int   stepIssueFrequency_X  = 400;
int   stepIssueFreqRamp_X = 4;
int   stepIssueFrequencyRampMax_X  = 220;
int   stepDriveSlack_X = 60;

float stepsPerMillimeter_Y = 10000L / 49.5;
int   stepIssueFrequency_Y  = 400;
int   stepIssueFreqRamp_Y = 4;
int   stepIssueFrequencyRampMax_Y  = 220;
int   stepDriveSlack_Y = 60;

float stepsPerMillimeter_Z = 10;
int   stepIssueFrequency_Z  = 1000;
int   stepIssueFreqRamp_Z = 3;
int   stepIssueFrequencyRampMax_Z  = 150;
int   stepDriveSlack_Z = 0;

/* Unit conversion factor */
float conversionFactor = 1;  // 1 for mm 25.4 for inches

/* Stepper library initialization */
Stepper myStepper1(8,10,9,11);
Stepper myStepper2(4,6,5,7);            
Stepper myStepper3(18,19,17,16);

/* Servo functions and limits */
Servo myServo;

int servoPosMax=83;
int servoPosMin=70;
int servoToolInc=10;
float servoPosZfactor=1.0;

/* Mode selector for the motors (see documentation) */
int   motorMode = 1;

/* X,Y,Z in absolute steps position */
posval_t X = 0;
posval_t Y = 0;
posval_t Z = 0;

/* Set to false to have the first G28(home) reset the axis */
boolean pos_known = false;

/* Set to true if there is are end-switches on A0 */
boolean have_endswitch = false;

/* reset travel distances in steps */
#define RESET_TRAVEL_X 15000
#define RESET_TRAVEL_Y 15000
#define RESET_TRAVEL_Z 1500
#define RESET_TRAVEL_STEP_DELAY 1 
#define RESET_PRELOAD_X 200
#define RESET_PRELOAD_Y 200
#define RESET_PRELOAD_Z 0

/* X,Y,Z in measurement value*/
float posX = 0.0;
float posY = 0.0;
float posZ = 0.0;

/* Tools and Feeds and Coolants */
int tool     = 0;
int spindle  = 0;
int coolant1 = 0;
int coolant2 = 0;

/* Spindle speed (M3 parameter)*/
int spindleSpeed = 0;

int led = 13;

#define COMMAND_SIZE 128
uint8_t command_line[COMMAND_SIZE];
uint8_t sin_count=0;
uint16_t no_data = 0;
uint8_t asleep = 0;

void setup() {
  Serial.begin(9600);
  
  // LED (Laser output)
  pinMode(led, OUTPUT);
  // General pupose (coolant 1) output
  pinMode(2, OUTPUT);
  // General pupose (coolant 2) output
  pinMode(3, OUTPUT);
  
  /* Init the steppers and servo */
  initMotors();
}

void clear_command_string() {
  for (int i=0; i<COMMAND_SIZE; i++) 
    command_line[i] = 0;
  sin_count = 0;
}

void loop() {
  uint8_t c;
  
  Serial.println("uCNC_controller (compatible: Grbl v0.81, Repetier 0.80, E3D v1.00)");
  Serial.println("ready");

  while (true) {
    //read in characters if we got them.
    if (Serial.available() > 0)   {
      c = (uint8_t)Serial.read();
      no_data = 0;
      asleep = 0;
      command_line[sin_count++] = c;
    }
    else {
      no_data++;
      delayMicroseconds(150);
    }
  
    if (sin_count && (c == '\n' || no_data > 100)) {
      command_line[sin_count] = 0;
      process_command(command_line);
      clear_command_string(); 
    }
  
    if (no_data == 60000)  {
      if (!asleep) {
        powerdown();
        asleep=1;
      }
    }
  }
}

