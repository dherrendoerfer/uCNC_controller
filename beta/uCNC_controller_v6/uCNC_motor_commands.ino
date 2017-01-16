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

#define DRILL_SPEED 0.1
#define DRILL_DEPTH 10 //mm

int posServo;

void initMotors()
{
  /*Set stepper base speed */
  mStepperX.setSpeed(stepIssueFrequency_X);
  mStepperY.setSpeed(stepIssueFrequency_Y);
  mStepperZ.setSpeed(stepIssueFrequency_Z);
  
  /*Set steppers slack (if any)*/
  mStepperX.setSlack(stepDriveSlack_X);
  mStepperY.setSlack(stepDriveSlack_Y);
  mStepperZ.setSlack(stepDriveSlack_Z);
}

void homeXYZ()
{
  int i=0;
  
  if(pos_known) {
//    movePosXYZ (0, 0, 0, 0);
    return;
  }
#ifdef DO_RESET  
  if (have_endswitch) {

    // todo
    
  } else {

    // todo
    
  }
#endif  
  resetPosXYZ();
}

void resetXYZ()
{
  Y = 0;
  X = 0;
  Z = 0;
}

void updateServo(int servoPos)
{
  if (servoPos>servoPosMax)
    servoPos = servoPosMax;
  if (servoPos<servoPosMin)
    servoPos = servoPosMin;
  
  myServo.write(servoPos);
  posServo = servoPos;
}

void servoZ()
{
  int servoPos = ((float)posZ*servoPosZfactor) + 90;
  updateServo(servoPos);
}

int moveZ(posval_t dZ, char *px, char *py, char *pz)
{
  Z = Z + dZ;  

}

void powerdown()
{
  myStepperX.powerdown();
  myStepperY.powerdown();
  myStepperZ.powerdown();
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
    myServo.write(spindleSpeed);
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
    updateServo(servoPosMin + (tool * servoToolInc));
  }
}

/* No direct IO below this line */

void _moveToXYZ(posval_t pX, posval_t pY, posval_t pZ, int accelX, int accelY, int accelZ)
{ 
}


void moveToXYZ(posval_t pX, posval_t pY, posval_t pZ, int accelX, int accelY, int accelZ)
{ 

}
/*
void moveToX(int pX)
{
  moveToXYZ(pX - X, 0, 0, 0, 0, 0);
}

void moveToY(int pY)
{
  moveToXYZ(0, pY - Y, 0, 0, 0, 0);
}

void moveToXY(int pX, int pY)
{
  moveToXYZ(pX - X, pY - Y, 0, 0, 0, 0);
}
*/
#ifdef BROKEN
void drill()
{
  int oZ;
  float oSpeed;
  
  delay(500);
  oZ = Z;
//  oSpeed=motorSpeed;
  
//  setXYSpeed(DRILL_SPEED);
  
  moveToXYZ(X,Y,Z-(DRILL_DEPTH*stepsPerMillimeter_X), 0, 0, 0);
  delay(500);
//  setXYSpeed(oSpeed);
  moveToXYZ(X,Y,oZ, 0, 0, 0);
}
#endif /*BROKEN*/   

void wait(unsigned long steptime)
{
  if (!steptime)
    return;
    
  while (micros() < steptime ) {
    delayMicroseconds(5);
  }
  
  return;
}


/* No direct use of step coordinates below this line */

posval_t convertPosX(float pos)
{ 
  return (posval_t)(pos*conversionFactor*stepsPerMillimeter_X);
}

posval_t convertPosY(float pos)
{ 
  return (posval_t)(pos*conversionFactor*stepsPerMillimeter_Y);
}

posval_t convertPosZ(float pos)
{ 
  return (posval_t)(pos*conversionFactor*stepsPerMillimeter_Z);
}

void linePos(float x2, float y2, float z2, float feedrate)
{
  lineXYZ(convertPosX(x2),convertPosY(y2),convertPosZ(z2), feedrate);
  posX=x2;
  posY=y2;
  posZ=z2;
}

void jumpPos(float x2, float y2, float z2)
{
  moveToXYZ(convertPosX(x2),convertPosY(y2),convertPosZ(z2), 0, 0, 0);
  posX=x2;
  posY=y2;
  posZ=z2;
}

//#ifdef BROKEN
void arcPos(int direction, float x2, float y2, float i, float j)
{
//      case 2://Clockwise arc
//      case 3://Counterclockwise arc
      float angleA, angleB, angle, radius, aX, aY, bX, bY, length, x, y, centerX, centerY;

      // Center coordinates are always relative

      aX = (posX - i);
      aY = (posY - j);
      bX = (x2 - i);
      bY = (y2 - j);
      
      centerX = posX + i;
      centerY = posX + j;

      Serial.println("aX");
      Serial.println(aX);
      Serial.println("aY");
      Serial.println(aY);

      Serial.println("bX");
      Serial.println(bX);
      Serial.println("bY");
      Serial.println(bY);

      if (direction == 0) { // Clockwise
        angleA = atan2(bY, bX);
        angleB = atan2(aY, aX);
      }
      else { // Counterclockwise
        angleA = atan2(aY, aX);
        angleB = atan2(bY, bX);
      }

      Serial.println("AngleA");
      Serial.println(angleA);
      Serial.println("AngleB");
      Serial.println(angleB);

      // Make sure angleB is always greater than angleA
      // and if not add 2PI so that it is (this also takes
      // care of the special case of angleA == angleB,
      // ie we want a complete circle)
      if (angleB <= angleA) 
        angleB += 2 * M_PI;
      angle = angleB - angleA;

      radius = sqrt(aX * aX + aY * aY);

      Serial.println("Radius");
      Serial.println(radius);

      length = radius * angle;

      Serial.println("Length");
      Serial.println(length);

      int steps, s, step;
//      steps = length / ((stepsPerMillimeter_X + stepsPerMillimeter_Y)/4); // approx 0.5mm
      steps = length / 4; // approx 0.5mm

      Serial.println("Steps");
      Serial.println(steps);

      for (s = 1; s <= steps; s++) {
        step = (direction == 1) ? s : steps - s; // Work backwards for CW
        x = centerX + radius * cos(angleA + angle * ((float) step / steps));
        y = centerY + radius * sin(angleA + angle * ((float) step / steps));

        Serial.println("X");
        Serial.println(x);
        Serial.println("Y");
        Serial.println(y);

        // straight line to new points
        //linePos(x,y,posZ);
      }
      
  //linePos(x2,y2,posZ);
}
//#endif /*BROKEN*/

void movePosXYZ (float x2, float y2, float z2, float feedrate )
{
  linePos(x2, y2, z2, feedrate);
}

void jumpPosXYZ (float x2, float y2, float z2)
{
  jumpPos(x2, y2, z2);
}

void resetPosXYZ()
{
  resetXYZ();
  posX = 0.0;
  posY = 0.0;
  posZ = 0.0;
  pos_known=true; 
}
