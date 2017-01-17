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
  this->step_number = 0;        // which step the motor is on
  this->speed = 0;              // the motor speed, in Hz
  this->last_step = 0;
  this->halfstep = hstep;

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

  // fix the position value pointer to 0
  this->motor_pos_emu = 0;
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
  this->last_step = 0;

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
  pinMode(this->motor_pin_3, OUTPUT);

  // When there are only 3 pins, set the other one to 0:
  this->motor_pin_4 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 3;

  // fix the position value pointer to 0
  this->motor_pos_emu = 0;
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

  // fix the position value pointer to 0
  this->motor_pos_emu = 0;
}
#endif

#ifdef STEPPER_EMULATION
/*
 * Emulation constructor.
 * Solely operates on a memory value.
 */
Stepper::Stepper(volatile long *motor_pin_1 )
{
  this->step_number = 0;        // which step the motor is on
  this->speed = 0;              // the motor speed, in Hz
  this->last_step = 0;

  // Arduino pins for the motor control connection:
  this->motor_pos_emu = motor_pin_1;

  // When not using any pins, set them to 0:
  this->motor_pin_1 = 0;
  this->motor_pin_2 = 0;
  this->motor_pin_3 = 0;
  this->motor_pin_4 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 0;
}
#endif

/*
  Sets the speed in steps per second (or Hz)
*/
void Stepper::setSpeed(int whatSpeed)
{
  this->speed = whatSpeed;
}

int Stepper::getSpeed()
{
  return(this->speed);
}

void Stepper::setSlack(int slack)
{
  this->slack = slack;
}

int Stepper::getSlack()
{
  return (this->slack);
}

void Stepper::plan(int seq_number, int direction, posval_t abssteps, posval_t deltaabs, posval_t startval, unsigned int Hz, unsigned int startHz)
{
  this->plan_direction = direction;
  this->plan_abssteps = abssteps;
  this->plan_deltaabs = deltaabs;
  this->plan_startval = startval;
  
  this->plan_steps = abssteps;
  this->plan_seqno = seq_number;
  this->plan_active = true;
  this->plan_Hz = Hz;
  this->plan_startHz = startHz;
}

int Stepper::getHz()
{
  return (this->plan_Hz);
}

void Stepper::tick()
{
  if(!this->plan_active)
    return;
  
  this->plan_steps--;
  
  this->plan_startval += this->plan_deltaabs;
  if(this->plan_startval >= this->plan_abssteps){
    this->plan_startval -= this->plan_abssteps;
    stepMotor(this->plan_direction);
  }
  
  if (!plan_steps) {
    this->plan_active=false;
  }
}

int Stepper::busy()
{
  return (this->plan_active);
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
  
  stepMotor(step);
}

/*
 * Reset the steppers emulated position.
 */
void Stepper::resetEmu()
{
  *this->motor_pos_emu = 0;
}

/*
 * Attach a variable to store the emulated position.
 */
void Stepper::attachEmu(volatile long *emu)
{
  this->motor_pos_emu = emu;
}

/*
 * Moves the motor forward or backwards.
 */
void Stepper::stepMotor(int step)
{
  int thisStep;
  
  this->step_number += step;
  this->last_step = step;

  if(this->motor_pos_emu)
    *this->motor_pos_emu += step;

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

void Stepper::powerdown()
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
