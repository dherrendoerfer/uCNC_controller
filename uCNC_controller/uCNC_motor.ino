
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
 *
 ********************************************************************************
 *
 *  From the documentation: These are the supported modes for the steppers 
 *                           and light/servo drivers
 *                           
 *	M150: Set Z-Axis mode
 *	---------------------
 *	Defines the mode how the Steppers, the servo and the laser driver are
 *	used by the controller and which codes modify their states
 *	  Mode 0: Stepper 1 is X
 *	          Stepper 2 is Y
 *	          Stepper 3 is Z
 *	          Servo is spindle speed
 *
 *	          
 *	  Mode 1: Stepper 1 is X
 *	          Stepper 2 is Y
 *	          Stepper 3 is Y
 *	          Servo     is Z (down for Z<0)
 *	          Laser     is Z (on for Z<0)
 *	
 *	  Mode 2: Stepper 1 is X
 *	          Stepper 2 is Y
 *	          Stepper 3 is Y
 *  	          Servo     is Z (+90 to -90)
 * 	          Laser     is driven by spindle
 *	          
 *	  Mode 3: Stepper 1 is X
 *	          Stepper 2 is Y
 *	          Stepper 3 is Y
 *	          Servo     is tool
 *  	          Laser     is Z (on for Z<0)
 *	          
 *	  Sample:
 *	    M150 S2   ;Sets mode 2
 *
 *
 */

/* IMPORTANT: All direct motor controls are here, everywhere else
              these functions here are used to keep this code portable 
              
              The functions are grouped into those using the native steps
              values or the measurement (float) values. */

#define MAX_SPEED 60
#define DRILL_SPEED 0.1

float motorSpeed = MAX_SPEED;

void initMotors()
{
  myStepper1.setSpeed(MAX_SPEED);
  myStepper2.setSpeed(MAX_SPEED);
  myStepper3.setSpeed(MAX_SPEED);

  myServo.attach(12);
  myServo.write(servoPosMax);

  myStepper1.step(-1400);
  myStepper2.step(-1500);

  myStepper2.step(400);
  myStepper1.step(75);
}

void setXYSpeed(float speed)
{
  // remember
  motorSpeed = speed;

  if (speed == 0 || speed > 1) {
    speed = MAX_SPEED;
  }
  else {
    speed *= MAX_SPEED;
  }
  
  myStepper1.setSpeed((int)speed);
  myStepper2.setSpeed((int)speed);
  myStepper3.setSpeed((int)speed);
  
}

void moveX(int dX)
{
  X = X + dX;
  myStepper1.step(dX);
}

void moveY(int dY)
{
  Y = Y + dY;
  
  switch(motorMode) {
  case 0:
    myStepper2.step(dY);
    break;
  case 1:
  case 2:
  case 3:
    int i;
    for (i=0;i<abs(dY);i++) {
      myStepper3.setSpeed(MAX_SPEED); //stepper3 is always driven with stepper2
      myStepper3.step((dY < 1) ? -1 : 1);
      myStepper2.step((dY < 1) ? -1 : 1);
    }
    break;
  }  
}

void resetXY()
{
  Y = 0;
  X = 0;
}

void penDown()
{
  delay(30);
  digitalWrite(led, HIGH);
  myServo.write(servoPosMin);
  delay(100);
}

void penUp()
{
  digitalWrite(led, LOW);
  myServo.write(servoPosMax);
  delay(100);
}

void updateServo(int servoPos)
{
  if (servoPos>servoPosMax)
    servoPos = servoPosMax;
  if (servoPos<servoPosMin)
    servoPos = servoPosMin;
  
  myServo.write(servoPos);
}

void servoZ()
{
  int servoPos = ((float)posZ*servoPosZfactor) + 90;
  updateServo(servoPos);
}

void moveZ(int dZ)
{
  Z = Z + dZ;  

  switch(motorMode) {
  case 0:
    myStepper3.setSpeed((int)motorSpeed); //stepper3 is always driven with stepper2
    myStepper3.step(dZ);
    break;
  case 1:
    digitalWrite(led,(posZ < 1) ? HIGH : LOW);
    updateServo((posZ < 1) ? servoPosMin : servoPosMax);
    delay(100);
    break;
  case 2:
    updateServo(servoPosMin + (servoPosZfactor*posZ));
    delay(200);
    break;
  case 3:
    digitalWrite(led,(posZ < 1) ? HIGH : LOW);
    break;
  }
}

void powerdown()
{
  // Stepper 1
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  // Stepper 2  
  digitalWrite(8,LOW);
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  // Stepper 3  
  digitalWrite(16,LOW);
  digitalWrite(17,LOW);
  digitalWrite(18,LOW);
  digitalWrite(19,LOW);
}

void updateMotorCodes()
{
  digitalWrite(2,(coolant1 == 1) ? HIGH : LOW);
  digitalWrite(3,(coolant2 == 1) ? HIGH : LOW);
  
  switch(motorMode) {
  case 0:
    myServo.write(spindleSpeed);
    digitalWrite(led,(spindle == 1) ? HIGH : LOW);
    break;
  case 2:
    digitalWrite(led,(spindle == 1) ? HIGH : LOW);
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

void moveToX(int pX)
{
  moveX(pX - X);
}

void moveToY(int pY)
{
  moveY(pY - Y);
}

void moveToXY(int pX, int pY)
{
  moveX(pX - X);
  moveY(pY - Y);
}

void moveToXYZ(int pX, int pY, int pZ)
{
  if (pX - X)
    moveX(pX - X);
  if (pY - Y)
    moveY(pY - Y);
  if (pZ - Z)
    moveZ(pZ - Z);
}

void drill()
{
  int oZ;
  float oSpeed;
  
  delay(500);
  oZ = Z;
  oSpeed=motorSpeed;
  
  setXYSpeed(DRILL_SPEED);
  
  moveToXYZ(X,Y,Z-(10*stepsPerMillimeter_X));
  delay(500);
  setXYSpeed(oSpeed);
  moveToXYZ(X,Y,oZ);
}

int sgn(int value)
{
  return value < 0 ? -1 : value > 0;
}

void lineXYZ(int x2, int y2, int z2)
{
  int n, deltax, deltay, deltaz, sgndeltax, sgndeltay, sgndeltaz, deltaxabs, deltayabs, deltazabs, x, y, z, drawx, drawy, drawz;

  deltax = x2 - X;
  deltay = y2 - Y;
  deltaz = z2 - Z;
  deltaxabs = abs(deltax);
  deltayabs = abs(deltay);
  deltazabs = abs(deltaz);
  sgndeltax = sgn(deltax);
  sgndeltay = sgn(deltay);
  sgndeltaz = sgn(deltaz);
  x = deltayabs >> 1;
  y = deltaxabs >> 1;
  z = deltazabs >> 1;
  drawx = X;
  drawy = Y;
  drawz = Z;
  
  moveToXYZ(drawx, drawy, drawz);
  
  // dX is biggest
  if(deltaxabs >= deltayabs && deltaxabs >= deltazabs){
    for(n = 0; n < deltaxabs; n++){
      y += deltayabs;
      if(y >= deltaxabs){
        y -= deltaxabs;
        drawy += sgndeltay;
      }
      z += deltazabs;
      if(z >= deltaxabs){
        z -= deltaxabs;
        drawz += sgndeltaz;
      }

      drawx += sgndeltax;
      moveToXYZ(drawx, drawy, drawz);
    }
    return;
  }
  // dY is biggest
  if(deltayabs >= deltaxabs && deltayabs >= deltazabs){
    for(n = 0; n < deltayabs; n++){
      x += deltaxabs;
      if(x >= deltayabs){
        x -= deltayabs;
        drawx += sgndeltax;
      }
      z += deltazabs;
      if(z >= deltayabs){
        z -= deltayabs;
        drawz += sgndeltaz;
      }
      drawy += sgndeltay;
      moveToXYZ(drawx, drawy, drawz);
    }
    return;
  }
  // dZ is biggest
  if(deltazabs >= deltaxabs && deltazabs >= deltayabs){
    for(n = 0; n < deltazabs; n++){
      x += deltaxabs;
      if(x >= deltazabs){
        x -= deltazabs;
        drawx += sgndeltax;
      }
      y += deltayabs;
      if(y >= deltazabs){
        y -= deltazabs;
        drawy += sgndeltay;
      }
      drawz += sgndeltaz;
      moveToXYZ(drawx, drawy, drawz);
    }
    return;
  }
}

void drawline(int x1, int y1, int x2, int y2)
{
  penUp();
  lineXYZ(x1,y1,5);
  penDown();
  lineXYZ(x2,y2,-5);
  penUp();
}

/* No direct use of step coordinates below this line */

int convertPosX(float pos)
{ 
  return (int)(pos*conversionFactor*stepsPerMillimeter_X);
}

int convertPosY(float pos)
{ 
  return (int)(pos*conversionFactor*stepsPerMillimeter_Y);
}

int convertPosZ(float pos)
{ 
  return (int)(pos*conversionFactor*stepsPerMillimeter_Z);
}

void drawlinePos(float x1, float y1, float x2, float y2)
{
  drawline(convertPosX(x1),convertPosY(y1),convertPosX(x2),convertPosY(y2));
  posX=x2;
  posY=y2;
}

void linePos(float x2, float y2, float z2)
{
  lineXYZ(convertPosX(x2),convertPosY(y2),convertPosZ(z2));
  posX=x2;
  posY=y2;
  posZ=z2;
}

#ifdef BROKEN
void arcPos(int direction, float x2, float y2, float i, float j)
{
//      case 2://Clockwise arc
//      case 3://Counterclockwise arc
      float angleA, angleB, angle, radius, aX, aY, bX, bY, length, x, y;

      // Center coordinates are always relative

      aX = (X - i);
      aY = (Y - j);
      bX = (x2 - i);
      bY = (y2 - j);

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
      if (angleB <= angleA) angleB += 2 * M_PI;
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
        x = i + radius * cos(angleA + angle * ((float) step / steps));
        y = j + radius * sin(angleA + angle * ((float) step / steps));

      Serial.println("X");
      Serial.println(x);
      Serial.println("Y");
      Serial.println(y);

        // straight line to new points
        linePos(x,y,posZ);
      }
      
  linePos(x2,y2,posZ);
}
#endif /*BROKEN*/

void movePosXYZ (float x2, float y2, float z2, float speed )
{
  setXYSpeed(speed);
  linePos(x2, y2, z2);
}
