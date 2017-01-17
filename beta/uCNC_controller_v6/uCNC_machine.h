/*
 * This file is part of uCNC_controller.
 *
 * Copyright (C) 2016,2017  D.Herrendoerfer
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

/* Version of the controller board in use.*/
  //#define V1_BOARD 1
#define V4_BOARD 1 
  //#define GRBL_BOARD 1


/* Mode selector for the motors (see documentation) */
int   motorMode = 0;

/* Conversion factor of steps/millimeter, speed, and backlash */
float stepNumPerMillimeter_X = 1600 / 5;
int   stepIssueFrequency_X  = 1600;
int   stepIssueStartFreq_X  = 400;
int   stepDriveSlack_X = 0;

float stepNumPerMillimeter_Y = 1600 / 5;
int   stepIssueFrequency_Y  = 1600;
int   stepIssueStartFreq_Y  = 400;
int   stepDriveSlack_Y = 0;

float stepNumPerMillimeter_Z = 1600 / 5 ;
int   stepIssueFrequency_Z  = 1600;
int   stepIssueStartFreq_Z  = 400;
int   stepDriveSlack_Z = 0;

/* Unit conversion factor */
float conversionFactor = 1;  // 1 for mm 25.4 for inches

/* Board definitions */

#ifdef V4_BOARD
Stepper mStepperX(10,11,7);
Stepper mStepperY(8,9,7);            
Stepper mStepperZ(2,4,7);            
//Stepper myStepperZ(18,16,17,19,0);

/* General purpose outputs */
#define LED_PIN    13   //LED/LASER output
#define GP1_PIN     6   //General pupose (coolant 1) output
#define GP2_PIN     5   //General pupose (coolant 2) output
#define GP3_PIN     3   //General pupose
#define SERVO_PIN  12   //Servo output
#define ENDSW_PIN  A0   //Start/End Switch input
#endif

#ifdef V1_BOARD
Stepper mStepperX(8,10,9,11,0);
Stepper mStepperY(4,6,5,7,0);            
//Stepper myStepperZ(18,19,17,16,0);
Stepper mStepperZ(17,19,16,18,0); //Hack for my broken board

/* General purpose outputs */
#define LED_PIN    13   //LED/LASER output
#define GP1_PIN     2   //General pupose (coolant 1) output
#define GP2_PIN     3   //General pupose (coolant 2) output
#define GP3_PIN    -1   //General pupose
#define SERVO_PIN  12   //Servo output
#define ENDSW_PIN  A0   //Start/End Switch input
#endif

#ifdef GRBL_BOARD
Stepper mStepperX(5,2,8);
Stepper mStepperY(6,3,8);            
Stepper mStepperZ(7,4,8);            

/* General purpose outputs */
#define LED_PIN    12   //LED/LASER output
#define DIR_PIN    13   //LED/LASER output
#define GP1_PIN    A3   //General pupose (coolant 1) output
#define GP2_PIN    A4   //General pupose (coolant 2) output
#define GP3_PIN    A2   //General pupose
#define SERVO_PIN  A5   //Servo output
#define ENDSW_PIN  A0   //Start/End Switch input
#endif

