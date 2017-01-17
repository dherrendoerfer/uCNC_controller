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



class Servo
{
  public:
    // constructor:
    Servo(int);
    Servo(int, volatile long *val);

    int attach(volatile long *val); // as above but also sets min and max values for writes.
    void detach();

    void stop();
    void start();

    
    bool attached(); // return true if this servo is attached
    bool running();
    
  private:
//    uint8_t chanIndex; // index into the channel data for this servo
};
