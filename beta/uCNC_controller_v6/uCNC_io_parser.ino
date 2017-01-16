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

#define MAX_COMMANDS 8 

struct command_t {
        uint8_t type; //i.e. G or M
        double  value; //string value associated
        //struct command_t *next;
};

//str: token in the form of Xnnn
//old: head of object chain else null
//returns: head of object chain

struct command_t command_list[MAX_COMMANDS];
uint8_t commandLength = 0;

void addObj(uint8_t *str) 
{
  struct command_t *c;
  if (commandLength == MAX_COMMANDS) {
     return;
  }
  c = &command_list[commandLength++];
  c->type   = str[0];
  c->value  = strtod((const char*)&str[1], NULL);
}

void purge_commands() 
{
  commandLength = 0;
}

void parse_commands(uint8_t *str) 
{
  uint8_t *token;
  uint8_t index = 0;

  while (str[index]) {
    token=str+index;
    while (str[index] > 'A' && str[index] < 'Z')
      index++;
      
    if (str[index] == ' ')
      index++;
    
    while (str[index] != ' ' && str[index] != 0)
      index++;

    if (str[index] == ' ')
      index++;
    
    addObj(token);
  }
}

//returns zero if value does not exist.
double getValue(const char x) 
{
  int i;
  //find entry
  for (i=0; i<commandLength; i++) {
    if (command_list[i].type == x) 
      break;
  }
  //did we find or run out?
  if (i==commandLength) 
    return 0;
 
  return command_list[i].value;
}


bool command_exists(const char x) 
{
  for (int i=0; i<commandLength; i++) {
    if (command_list[i].type == x) return 1;
  }
  return 0;
}
