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


/*Settings */
boolean abs_mode = true;

float g0_feedrate = 100.0; /* mm per minute*/
float g1_feedrate = 100.0; /* mm per minute*/

#define DEBUG 1

struct f_PosXYZ {
  float x;
  float y;
  float z;
};

void setXYZ(struct f_PosXYZ *fp) 
{
  fp->x = (command_exists('X')) ? (getValue('X') + ((abs_mode) ? 0 : posX)) : posX;
  fp->y = (command_exists('Y')) ? (getValue('Y') + ((abs_mode) ? 0 : posY)) : posY;
  fp->z = (command_exists('Z')) ? (getValue('Z') + ((abs_mode) ? 0 : posZ)) : posZ;
}

void process_command(uint8_t *command_string)
{
  uint8_t code;
  uint16_t k;
  float temp;
  f_PosXYZ fp;
  unsigned int lineNo = 0;


  //the character / means delete block... used for comments and stuff.
  if (command_string[0] == '/') {
    Serial.println("ok");
    return;
  }

  purge_commands(); //clear old commands
  parse_commands(command_string); //create linked list of arguments

  if (command_exists('N')) 
    lineNo = getValue('N');

  if (command_exists('G')) {
    code = getValue('G');

    switch(code) {
    case 0: //Rapid Motion
      setXYZ(&fp);
      if (command_exists('F')) g0_feedrate = getValue('F'); //feedrate persists till changed.
      

//      jumpPosXYZ (fp.x, fp.y, fp.z);
      break;
    case 1: //Coordinated Motion
      setXYZ(&fp);
      if (command_exists('F')) g1_feedrate = getValue('F'); //feedrate persists till changed.
//      movePosXYZ (fp.x, fp.y, fp.z, _feedrate);
      break;   
    case 4: //Dwell
      delay((int)getValue('P'));
      break;
    case 20: //Inches for Units
      conversionFactor = 25.4;  // 1 for mm 25.4 for inches
      break;
    case 21: //mm for Units
      conversionFactor = 1;  // 1 for mm 25.4 for inches      
      break;
    case 28: //go home.
//      homeXYZ();
      break;
    case 30://go home via an intermediate point.
      setXYZ(&fp);
 //     movePosXYZ (fp.x, fp.y, fp.z, 0);
 //     movePosXYZ (0, 0, 0, 0);
      break;  
    case 90://Absolute Positioning
      abs_mode = true;
      break;
    case 91://Incremental Positioning
      abs_mode = false;
      break;
    case 92://Set as home
      resetPosXYZ();
      break;
    case 93://Inverse Time Feed Mode
      break;  //TODO: add this
    case 94://Feed per Minute Mode
      break;  //TODO: add this
    default:
      Serial.print("huh? G");
      Serial.println(code,DEC);
      return;
    }
  }

  if (command_exists('T')) {
    tool = getValue('&');
    updateToolCodes();
  }
  
  if (command_exists('M')) {
    code = getValue('M');

    switch(code) {
    case 3:
      if (command_exists('S')) spindleSpeed = (int)getValue('S');
      spindle=1;
      break;
    case 4:
      if (command_exists('S')) spindleSpeed = (int)getValue('S');
      spindle=2;
      break;
    case 5:
      spindle=0;
      break;
    case 7:
    case 8:
      coolant1=1;
      break;
    case 9:
      coolant1=0;
      break;
    case 10:
      coolant2=1;
      break;
    case 11:
      coolant2=0;
      break;
    case 18:
      powerdown();
      break;
    }
    updateMotorCodes();
  }
  
  Serial.println("ok");
}
