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
#include <Stepper.h>

/* Version of this progam */
float uCNC_Version = 1.0;

/* Development functions - broken code*/
//#define BUILTIN 1
//#define BROKEN 1

/* Conversion factor of steps per millimeter */
float stepsPerMillimeter_X = 200 / 17.1;
float stepsPerMillimeter_Y = -200 / 17.1;
float stepsPerMillimeter_Z = 200 / 17.1;

/* Unit conversion factor */
float conversionFactor = 1;  // 1 for mm 25.4 for inches

/* Stepper library initialization */
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
Stepper myStepper1(stepsPerRevolution, 8,9,10,11);
Stepper myStepper2(stepsPerRevolution, 4,5,6,7);            
Stepper myStepper3(stepsPerRevolution, 17,16,19,18);

/* Servo functions and limits */
Servo myServo;

int servoPosMax=83;
int servoPosMin=70;
int servoToolInc=10;
float servoPosZfactor=1.0;

/* Mode selector for the motors (see documentation) */
int   motorMode = 0;

/* X,Y,Z in absolute steps position */
int X = 0;
int Y = 0;
int Z = 0;

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
  Serial.println("uCNC_controller (compatible: Grbl v0.81, Repetier 0.80, E3D v1.00)");
  
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
      //sin_count=0; 
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

