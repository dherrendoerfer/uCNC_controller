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

//#include "uCNC_servo.h"

static boolean servoIsrRunning = 0;
static unsigned char servoPin = 0;
static unsigned int servoIsrCounter = 0;

volatile long *servoVal;

#define SERVO_FRAME_LENGTH 160
#define SERVO_MIN_LENGTH 70


static void initISR()
{ 
  servoIsrCounter = 0; // clear the value of the ISR counter;

  noInterrupts();           // disable all interrupts
  TCCR2A = 0;
  TCCR2B = 0;
  
  /* Configure timer2 in normal mode (pure counting, no PWM etc.) */
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);

  /* Disable Compare Match A interrupt enable (only want overflow) */
  TIMSK2 &= ~(1<<OCIE2A);

  TCNT2 = 140;            // preload timer 65536-16MHz/256/2Hz
  TCCR2B |= (1 << CS22);    // prescaler 
  TCCR2B |= (1 << CS20);    // prescaler 
  //TCCR2B &= ~(1 << CS20);
  TCCR2B &= ~(1 << CS21);
  TIMSK2 |= (1 << TOIE2);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts

  servoIsrRunning = 1; // flag to indicate this initialisation code has been executed
  
}

ISR (TIMER2_OVF_vect)
{
  TCNT2 = 236; // preload timer value *experimented
  servoIsrCounter++;
  
  if (servoIsrCounter == SERVO_FRAME_LENGTH) {
    servoIsrCounter = 0;
    return;
  }
  
  if (servoIsrCounter == 1) {
    TCNT2=160;
    digitalWrite(servoPin,HIGH);
    return;
  }
  if (servoIsrCounter == 2) {
    TCNT2=245 - 90 - *servoVal;
    return;
  }
  if (servoIsrCounter == 3) {
    digitalWrite(servoPin,LOW);
  }
}

Servo::Servo(int pin)
{
  servoPin = pin;
  pinMode(pin, OUTPUT);
}

Servo::Servo(int pin, volatile long *val)
{
  servoPin = pin;
  pinMode(pin, OUTPUT);
  servoVal=val;
  initISR();
}

int Servo::attach(volatile long *val)
{
  servoVal = val;
}

void Servo::detach()
{
  servoVal = 0;
}
void Servo::stop()
{
  TIMSK2 &= ~(1 << TOIE2);
  servoIsrRunning = 0;
}

void Servo::start()
{
  initISR();
}


bool Servo::attached()
{
  if (servoVal)
    return 1;
  return 0;
}

bool Servo::running()
{
  
  if (servoIsrRunning)
    return 1;
  return 0;
}


