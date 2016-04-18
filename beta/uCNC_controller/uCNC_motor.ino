
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

#define DRILL_SPEED 0.1
#define DRILL_DEPTH 10 //mm

void initMotors()
{
  /*Set stepper base speed */
  myStepper1.setSpeed(stepIssueFrequency_X);
  myStepper2.setSpeed(stepIssueFrequency_Y);
  myStepper3.setSpeed(stepIssueFrequency_Z);
  
  /*Set steppers slack (if any)*/
  myStepper1.setSlack(stepDriveSlack_X);
  myStepper2.setSlack(stepDriveSlack_Y);
  myStepper3.setSlack(stepDriveSlack_Z);

  myServo.attach(12);
  myServo.write(servoPosMax);
}

void homeXYZ()
{
  int i=0;
  
  if(pos_known) {
    movePosXYZ (0, 0, 0, 0);
    return;
  }
#ifdef DO_RESET  
  if (have_endswitch) {
    /* Go (slowly) into each end-switch-on position and back out
     * until the switch opens, and then some more.
     * The direction is derived from the sign values of the 
     * distance calculation base values.  
     *
     * if the end-switch is on go forward a bit 
     */
     
    if (analogRead(0) > 200)
      myStepper1.step(50*sgn(stepsPerMillimeter_X), RESET_TRAVEL_STEP_DELAY); 
    if (analogRead(0) > 200)
      myStepper2.step(50*sgn(stepsPerMillimeter_Y), RESET_TRAVEL_STEP_DELAY); 
    if (motorMode==0 && analogRead(0) > 200)
      myStepper3.step(50*sgn(stepsPerMillimeter_Z), RESET_TRAVEL_STEP_DELAY); 
    
    if (analogRead(0) > 200) // fail, switch is still on
      return;    
    
    while(i++<RESET_TRAVEL_X && analogRead(0) < 200)
      myStepper1.step(-1*sgn(stepsPerMillimeter_X), RESET_TRAVEL_STEP_DELAY);
    i=0;
    while(i++<200 && analogRead(0) > 200)
      myStepper1.step(1*sgn(stepsPerMillimeter_X), RESET_TRAVEL_STEP_DELAY*20);
    myStepper1.step(RESET_PRELOAD_X*sgn(stepsPerMillimeter_X), RESET_TRAVEL_STEP_DELAY);

    i=0;
    while(i++<RESET_TRAVEL_Y && analogRead(0) < 200)
      myStepper2.step(-1*sgn(stepsPerMillimeter_Y), RESET_TRAVEL_STEP_DELAY);
    i=0;
    while(i++<200 && analogRead(0) > 200)
      myStepper2.step(1*sgn(stepsPerMillimeter_Y), RESET_TRAVEL_STEP_DELAY*20);
    myStepper2.step(RESET_PRELOAD_Y*sgn(stepsPerMillimeter_Y), RESET_TRAVEL_STEP_DELAY);

    if (motorMode == 0) {
      i=0;
      while(i++<RESET_TRAVEL_Z && analogRead(0) < 200)
        myStepper3.step(-1*sgn(stepsPerMillimeter_Z), RESET_TRAVEL_STEP_DELAY);
      i=0;
      while(i++<200 && analogRead(0) > 200)
        myStepper3.step(1*sgn(stepsPerMillimeter_Z), RESET_TRAVEL_STEP_DELAY*20);
      myStepper3.step(RESET_PRELOAD_Z*sgn(stepsPerMillimeter_Z), RESET_TRAVEL_STEP_DELAY);
    }
    
  } else {
    /* Reset the position by moving a defined distance, (into a block)
    *  and then a defined distance back out again.
    */ 
    myStepper1.step(-1*sgn(stepsPerMillimeter_X)*RESET_TRAVEL_X, RESET_TRAVEL_STEP_DELAY);
    myStepper2.step(-1*sgn(stepsPerMillimeter_Y)*RESET_TRAVEL_Y, RESET_TRAVEL_STEP_DELAY);
    if (motorMode == 0) {
      myStepper3.step(-1*sgn(stepsPerMillimeter_Z)*RESET_TRAVEL_Z, RESET_TRAVEL_STEP_DELAY);
      delay(500);
      myStepper3.step(sgn(stepsPerMillimeter_Z)*RESET_PRELOAD_Z, RESET_TRAVEL_STEP_DELAY);
    }
    myStepper2.step(sgn(stepsPerMillimeter_Y)*RESET_PRELOAD_Y, RESET_TRAVEL_STEP_DELAY);
    myStepper1.step(sgn(stepsPerMillimeter_X)*RESET_PRELOAD_X, RESET_TRAVEL_STEP_DELAY);
  }
#endif  
  /* Reset this position to be 0,0,0 */
  resetPosXYZ();
}

int moveX(posval_t dX, char *px, char *py, char *pz)
{
  int i;
  X = X + dX;

  *px += tristate(dX);
//  *py += tristate(dX); // My Plotter (needs COMPLEX_MOVE defined)
  return stepIssueFrequency_X;
}

int moveY(posval_t dY, char *px, char *py, char *pz)
{
  Y = Y + dY;
  
  switch(motorMode) {
  case 0:
    *py += tristate(dY);
    break;
  case 1:
  case 2:
  case 3:
    *py += tristate(dY);  // Stepper 2 and 3 are Y
    *pz += tristate(dY);
    break;
  }  
  return (stepIssueFrequency_Y);
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
}

void servoZ()
{
  int servoPos = ((float)posZ*servoPosZfactor) + 90;
  updateServo(servoPos);
}

int moveZ(posval_t dZ, char *px, char *py, char *pz)
{
  Z = Z + dZ;  

  switch(motorMode) {
  case 0:
    *pz += tristate(dZ);
    break;
  case 1:
    digitalWrite(led,(Z < 0) ? HIGH : LOW);
    if ( (Z == -1 && dZ == -1) || (Z == 0 && dZ == 1)) {
      updateServo((Z < 0) ? servoPosMin : servoPosMax);
      delay(100);
    }
    return 0;
    break;
  case 2:
    updateServo(servoPosMin + (servoPosZfactor*posZ));
    delay(10);
    break;
  case 3:
    digitalWrite(led,(posZ > 0) ? HIGH : LOW);
    break;
  }
  return (stepIssueFrequency_Z);
}

void powerdown()
{
  myStepper1.powerdown();
  myStepper2.powerdown();
  myStepper3.powerdown();
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

void _moveToXYZ(posval_t pX, posval_t pY, posval_t pZ, int accelX, int accelY, int accelZ)
{ 
  int fx,fy,fz;
  char px,py,pz;   // The frequencies for each stepper

  fx=0; //motor frequencies
  fy=0;
  fz=0;

  px=0; //position increments
  py=0;
  pz=0;

  if (pX - X)
    fx = moveX(pX - X, &px, &py, &pz);
  if (pY - Y)
    fy = moveY(pY - Y, &px, &py, &pz);
  if (pZ - Z)
    fz = moveZ(pZ - Z, &px, &py, &pz);

  /* in theory px,py,pz should never be larger than
   * 2 or smaller than -2. Therefore there should never
   * be more than 2 move schedules here. */
  
  if (fx) {
    myStepper1.chk(fx+accelX,0);
  }
  if (fy) {
    myStepper2.chk(fy+accelY,0);
  }
  if (fz) {
    myStepper3.chk(fz+accelZ,0);
  }

  if (px) {
    myStepper1.update(tristate(px));
  }
  if (py) {
    myStepper2.update(tristate(py));
  }
  if (pz) {
    myStepper3.update(tristate(pz));
  } 

#ifdef COMPLEX_MOVE
  if (abs(px)==2) {
    myStepper1.chk(fx+accelX);
    myStepper1.update(tristate(px));
  }
  if (abs(py)==2) {
    myStepper2.chk(fy+accelY);
    myStepper2.update(tristate(py));
  }
  if (abs(pz)==2) {
    myStepper3.chk(fz+accelZ);
    myStepper3.update(tristate(pz));
  }
#endif
}

int accelerate( posval_t pos,posval_t distance, int acceleration, int limit )
{
  unsigned long accval;
  
  if ( pos < distance/2 ) {
    // accelerate 
    accval = pos * acceleration;
    if (accval < limit)
      return (int)accval;
  }
  else {
    // brake
    accval = (distance-pos) * acceleration;
    if (accval < limit)
      return (int)accval;
  }
  
  return limit;
}

void moveToXYZ(posval_t pX, posval_t pY, posval_t pZ, int accelX, int accelY, int accelZ)
{ 
  posval_t absX = abs(pX - X);
  posval_t absY = abs(pY - Y);
  posval_t absZ = abs(pZ - Z);
  
  posval_t deltaX = absX;
  posval_t deltaY = absY;
  posval_t deltaZ = absZ;

  int mX,mY,mZ;
  
  while (absX || absY || absZ)
  {
    mX = 0;
    mY = 0;
    mZ = 0;
    
    if (absX) {
      mX = tristate(pX - X);
      absX--;
      accelX=accelerate(deltaX-absX, deltaX, stepIssueFreqRamp_X, stepIssueFrequencyRampMax_X);
    }
    if (absY) {
      mY = tristate(pY - Y);
      absY--;
      accelY=accelerate(deltaY-absY, deltaY, stepIssueFreqRamp_Y, stepIssueFrequencyRampMax_Y);
    }
    if (absZ) {
      mZ = tristate(pZ - Z);
      absZ--;
      accelZ=accelerate(deltaZ-absZ, deltaZ, stepIssueFreqRamp_Z, stepIssueFrequencyRampMax_Z);
    }
    _moveToXYZ(X + mX, Y + mY, Z + mZ, accelX, accelY, accelZ);
  }
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

void wait(long steptime)
{
  long delay_time;
  
  if (!steptime)
    return;
    
  while (millis() < steptime ) {
    delayMicroseconds(500);
  }
  
  return;
}

void lineXYZ(posval_t x2, posval_t y2, posval_t z2, float feedrate)
{
  posval_t n, deltax, deltay, deltaz, sgndeltax, sgndeltay, sgndeltaz, deltaxabs, deltayabs, deltazabs, x, y, z, drawx, drawy, drawz;

  deltax = x2 - X;
  deltay = y2 - Y;
  deltaz = z2 - Z;
  deltaxabs = abs(deltax);
  deltayabs = abs(deltay);
  deltazabs = abs(deltaz);
  sgndeltax = sgn(deltax);
  sgndeltay = sgn(deltay);
  sgndeltaz = sgn(deltaz);
  x = deltaxabs >> 1;
  y = deltayabs >> 1;
  z = deltazabs >> 1;
  drawx = X;
  drawy = Y;
  drawz = Z;
  
  int accel=0;
  
  int stepdelay = 0;
  long time=0;
  long laststep = 0;
  
  int delay_X=6000/(fabs(stepsPerMillimeter_X) * feedrate);
  int delay_Y=6000/(fabs(stepsPerMillimeter_Y) * feedrate);
  int delay_Z=6000/(fabs(stepsPerMillimeter_Z) * feedrate);
    
  moveToXYZ(drawx, drawy, drawz, 0, 0, 0);
  laststep=millis();
  
  // dX is biggest
  if(deltaxabs >= deltayabs && deltaxabs >= deltazabs){
    for(n = 0; n < deltaxabs; n++){
      stepdelay = delay_X;

      y += deltayabs;
      if(y >= deltaxabs){
        y -= deltaxabs;
        drawy += sgndeltay;
        stepdelay += delay_Y;
      }
      z += deltazabs;
      if(z >= deltaxabs){
        z -= deltaxabs;
        drawz += sgndeltaz;
        stepdelay += delay_Z;
      }

      drawx += sgndeltax;
      
      accel = accelerate(n,deltaxabs, stepIssueFreqRamp_X, stepIssueFrequencyRampMax_X);

      wait(laststep+stepdelay);
      _moveToXYZ(drawx, drawy, drawz, accel, 0, 0);
      laststep=millis();
    }
    return;
  }
  // dY is biggest
  if(deltayabs >= deltaxabs && deltayabs >= deltazabs){
    for(n = 0; n < deltayabs; n++){
      stepdelay = delay_Y;

      x += deltaxabs;
      if(x >= deltayabs){
        x -= deltayabs;
        drawx += sgndeltax;
        stepdelay += delay_X;
      }
      z += deltazabs;
      if(z >= deltayabs){
        z -= deltayabs;
        drawz += sgndeltaz;
        stepdelay += delay_Z;
      }
      drawy += sgndeltay;

      accel = accelerate(n,deltayabs, stepIssueFreqRamp_Y, stepIssueFrequencyRampMax_Y);

      wait(laststep+stepdelay);
      _moveToXYZ(drawx, drawy, drawz, 0, accel, 0);
      laststep=millis();
    }
    return;
  }
  // dZ is biggest
  if(deltazabs >= deltaxabs && deltazabs >= deltayabs){
    for(n = 0; n < deltazabs; n++){
      stepdelay = delay_Z;

      x += deltaxabs;
      if(x >= deltazabs){
        x -= deltazabs;
        drawx += sgndeltax;
        stepdelay += delay_X;
      }
      y += deltayabs;
      if(y >= deltazabs){
        y -= deltazabs;
        drawy += sgndeltay;
        stepdelay += delay_Y;
      }
      drawz += sgndeltaz;

      accel = accelerate(n,deltazabs, stepIssueFreqRamp_Z, stepIssueFrequencyRampMax_Z);

      wait(laststep+stepdelay);
      _moveToXYZ(drawx, drawy, drawz, 0, 0, accel);
      laststep=millis();
    }
    return;
  }
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
