 *
 * This file is part of uCNC_controller.
 *
 * Copyright (C) 2014,2015,2016  D.Herrendoerfer
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
 *

 ********************** BETA version ALERT !!!!!! **************************
THIS IS BETA CODE, IT MAY BE INCOMPLETE OR BROKEN. YOU HAVE BEEN WARNED.

THIS IS for the V4 controller board or compatibles.

uCNC_controller Readme file:
============================

uCNC_controller is a g-code interpreting controller app that is intended
for use on the Arduino platform. It interprets a number of g-code 
commands and controls a number of stepper motors (3), some 
digital outputs (2), and number of servos (1) accordingly.
It is intended for use in home or educational projects, and as the 
license states: it comes with no warranty whatsoever. If you use it
and it breaks in your hands, you get to keep all the pieces.


Default configuration:
======================

  Connector   IO-Ports on Arduino nano w. ATMEGA328P
------------>--------------------------------------------
  Stepper 1:  7,10,11 for Polulu A4988 driver
  Stepper 2:  7,8,9            "
  Stepper 3:  2,4,7            "
  Stepper 4:  A2,A3,A4,A5 (digital out 16,17,18,19)               
  12V out 1:  3
  12V out 2:  5
  12V out 3:  6
  Laser out:  13
  Servo out:  12
  Unused   :  A0,A1,A6,A7 (A6 and A7 cannot be used as output) 

Note, that the controller now supports more than one connection
  scheme, as there are several boards available that can drive
  the steppers and ports. For example the GRBL shield is supported
  natively. Work is in progress to support the RAMPS v1.4 shields
  as well.

System Reset and Initialization routine
---------------------------------------
  Options are:
  1. Upon reset or power up the controller attempts to move all
     axis into their minimal position by traversing the configured
     distance into a solid zero position block - builds should be
     designed to cope with this.
  2. Reset to zero switch, whereas a single input may be used for
     all reset switches. During homing the firmware first tries to
     clear the home condition by moving the axis away from the 
     switch.
  3. Do nothing, and let the operator zero to the appropriate 
     position 

Device-specific values and devices
==================================

Control ports:
--------------
 - Liquid coolant (M7,M8,M9) controls 12VO port 1
 - Vacuum (M10,M11) controls 12VO port 2
 - Spindle(M3,M4,M5) controls Laser and Servo ports (mode dependent)
 - Tool (TXX) controls Servo (mode dependent)
 - Z value controls Laser (on when Z negative - mode dependent)

Device-specific G-Code commands:
================================
  NOTE: All of these settings can be made in the source code of the
        controller application, and should in fact be made there.
        The possibility to change them is intended to make it possible
        to create a controller module that can be shared between 
        several installations, and that is configured by the init
        sequence of its controlling host.

M150: Set Z-Axis mode
---------------------
Defines the mode how the Steppers, the servo and the laser driver are
used by the controller and which codes modify their states
  Mode 0: Stepper 1 is X
          Stepper 2 is Y
          Stepper 3 is Z
          Servo is spindle speed
          Laser is driven by spindle
          
  Mode 1: Stepper 1 is X
          Stepper 2 is Y
          Stepper 3 is Y
          Servo     is Z (down for Z<0)
          Laser     is Z (on for Z<0)

  Mode 2: Stepper 1 is X
          Stepper 2 is Y
          Stepper 3 is Y
          Servo     is Z (+90 to -90)
          Laser     is driven by spindle
          
  Mode 3: Stepper 1 is X
          Stepper 2 is Y
          Stepper 3 is Y
          Servo     is tool
          Laser     is Z (on for Z<0)
          
  Sample:
    M150 S2   ;Sets mode 2
          
M151: Set servo upper position
------------------------------
Sets the maximum value to send to the servo. Range is 0 to 180.

  Sample:
    M151 S170   ;Sets the maximum position to 170

M152: Set servo lower position
------------------------------
Sets the minimum value to send to the servo. Range is 0 to 180.

  Sample:
    M152 S10   ;Sets the minimum position to 10

M153: Set servo tool increment
------------------------------
Sets the increment value to send to the servo depending on the selected tool.

  Sample:
    M153 S10   ;Sets an servo increment of 10 for each selected tool.

M154: Set servo Z factor
------------------------
Sets the ratio between the Z value and the servo position in degrees.

  Note: The Z value at factor 1 is -89 to +90, this also honors the minimum
        and max values set to the servo, so set them to 0 and 180 for full 
        stroke. The sign of this value determines the direction of the servo.

  Sample:
    M154 S0.1  ;Sets ratio to 0.1


M160: Set X-Axis step to mm ratio
---------------------------------
Sets the calibration value of the X-Axis in relation to the steps needed to
travel 1 milimeter. The sign of this value determines the direction of the 
stepper.

  Sample:
    M160 S17.1  ;Sets the ratio of the X-Axis to 17.1 steps per milimeter

M161: Set Y-Axis step to mm ratio
---------------------------------
Sets the calibration value of the Y-Axis in relation to the steps needed to
travel 1 milimeter. The sign of this value determines the direction of the 
stepper.

  Sample:
    M161 S17.1  ;Sets the ratio of the Y-Axis to 17.1 steps per milimeter

M162: Set Z-Axis step to mm ratio
---------------------------------
Sets the calibration value of the Z-Axis in relation to the steps needed to
travel 1 milimeter. The sign of this value determines the direction of the 
stepper.

  Sample:
    M162 S17.1  ;Sets the ratio of the X-Axis to 17.1 steps per milimeter
