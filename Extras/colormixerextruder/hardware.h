/*
 * This file is part of colormixextruder.
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
 *   along with colormixextruder.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef EDUARD_BOARD
/* uControl board V1 'EDUARD' */
/* Definitions of IO Pins and Stepper/Servo/Laser Config */
#define A4988_A_ENA_PIN 10
#define A4988_A_STP_PIN 9
#define A4988_A_DIR_PIN 8

#define A4988_B_ENA_PIN 7
#define A4988_B_STP_PIN 6
#define A4988_B_DIR_PIN 5

#define A4988_C_ENA_PIN 4
#define A4988_C_STP_PIN 3
#define A4988_C_DIR_PIN 2

/* General purpose outputs */
#define LED        13
#define LED_PIN    11   //LED/LASER output
#endif
