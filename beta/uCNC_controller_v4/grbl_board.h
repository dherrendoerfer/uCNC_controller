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

#define GRBL_BOARD 1

Stepper myStepperX(5,2,8);
Stepper myStepperY(6,3,8);            
Stepper myStepperZ(7,4,8);            

/* General purpose outputs */
#define LED_PIN    12   //LED/LASER output
#define DIR_PIN    13   //LED/LASER output
#define GP1_PIN    A3   //General pupose (coolant 1) output
#define GP2_PIN    A4   //General pupose (coolant 2) output
#define GP3_PIN    A2   //General pupose
#define SERVO_PIN  A5   //Servo output
#define ENDSW_PIN  A6   //Start/End Switch input

