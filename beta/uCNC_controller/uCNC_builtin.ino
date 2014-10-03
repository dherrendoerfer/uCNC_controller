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
 */

/* Built-in functions for setup and debug purposes 
 * using these may require some tweaking           */

#ifdef BUILTIN

void penDown()
{
    jumpPos(posX,posY,-2);
}

void penUp()
{
  jumpPos(posX,posY,2);
  delay(350);
}

void _drawline(int x1, int y1, int x2, int y2)
{
  penUp();
  lineXYZ(x1, y1, Z, 30.0);
  penDown();
  lineXYZ(x2, y2, Z, 30.0);
  penUp();
}

void drawline(int x1, int y1, int x2, int y2)
{
  _drawline(sgn(stepsPerMillimeter_X)*x1,sgn(stepsPerMillimeter_Y)*y1,
            sgn(stepsPerMillimeter_X)*x2,sgn(stepsPerMillimeter_Y)*y2 );
}

void drawlinePos(float x1, float y1, float x2, float y2)
{
  penUp();
  jumpPos(x1, y1, posZ );
  penDown();
  linePos(x2, y2, posZ, 30.0);
  penUp();

}

void calibratePattern()
{
  int i=0;
  /*Draw a line pattern in absolute step mode*/
/*
  drawline(50,50,1000,50);
  drawline(1000,150,50,150);
  drawline(50,250,1000,250);
  drawline(1000,350,50,350);

  for (i=50; i<1000; i+=50)
    drawline(i,30,i,70);

  for (i=100; i<1000; i+=100)
    drawline(i,30,i,170);

  for (i=200; i<1000; i+=200)
    drawline(i,30,i,270);

  for (i=400; i<1000; i+=400)
    drawline(i,30,i,370);
*/
  /*Draw a line pattern in 5,10,20,40mm spacing*/
  drawlinePos(5,25,60,25);
  drawlinePos(60,30,5,30);
  drawlinePos(5,35,60,35);
  drawlinePos(60,40,5,40);
  
  for (i=5; i<60; i+=5)
    drawlinePos(i,23,i,27);
  
  for (i=10; i<60; i+=10)
    drawlinePos(i,23,i,32);

  for (i=20; i<60; i+=20)
    drawlinePos(i,23,i,37);

  for (i=40; i<60; i+=40)
    drawlinePos(i,23,i,42);
    
  i=0;
}

void slackPattern()
{
  int i=0;
  stepDriveSlack_X = 0;
  stepDriveSlack_Y = 0;
  
  while (i<65) {
    myStepper1.setSlack(stepDriveSlack_X);
    myStepper2.setSlack(stepDriveSlack_Y);
    
    drawlinePos(i+0,2,i+0,2);
    drawlinePos(i+0,5,i+5,5);
    drawlinePos(i+5,5,i+5,10);
    
    drawlinePos(i+10,9,i+10,9);
    drawlinePos(i+10,5,i+5,5);
    drawlinePos(i+5,5,i+5,0);
  
    i+=8;
    stepDriveSlack_X += 10;
    stepDriveSlack_Y += 10;
  }
}

void slackPattern2()
{
  int i=0;
  stepDriveSlack_X = 30;
  stepDriveSlack_Y = 30;

  myStepper1.setSlack(stepDriveSlack_X);
  myStepper2.setSlack(stepDriveSlack_Y);
  
  while (i<65) {
    myStepper1.setSlack(stepDriveSlack_X);
    
    drawlinePos(i+1,1,i+1,9);
    drawlinePos(i+3,1,i+3,9);
    drawlinePos(i+5,1,i+5,9);
    drawlinePos(i+7,1,i+7,9);
    
    drawlinePos(i+6,1,i+6,9);
    drawlinePos(i+4,1,i+4,9);
    drawlinePos(i+2,1,i+2,9);
  
    stepDriveSlack_X += 10;
    i+=10;
  }
  i=0;
  while (i<65) {
    myStepper2.setSlack(stepDriveSlack_Y);
    
    drawlinePos(i+1,11,i+9,11);
    drawlinePos(i+1,13,i+9,13);
    drawlinePos(i+1,15,i+9,15);
    drawlinePos(i+1,17,i+9,17);
    
    drawlinePos(i+1,16,i+9,16);
    drawlinePos(i+1,14,i+9,14);
    drawlinePos(i+1,12,i+9,12);
  
    stepDriveSlack_Y += 10;
    i+=10;
  }
}

#endif

#ifdef BUILTIN
void dumpSettings()
{
  Serial.print("Hello, this is uCNC_contoller version ");
  Serial.println(uCNC_Version,1);
  Serial.println("");
  Serial.println("current settings are:");
  Serial.println("");
  Serial.print("motorMode:");
  Serial.println(motorMode);
  Serial.println("Stepper positions:");
  Serial.print("X:");
  Serial.println(X);
  Serial.print("Y:");
  Serial.println(Y);
  Serial.print("Z:");
  Serial.println(Z);
  Serial.println("Absolute positions:");
  Serial.print("posX:");
  Serial.println(posX,3);
  Serial.print("posY:");
  Serial.println(posY,3);
  Serial.print("posZ:");
  Serial.println(posZ,3);
  Serial.println("Stepper settings:");
  Serial.print("stepsPerMillimeter_X:");
  Serial.println(stepsPerMillimeter_X, 3);
  Serial.print("stepsPerMillimeter_Y:");
  Serial.println(stepsPerMillimeter_Y, 3);
  Serial.print("stepsPerMillimeter_Z:");
  Serial.println(stepsPerMillimeter_Z, 3);
  Serial.println("Servo settings:");
  Serial.print("servoPosZfactor:");
  Serial.println(servoPosZfactor, 3);
  Serial.print("servoPosMax:");
  Serial.println(servoPosMax);
  Serial.print("servoPosMin:");
  Serial.println(servoPosMin);
  Serial.print("servoToolInc:");
  Serial.println(servoToolInc);
  Serial.print("EOM."); 
}
#endif


