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

/* Define these according to your driver board design 
   Only define the ones you want/need this saves memory */
//#define STEPPER_2PIN 1
#define STEPPER_3PIN 1
#define STEPPER_4PIN 1
#define STEPPER_EMULATION

class Stepper {
  public:
    // constructors:
#ifdef STEPPER_2PIN    
    Stepper(int motor_pin_1, int motor_pin_2);
#endif

#ifdef STEPPER_3PIN    
    Stepper(int motor_pin_1, int motor_pin_2, int motor_pin_3);
#endif

#ifdef STEPPER_4PIN    
    Stepper(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, int hstep);
#endif

#ifdef STEPPER_EMULATION    
    Stepper(volatile long *motor_pin_1);
#endif

    void resetEmu();
    void attachEmu(volatile long *emu);
    
    void setSpeed(int whatSpeed);
    int getSpeed();
    void setSlack(int slack);
    int getSlack();
    void update(int step_to_move);
    void powerdown();
    
    void plan(int seq_number, int direction, posval_t abssteps, posval_t deltaabs, posval_t startval, unsigned int Hz, unsigned int startHz);
    void tick();
    int getHz();
    int busy();

  private:
    void stepMotor(int this_step);
    
    volatile long *motor_pos_emu;    // pointer to the emulated stepper value

    int speed;                      // Speed in Hz
    int slack;                      // Slack of gears or belt
    int pin_count;                  // whether you're driving the motor with 2 or 4 pins
    volatile unsigned int step_number;       // which step the motor is on
    int halfstep;                   // use halfsteps (doubles steps per rotation)

    // motor pin numbers:
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;

    int last_step;
    
    int plan_direction;
    posval_t plan_steps;
    posval_t plan_abssteps;
    posval_t plan_deltaabs;
    posval_t plan_startval;
    unsigned int plan_Hz;
    unsigned int plan_startHz;

    volatile int plan_active;
    volatile int plan_seqno;
};


