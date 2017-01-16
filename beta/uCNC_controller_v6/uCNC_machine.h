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

/* Conversion factor of steps per millimeter, speed, and backlash */
float stepNumPerMillimeter_X = 1600 / 5;
int   stepIssueFrequency_X  = 1600;
int   stepDriveSlack_X = 0;

float stepNumPerMillimeter_Y = 1600 / 5;
int   stepIssueFrequency_Y  = 1600;
int   stepDriveSlack_Y = 0;

float stepNumPerMillimeter_Z = 1600 / 5 ;
int   stepIssueFrequency_Z  = 1600;
int   stepDriveSlack_Z = 0;

/* Unit conversion factor */
float conversionFactor = 1;  // 1 for mm 25.4 for inches

