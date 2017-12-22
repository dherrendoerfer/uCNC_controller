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
 *  Code in this file is derived from the Arduino stepper library by:
 *    Tom Igoe, Sebastian Gassner,David Mellis, and Noah Shibley
 *  licensed under: Creative Commons Attribution-ShareAlike 3.0 License.
 *  Credit and authorship remains with the original authors.
 *
 * This source file is duplicated to faciliate the adaptation to look-ahead
 * and acceleration code regarding the use in CNC type machines.
 */

/* README:
This file contains the drivers for 3 types of stepper controllers.
By choosing the appropriate constructor you choose the driver.
Available are:

2Pin: Bipolar driver for double-h bar driver chips
3pin: Bipolad driver for A4988 or similar chips with direction and step 
      interface and enable
4pin: Unipolar stepper driver for use with ULN2003/2004 driver ICs
*/

#ifdef STEPPER_2PIN
/*
 * two-wire constructor.
 * Sets which wires should control the motor.
 */
Stepper::Stepper(int motor_pin_1, int motor_pin_2)
{
  this->step_number = 0;      // which step the motor is on
  this->speed = 0;            // the motor speed, in Hz
  this->direction = 0;        // motor direction
  this->last_step_time = 0;   // time stamp in ms of the last step taken
  this->last_step = 0;

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);

  // When there are only 2 pins, set the other two to 0:
  this->motor_pin_3 = 0;
  this->motor_pin_4 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 2;
}
#endif

#ifdef STEPPER_3PIN
/*
 * three-wire constructor.
 * Sets which wires should control the motor.
 * pin 1: direction
 * pin 2: step
 * pin 3: enable(inverted)
 */
Stepper::Stepper(int motor_pin_1, int motor_pin_2, int motor_pin_3)
{
  this->step_number = 0;      // which step the motor is on
  this->speed = 0;            // the motor speed, in Hz
  this->last_step_time = 0;   // time stamp in ms of the last step taken
  this->last_step = 0;

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
  pinMode(this->motor_pin_3, OUTPUT);

  // When there are only 2 pins, set the other two to 0:
  this->motor_pin_4 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 3;
}
#endif


#ifdef STEPPER_4PIN
/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */
Stepper::Stepper(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, int hstep)
{
  this->step_number = 0;        // which step the motor is on
  this->speed = 0;              // the motor speed, in Hz
  this->last_step_time = 0;     // time stamp in ms of the last step taken
  this->last_step = 0;
  this->halfstep = hstep;

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
  pinMode(this->motor_pin_3, OUTPUT);
  pinMode(this->motor_pin_4, OUTPUT);

  // pin_count is used by the stepMotor() method:
  this->pin_count = 4;
}
#endif

/*
  Sets the speed in steps per second (or Hz)
*/
void Stepper::setSpeed(int whatSpeed)
{
  this->speed = whatSpeed;
}

void Stepper::setSlack(int slack)
{
  this->slack = slack;
}

/*
   Check if it is ok to issue a step now, at the given
   step frequency.
 */
int Stepper::chk(long frequency, int noblock)
{
  unsigned long delay_time;
  
  if (!frequency)
    return 0;
    
  delay_time = 1000000L / frequency;

  if (noblock) {
    if (micros() - this->last_step_time <  delay_time)
      return 1;
    else
      return 0;
  }
    
  while (micros() - this->last_step_time <  delay_time) {
    delayMicroseconds(5);
  }
  
  return 0;
}

void Stepper::unslack(int direction)
{
  int i;
  for (i=0;i<this->slack;i++) {
    stepMotor(direction);

    chk(this->speed,0);
  }
}

/*
  Moves the stepper exactly one step forward or back
 */
void Stepper::update(int step_to_move)
{
  int step;
  
  if (step_to_move == 0)
    return;
  
  step = sgn(step_to_move);
  
  if (this->slack && step != this->last_step){
    unslack(step); //unslack in this direction
  }
  
  stepMotor(step);
}

/*
 * Moves the motor forward or backwards.
 */
void Stepper::stepMotor(int step)
{
  int thisStep;
  
  this->step_number += step;
  this->last_step_time = micros();
  this->last_step = step;


#ifdef STEPPER_2PIN
  if (this->pin_count == 2) {
    thisStep = this->step_number % 4;
    
    switch (thisStep) {
      case 0: /* 01 */
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      break;
      case 1: /* 11 */
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, HIGH);
      break;
      case 2: /* 10 */
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      break;
      case 3: /* 00 */
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      break;
    }
    return;
  }
#endif

#ifdef STEPPER_3PIN
  if (this->pin_count == 3) {
    /*Enable stepper driver*/
    digitalWrite(motor_pin_3, LOW);
    /*Set direction pin*/
    digitalWrite(motor_pin_1,(step==1));
    /*Pulse step line*/
    digitalWrite(motor_pin_2, HIGH);
    delayMicroseconds(1);
    digitalWrite(motor_pin_2, LOW);
    return;
  }
#endif

#ifdef STEPPER_4PIN
  if (this->pin_count == 4) {
    if (this->halfstep)
      thisStep = this->step_number % 8;
    else 
      thisStep = 2 * (this->step_number % 4);
        
    switch (thisStep) {
      case 0:    // 1010
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
      break;
      case 1:    // 0010
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
      break;
      case 2:    // 0110
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
      break;
      case 3:    // 0100
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, LOW);
      break;
      case 4:    //0101
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
      break;
      case 5:    //0001
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
      break;
      case 6:    //1001
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
      break;
      case 7:    //1000
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, LOW);
      break;
    }
  }
#endif
}   

void Stepper::powerdwn()
{
#ifdef STEPPER_2PIN
  if (this->pin_count == 2) {
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
  }
#endif
#ifdef STEPPER_3PIN
  if (this->pin_count == 3) {
      /* Pull enable pin HIGH*/
      digitalWrite(motor_pin_3, HIGH);
  }
#endif
#ifdef STEPPER_4PIN
  if (this->pin_count == 4) {
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, LOW);
  }
#endif
}

void Stepper::powerup()
{
#ifdef STEPPER_3PIN
  if (this->pin_count == 3) {
      /* Pull enable pin HIGH*/
      digitalWrite(motor_pin_3, LOW);
  }
#endif
}


void Stepper::home(unsigned int switch_input, unsigned long home_steps, unsigned long out_steps, int direction, unsigned int speed)
{
  unsigned long i=0;
  /* Move into the switch */

  while(i++<home_steps && analogRead(switch_input) < ENDSWITCH_ON_LEVEL) {
    chk(speed,0);
    update(-1*direction);
  }
  
  delay(500);
  
  i=0;
  while(i++<out_steps && analogRead(switch_input) > ENDSWITCH_ON_LEVEL) {
    chk(speed,0);
    update(direction);
  }
}

void Stepper::step(unsigned long steps, unsigned int speed)
{
  unsigned long i=0;

  while(i++<steps) {
    chk(speed,0);
    update(sgn(steps));
  }
}
