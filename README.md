Copyright (C) 2014,2015,2016 D.Herrendoerfer 

uCNC_controller
===============

A small Arduino CNC controller

uCNC_controller is a small CNC G-Code interpreting controller for educational or home projects.
It supports small set of commands and can be configured to behave in several different ways.

Included in the Extras directory are sample files to replicate a controller circuit board and 3D files to build a small 3 axis CNC plotter.

It controls up to 3 stepper motors via the Arduino stepper library, as well as one servo, and two general purpose outputs.

Please read the included uCNC_controller/README.txt file for more information.

The beta directory contains pre-release versions of the controller firmware.

To build the uCNC_controller firmware the Arduino stepper library is required.

The firmware requires a 5V Arduino board with at least 13 digital and 6 Analog I/O ports.
Recommended are 32k of flash memory.
Among the supported boards are: Arduino Uno, Arduino Nano, Arduino Mega 2560, and many more.
