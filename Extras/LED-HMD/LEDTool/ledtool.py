#!/usr/bin/python

'''
 * This file is part of uCNC_controller.
 *
 * Copyright (C) 2017  D.Herrendoerfer
 *
 *   ledtool.py is free software: you can redistribute it and/or modify
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
'''

from PIL import Image
import os
import sys
from optparse import OptionParser

versionstring = "v0.1-alpha"

gheight = 26
geom = [ 10,8,6,5,4,3,2,2,1,1,1,0,0,0,0,1,1,1,2,2,3,4,5,6,8,10 ]
glen = [ 6,10,14,16,18,20,22,22,24,24,24,26,26,26,26,24,24,24,22,22,20,18,16,14,10,6 ]

options = {}

def main(argv):
    global versionstring
    global options
    # Main function everything starts here
    parser = OptionParser()
    parser.add_option("-i", "--imgfile", dest="imagefile", 
                      help="read image from this FILE", metavar="FILE")
    
    (options, args) = parser.parse_args()

    if options.imagefile == None:
        print "You need to (at least) specify an image file."
        print "Try " +argv[0]+ " -h" 
        sys.exit(1)

    imgfilename, imgfile_extension = os.path.splitext(options.imagefile)
    
    im = Image.open(options.imagefile)
    
    print " - format " + im.format
    print " - size   " , im.size
    print " - mode   " , im.mode

    if im.size != (26,26):
        newsize = 26,26
        im=im.resize(newsize,Image.NEAREST)
    
    if im.mode != "RGBA":
        im = im.convert("RGBA")

    ximgsize,yimgsize = im.size
    
    px = im.load()

    print "/* "+options.imagefile+" */"
    sys.stdout.write("const PROGMEM unsigned char image[]={")

    for line in range(0, gheight):
        for pixelcolumn in range(geom[line], glen[line] + geom[line]):
            red,green,blue,alpha = px[pixelcolumn,line]
            if alpha < 255:
                sys.stdout.write("0, 0, 0, ")
            else:
                sys.stdout.write(str(red))
                sys.stdout.write(", ")
                sys.stdout.write(str(green))
                sys.stdout.write(", ")
                sys.stdout.write(str(blue))
                sys.stdout.write(", ")
    sys.stdout.write("};\n");

    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv)
