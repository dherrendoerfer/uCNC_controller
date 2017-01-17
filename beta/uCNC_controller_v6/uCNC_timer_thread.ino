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

#define ledPin 13

//volatile int timer_idle = 1;

volatile unsigned int TMVAL = 34286;

void timer1_setup()
{
  pinMode(ledPin, OUTPUT);

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 34286;            // preload timer 65536-16MHz/256/2Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

void tick_handler()
{
  sei(); // Allow interrupts in here. 
  mStepperX.tick();
  mStepperY.tick();
  mStepperZ.tick();
  if (!mStepperX.busy() && !mStepperY.busy() && !mStepperZ.busy()) {
    timer_idle = 1;
    TMVAL = 34286;
  }
}

// interrupt service routine that wraps a user defined function 
// supplied by attachInterrupt
ISR(TIMER1_OVF_vect)        
{
  TCNT1 = TMVAL;
  if (!timer_idle)
    tick_handler();
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}

unsigned int hzToTimer1(unsigned int Hz)
{
  return 65536-(62500/Hz);
}

void start_timer() 
{
  timer_idle = 0;

  if (mStepperX.busy()) {
    TMVAL = hzToTimer1(mStepperX.getHz());
    TCNT1 = TMVAL;
    return;
  }
  if (mStepperY.busy()) {
    TMVAL = hzToTimer1(mStepperY.getHz());
    TCNT1 = TMVAL;
    return;
  }
  if (mStepperZ.busy()) {
    TMVAL = hzToTimer1(mStepperZ.getHz());
    TCNT1 = TMVAL;
    return;
  }
}



