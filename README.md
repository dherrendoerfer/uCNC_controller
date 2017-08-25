Copyright (C) 2014,2015,2016 D.Herrendoerfer 

# uCNC_controller: A small Arduino CNC

uCNC_controller is a small CNC G-Code interpreting controller for educational or home projects.  
It supports small set of commands and can be configured to behave in several different ways.  
Included in the Extras directory are sample files to replicate a controller circuit board and  
3D files to build a small 3 axis CNC plotter.

### uCNC_controller:
It controls up to three stepper motors via the Arduino stepper library, as well as one servo, and  
two general purpose outputs. It is programmed for simplicity and readability more than for being  
a feature-complete g-code interpreter. It is intended to be a stepping stone for people intending  
to learn about machine control.

### The Extras:
Within the Extras directory there are tools, layout and schematic files, control firmwares to  
extend the basic use example of a simple plotter. Feel free to delve.

Please read the included **uCNC_controller/README.txt** file for more information.  
Please read the included **LICENSE** files and make sure you understand them before redistributing anything from this repo.

### More Information
The original source code of uCNC_controller was developed for this project in 2013  
<a href="http://www.youtube.com/watch?feature=player_embedded&v=rLQajSRnELc
" target="_blank"><img src="http://img.youtube.com/vi/rLQajSRnELc/0.jpg" 
alt="Direct ink to PCB CNC Plotter" width="240" height="180" border="10" /></a>

Also, feel free to check out my [YouTube Channel](https://www.youtube.com/channel/UCe9XrOQuQR8SzbB_zRUIf0w "Dirk Herrendoerfers YouTube Channel")
all the source code and project files from the projects  
featured on the channel are hosted here.

## Notes
- The beta directory contains pre-release versions of the controller firmware.
- To build the uCNC_controller firmware the Arduino stepper library is required.
- The firmware requires a 5V Arduino board with at least 13 digital and 6 Analog I/O ports.  
- Recommended are 32k of flash memory.  
- Among the supported boards are: Arduino Uno, Arduino Nano, Arduino Mega 2560, and many more.
- This is work-in-progress. Check back frequently.
