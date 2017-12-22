/*
 * This file is part of uCNC_controller.
 *
 * Copyright (C) 2017  D.Herrendoerfer
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

#define V4_BOARD 1

Stepper myStepperX(10,11,7);
Stepper myStepperY(8,9,7);            
//Stepper myStepperZ(2,4,7);            
Stepper myStepperZ(18,16,17,19,0);

/* General purpose outputs */
#define LED_PIN    13   //LED/LASER output
#define GP1_PIN     6   //General pupose (coolant 1) output
#define GP2_PIN     5   //General pupose (coolant 2) output
#define GP3_PIN     3   //General pupose
#define SERVO_PIN  12   //Servo output
#define ENDSW_PIN  A0   //Start/End Switch input

