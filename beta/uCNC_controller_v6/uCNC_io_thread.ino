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

#define COMMAND_SIZE 128
uint8_t command_line[COMMAND_SIZE];
uint8_t sin_count=0;
uint16_t no_data = 0;
uint8_t asleep = 0;

void clear_command_string() {
  for (int i=0; i<COMMAND_SIZE; i++) 
    command_line[i] = 0;
  sin_count = 0;
}

void handleIO()
{
    uint8_t c;

  //read in characters if we got them.
  if (Serial.available() > 0)   {
    c = (uint8_t)Serial.read();
    no_data = 0;
    asleep = 0;
    command_line[sin_count++] = c;
  }
  else {
    no_data++;
  }

  if (sin_count && (c == '\n' || no_data > 100)) {
    command_line[sin_count] = 0;
    process_command(command_line);
    clear_command_string(); 
  }

  if (no_data == 60000)  {
    if (!asleep) {
      powerdown();
      asleep=1;
    }
  }
}
