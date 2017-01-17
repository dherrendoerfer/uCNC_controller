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

void powerdown()
{
  mStepperX.powerdown();
  mStepperY.powerdown();
  mStepperZ.powerdown();
}

void updateMotorCodes()
{
  digitalWrite(GP1_PIN,(coolant1 == 1) ? HIGH : LOW);
  digitalWrite(GP2_PIN,(coolant2 == 1) ? HIGH : LOW);

#ifdef DIR_PIN
  digitalWrite(DIR_PIN,(spindle > 1) ? HIGH : LOW);
#endif
  
  switch(motorMode) {
  case 0:
//    myServo.write(spindleSpeed);
    digitalWrite(LED_PIN,(spindle > 0) ? HIGH : LOW);
    break;
  case 2:
    digitalWrite(LED_PIN,(spindle > 0) ? HIGH : LOW);
    break;
  }
}

void updateToolCodes()
{
  if (motorMode == 3) {
//    updateServo(servoPosMin + (tool * servoToolInc));
  }
}

/* No direct IO below this line */




/* No direct use of step coordinates below this line */

posval_t convertPosX(float pos)
{ 
  return (posval_t)(pos*conversionFactor*stepNumPerMillimeter_X);
}

posval_t convertPosY(float pos)
{ 
  return (posval_t)(pos*conversionFactor*stepNumPerMillimeter_Y);
}

posval_t convertPosZ(float pos)
{ 
  return (posval_t)(pos*conversionFactor*stepNumPerMillimeter_Z);
}

void linePos(unsigned int lineNo, float x2, float y2, float z2, float feedrate)
{
  float distance;
  // This is very slow.
  distance = sqrt(sq(z2 - posZ) + sq(x2 - posX) + sq(y2 - posY));
  
//  addPlanItem(); 
  
//  lineXYZ(convertPosX(x2),convertPosY(y2),convertPosZ(z2), feedrate);
  posX=x2;
  posY=y2;
  posZ=z2;
}

void jumpPos(float x2, float y2, float z2)
{
//  moveToXYZ(convertPosX(x2),convertPosY(y2),convertPosZ(z2), 0, 0, 0);
  posX=x2;
  posY=y2;
  posZ=z2;
}

void resetPosXYZ()
{
//  resetXYZ();
  posX = 0.0;
  posY = 0.0;
  posZ = 0.0;
  pos_known=true; 
}
