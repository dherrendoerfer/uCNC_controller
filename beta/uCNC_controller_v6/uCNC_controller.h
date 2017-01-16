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

/* Typedef for the data type for absolute positions 
    depending on the size of the target surface this
    may need to be changed from int to long */
typedef long posval_t;

/* X,Y,Z in absolute steps position */
volatile posval_t X = 0;
volatile posval_t Y = 0;
volatile posval_t Z = 0;

/* Set to false to have the first G28(home) reset the axis */
bool pos_known = false;

/* Set to true if there is are end-switches on A0 */
bool have_endswitch = false;

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

volatile int timer_idle = 1;


int sgn(posval_t value)
{
  return value < 0 ? -1 : 1;
}

int tristate(posval_t value)
{
  if (value == 0)
    return 0;
  return value < 0 ? -1 : 1;
}

