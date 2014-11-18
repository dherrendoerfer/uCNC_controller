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

/* Typedef for the data type for absolute positions 
    depending on the size of the target surface this
    may need to be changed from int to long */
typedef int posval_t;

/* Define these according to your driver board design 
    currently only 4pin is supported */
//#define STEPPER_2PIN 1
#define STEPPER_4PIN 1

class Stepper {
  public:
    // constructors:
#ifdef STEPPER_2PIN    
    Stepper(int motor_pin_1, int motor_pin_2);
#endif

#ifdef STEPPER_4PIN    
    Stepper(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);
#endif

    int chk(long frequency, int noblock);
    void setSpeed(int whatSpeed);
    void setSlack(int slack);
    void update(int step_to_move);
    void step(int steps, int delay_ms);
    void powerdown();

  private:
    void stepMotor(int this_step);
    void unslack(int direction);

    int speed;                      // Speed in Hz
    int slack;                      // Slack of gears or belt
    int pin_count;                  // whether you're driving the motor with 2 or 4 pins
    unsigned int step_number;       // which step the motor is on

    // motor pin numbers:
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;

    long last_step_time;      // time stamp in ms of when the last step was taken
    int last_step;
};

int sgn(posval_t value)
{
  return value < 0 ? -1 : value > 0;
}

int tristate(posval_t value)
{
  if (value == 0)
    return 0;
  return value < 0 ? -1 : value > 0;
}

