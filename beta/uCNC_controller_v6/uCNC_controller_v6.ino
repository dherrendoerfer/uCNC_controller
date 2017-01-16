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

#include "uCNC_controller.h"
#include "uCNC_stepper_lt.h"
#include "uCNC_machine.h"

Stepper mStepperX(8,10,9,11,1);
Stepper mStepperY(4,6,5,7,1);            
Stepper mStepperZ(17,19,16,18,1);

void setup()
{
  timer1_setup();
  
  Serial.begin(9600);

}


void loop()
{ 
  int shutdown = 0;
  unsigned long starttime = 0;
  unsigned long stoptime = 0;
  
  Serial.println("uCNC_controller_v4 (compatible: Grbl v0.81, Repetier 0.80, E3D v1.00)");

  while (!shutdown) {

    Serial.println("test");
    
    plan_line(4096, 0, 0, 0.8);

    starttime = micros();
    if (mStepperX.busy() || mStepperY.busy() || mStepperZ.busy()) {
      mStepperX.tick();
      mStepperY.tick();
      mStepperZ.tick();
    }
    stoptime = micros();

    delay(1);

    while (mStepperX.busy() || mStepperY.busy() || mStepperZ.busy()) {
      mStepperX.tick();
      mStepperY.tick();
      mStepperZ.tick();
      delayMicroseconds(800);
    }

    Serial.print("Time for one slot :");
    Serial.print(stoptime - starttime);
    Serial.println("uS");

    plan_line(0, 4096, 0, 100);

    while (mStepperX.busy() || mStepperY.busy() || mStepperZ.busy()) {
      mStepperX.tick();
      mStepperY.tick();
      mStepperZ.tick();
      delayMicroseconds(800);
    }
    plan_line(0, 0, 4096, 100);

    while (mStepperX.busy() || mStepperY.busy() || mStepperZ.busy()) {
      mStepperX.tick();
      mStepperY.tick();
      mStepperZ.tick();
      delayMicroseconds(800);
    }

    plan_line(1024, 2048, 4096, 100);

    starttime = micros();
    if (mStepperX.busy() || mStepperY.busy() || mStepperZ.busy()) {
      mStepperX.tick();
      mStepperY.tick();
      mStepperZ.tick();
    }
    stoptime = micros();
    
    delay(1);

    while (mStepperX.busy() || mStepperY.busy() || mStepperZ.busy()) {
      mStepperX.tick();
      mStepperY.tick();
      mStepperZ.tick();
      delayMicroseconds(800);
    }

    Serial.print("Time for three slots :");
    Serial.print(stoptime - starttime);
    Serial.println("uS");

    Serial.println("done.");
    
    Serial.print("Using timer :");
    plan_line(0, 0, 0, 3.8);
    start_timer();
    
    while (!timer_idle)
      delay(20);

    Serial.print("done.");
    
    delay(20000);

  }
}

