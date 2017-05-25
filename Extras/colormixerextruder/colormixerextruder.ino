/*
 * This file is part of colormixextruder.
 *
 * Copyright (C) 2017  D.Herrendoerfer
 *
 *   colormixextruder is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   colormixextruder is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with colormixextruder.  If not, see <http://www.gnu.org/licenses/>.
 */

/* FIXME: Enable pin passing is missing */

#include <avr/interrupt.h>

#define EDUARD_BOARD 1
#include "hardware.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

volatile char value = 0;
volatile char dir = 0;

volatile char colorselect = 0;

static char colormatrix[15][3] = {  0b00111111, 0b00000000, 0b00000000, // All color 1 
                                    0b00000000, 0b00111111, 0b00000000, // All color 2
                                    0b00000000, 0b00000000, 0b00111111, // All color 3
                                    0b00100100, 0b00010010, 0b00001001, // All colors mixed
                                    0b00101010, 0b00010101, 0b00000000, // Mix color 1 & 2  50% each
                                    0b00000000, 0b00101010, 0b00010101, // Mix color 2 & 3  50% each
                                    0b00101010, 0b00000000, 0b00010101, // Mix color 1 & 3  50% each
                                    0b00110110, 0b00001001, 0b00000000, // Mix color 1 & 2  66/33%
                                    0b00000000, 0b00110110, 0b00001001, // Mix color 2 & 3  66/33%
                                    0b00110110, 0b00000000, 0b00001001, // Mix color 1 & 3  66/33%
                                    0b00001001, 0b00110110, 0b00000000, // Mix color 1 & 2  33/66%
                                    0b00000000, 0b00001001, 0b00110110, // Mix color 2 & 3  33/66%
                                    0b00001001, 0b00000000, 0b00110110  // Mix color 1 & 3  33/66%
                                  };

void setup()
{
  pinMode(A0,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  pinMode(A2,INPUT_PULLUP);
  
  cli();
  PCICR  = 0b00000010; // Enables Ports B Pin Change Interrupts
  PCMSK1 = 0b00000001; // PCINT08 (A0)
  sei();

  pinMode(A4988_A_ENA_PIN,OUTPUT);
  pinMode(A4988_A_STP_PIN,OUTPUT);
  pinMode(A4988_A_DIR_PIN,OUTPUT);

  pinMode(A4988_B_ENA_PIN,OUTPUT);
  pinMode(A4988_B_STP_PIN,OUTPUT);
  pinMode(A4988_B_DIR_PIN,OUTPUT);

  pinMode(A4988_C_ENA_PIN,OUTPUT);
  pinMode(A4988_C_STP_PIN,OUTPUT);
  pinMode(A4988_C_DIR_PIN,OUTPUT);

  digitalWrite(A4988_A_ENA_PIN, LOW);
  digitalWrite(A4988_B_ENA_PIN, LOW);
  digitalWrite(A4988_C_ENA_PIN, LOW);

  Serial.begin(115200);
}


/* Looking for this String: 
    " Txx" followed by a star or a line end */
char* expect = "T";
char eptr = 0;
char elen = 1;
char col = 0;

unsigned int light=0;

char _readbl() {
  while (!Serial.available());
  return Serial.read();
}


void loop()
{
  char tmp;

  /* The LED indicator */
  if (light) {
    if ( light < 30000) {
      light++;
      digitalWrite(LED,HIGH);
    }
    else {
      digitalWrite(LED,LOW);
      light = 0;
    }
  }

  /* Streaming serial read and find for T codes */  
  if (Serial.available()) {  
    tmp=Serial.read();
    
    if (tmp == expect[eptr]) {
      light=20000;
      eptr++;
    }
    else
      eptr=0;
      
    if (eptr == elen) {
      eptr = 0;
      tmp=_readbl();
      if ( tmp >= '0' && tmp <='9') {
        col = (tmp - '0');
        tmp=_readbl();
        if ( tmp >= '0' && tmp <='9' ) {
          col = 10*col + (tmp - '0');
          tmp=_readbl();
          if ( tmp == '*' || tmp == '\n' ) {
            colorselect = col;
            light=1;
          }
        }
        else {
          if (tmp == '*' || tmp == '\n') {
            colorselect = col;
            light=1;
          }
        }
      }
    }
  }  
}

/* Simplified interrupt handler for catching a single event
   on any input pin.*/
ISR(PCINT1_vect)
{ 
  /* Ignoring the value of A0 since it 
     triggered this interrupt */
  if (!digitalRead(A2)) {
    if (digitalRead(A1)) {
      value++;
      dir = HIGH;
    }
    else {
      value--;
      dir = LOW;
    }
    if (value > 5)
      value=0;
    else if (value < 0)
      value=5;
    
    /*Translate & Move*/
    
    if(CHECK_BIT(colormatrix[colorselect][0], value)){
      digitalWrite(A4988_A_DIR_PIN,dir);
      digitalWrite(A4988_A_STP_PIN,HIGH);
    }
    if(CHECK_BIT(colormatrix[colorselect][1], value)){
      digitalWrite(A4988_B_DIR_PIN,dir);
      digitalWrite(A4988_B_STP_PIN,HIGH);
    }
    if(CHECK_BIT(colormatrix[colorselect][2], value)){
      digitalWrite(A4988_C_DIR_PIN,dir);
      digitalWrite(A4988_C_STP_PIN,HIGH);
    }
    __asm__("nop\n\t");
    digitalWrite(A4988_A_STP_PIN,LOW);
    digitalWrite(A4988_B_STP_PIN,LOW);
    digitalWrite(A4988_C_STP_PIN,LOW);
    
  }
}


