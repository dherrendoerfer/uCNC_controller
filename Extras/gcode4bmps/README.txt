 *
 * This file is part of uCNC_controller.
 *
 * Copyright (C) 2017  D.Herrendoerfer
 *
 *   gcode4bmps.py is free software: you can redistribute it and/or modify
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
 *
 
gcode4bmps.py:
--------------

Gcode for bitmaps is a very simple tool to transform bitmaps to gcode
for raster engraving. (Engraving line by line, not vector-style)
Images read are converted to grayscale and then interpreted as black
and white (trip value 128).

Installation:
-------------

Gcode4bmps.py was developed for python2.7. It also requires the
prior installation of the python-pillow library and it's development
package (python-pillow-devel).
Futhermore, to read and interpret ps, eps, and pdf files, the ghostscript
package must be installed. (This was only tested on Linux, but should work
with Windows too.)

Usage:
------

Basic usage:

./gcode4bmps.py  -d 300 -i test/tux-bw.gif

This will read the image and generate g-code to engrave it. The output
will be written to output.nc in the current directory (default, unless specified)
You need to supply at least one of:
	- Output size with -x and -y (in mm)
	- Source image DPI with -d 
	- Source image DPCM (dots per cm) with -m 

You can specify the output file name with -g ( '-g -' for standard out, implies using -q )

Using postscript or pdf:
------------------------

/gcode4bmps.py -G -C -d 300 -i test/tux-bw.pdf

Use the -G option to turn on ghostscript transcoding, and also use
the -C option to crop the resulting image (ghostscript output is always a page)


Options available:

Usage: gcode4bmps.py [options]

Options:
  -h, --help            				show this help message and exit
  -i FILE, --imgfile=FILE				read image from this FILE
  -g FILE, --gcodefile=FILE				write gcode to this FILE
  -s STEPSIZE, --stepsize=STEPSIZE		stepsize of a single output pixel/dot (bit/laser size)
  -x XSIZE, --xsize=XSIZE				x size of the output gcode in mm
  -y YSIZE, --ysize=YSIZE				y size of the output gcode in mm
  -d DPI, --dpi=DPI     				source image dpi value
  -m DPCM, --dpcm=DPCM  				source image dpcm value
  -n, --negative        				invert the image
  -p PASSES, --passes=PASSES			number of passes over a line
  -u, --upwards         				process from bottom to top
  -z ZDOWN, --zdown=ZDOWN				value for Z axis down
  -Z ZUPPER, --zupper=ZUPPER			value for Z axis up
  -b, --bidirectional   				engrave in both directions
  -B, --backlash        				compensate for backlash in unidirectional engraving
  -f FEED1, --feed1=FEED1				set feedrate for G1 moves
  -F FEED0, --feed0=FEED0				set feedrate for G0 moves
  -M speed, --spindle=speed             enable insertion of spindle start and stop with selected speed
  -C, --crop            				crop the image to only plot the used area
  -G, --ghostscript     				use ghostscript to import postscript image files
  -q, --quiet           				don't print status messages to stdout


  						