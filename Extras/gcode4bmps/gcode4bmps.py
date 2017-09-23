#!/usr/bin/python

'''
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
'''

from PIL import Image
import os
import sys
from optparse import OptionParser

versionstring = "v0.97-alpha"
lastypos = 0.0

commentstring = ";"
options = {}

def engraver_down(xpos,ypos,depth,target):
    global options
    global lastypos
    if lastypos != ypos:
        target.write("G0 X%0.4f Y%0.4f\n" % (xpos,ypos))
    else:
        target.write("G0 X%0.4f\n" % (xpos))
    if options.engraver:
        target.write("M03\n")
    else:
        target.write("G1 Z%0.2f\n" % (float(depth)))
    lastypos = ypos

def engraver_up(xpos,ypos,depth,target):
    global options
    global lastypos
    if lastypos != ypos:
        target.write("G1 X%0.4f Y%0.4f\n" % (xpos,ypos))
    else:
        target.write("G1 X%0.4f\n" % (xpos))
    if options.engraver:
        target.write("M05\n")
    else:
        target.write("G1 Z%0.2f\n" % (float(depth)))
    lastypos = ypos


def engraver_log(text,target):
    target.write("%s %s\n" % (commentstring,text))

def engraver_g0f(xpos,ypos,feedrate,target):
    global lastypos
    target.write("G0 X%0.4f Y%0.4f F%d\n" % (xpos,ypos,int(feedrate)))
    lastypos = ypos

def engraver_g0(xpos,ypos,target):
    global lastypos
    target.write("G0 X%0.4f Y%0.4f\n" % (xpos,ypos))
    lastypos = ypos

def engraver_g1f(xpos,ypos,feedrate,target):
    global lastypos
    target.write("G1 X%0.4f Y%0.4f F%d\n" % (xpos,ypos,int(feedrate)))
    lastypos = ypos

def engraver_m3s(spindle,target):
    global options
    if options.engraver:
        target.write("M5 S%d\n" % (int(spindle)))
    else:
        target.write("M3 S%d\n" % (int(spindle)))

def engraver_m5(target):
    target.write("M5\n")

def generate_cutout(xsize,ysize,zdown,zupper,stepsize,depth,target):
    pos0x = 0 - 3 * float(stepsize)
    pos0y = 0 - 3 * float(stepsize)
    
    pos1x = xsize + 3 * float(stepsize)
    pos1y = ysize + 3 * float(stepsize)
    loops = int(float(depth) / float (zdown))

    pos0z = 0

    target.write("G0 X%0.4f Y%0.4f\n" % (pos0x,pos0y))

    for z in range(0,loops):
        pos0z += float(zdown)
        target.write("G1 Z%0.2f\n" % (float(pos0z)))

        target.write("G1 X%0.4f\n" % (pos1x))
        target.write("G1 Y%0.4f\n" % (pos1y))
        target.write("G1 X%0.4f\n" % (pos0x))
        target.write("G1 Y%0.4f\n" % (pos0y))

    target.write("G1 Z%0.2f\n" % (float(zupper)))

def getcroparea(px,ximgsize,yimgsize):
    x1bound = ximgsize
    x2bound = 0
    y1bound = yimgsize
    y2bound = 0
    
    for x in range(0,ximgsize):
        for y in range(0,yimgsize):
            if px[x,y] != 255:
                if x < x1bound:
                    x1bound = x
                if x > x2bound:
                    x2bound = x
                if y < y1bound:
                    y1bound = y
                if y > y2bound:
                    y2bound = y
                    
    if x1bound > 20:
        x1bound -= 10
    if y1bound > 20:
        y1bound -= 10
    if x2bound < (ximgsize - 20):
        x2bound += 10
    if y2bound < (yimgsize - 20):
        y2bound += 10
        
    return x1bound,x2bound,y1bound,y2bound

def main(argv):
    global versionstring
    global options
    # Main function everything starts here
    parser = OptionParser()
    parser.add_option("-i", "--imgfile", dest="imagefile", 
                      help="read image from this FILE", metavar="FILE")
    parser.add_option("-g", "--gcodefile", dest="gcodefile", default="output.nc",
                      help="write gcode to this FILE", metavar="FILE")
    parser.add_option("-s", "--stepsize", dest="stepsize", default=0.1,
                      help="stepsize of a single output pixel/dot (envraver/laser size) in mm")
    parser.add_option("-x", "--xsize", dest="xsize", default=0,
                      help="x size of the output gcode in mm")
    parser.add_option("-y", "--ysize", dest="ysize", default=0, 
                      help="y size of the output gcode in mm")
    parser.add_option("-d", "--dpi", dest="dpi", default=0, 
                      help="source image dpi value")
    parser.add_option("-m", "--dpcm", dest="dpcm", default=0, 
                      help="source image dpcm value")
    parser.add_option("-n", "--negative",
                      action="store_true", dest="negative", default=False,
                      help="invert the image")
    parser.add_option("-p", "--passes", dest="passes", default=1, 
                      help="number of passes over a line")
    parser.add_option("-u", "--upwards",
                      action="store_true", dest="upwards", default=False,
                      help="process from bottom to top")
    parser.add_option("-z", "--zdown", dest="zdown", default=-1, 
                      help="value for Z axis down")
    parser.add_option("-Z", "--zupper", dest="zupper", default=1, 
                      help="value for Z axis up")
    parser.add_option("-b", "--bidirectional",
                      action="store_true", dest="bidirectional", default=False,
                      help="engrave in both directions")
    parser.add_option("-B", "--backlash",
                      action="store_true", dest="backlash", default=False,
                      help="compensate for backlash in unidirectional engraving")
    parser.add_option("-f", "--feed1", dest="feed1", default=0,
                      help="set feedrate for G1 moves")
    parser.add_option("-F", "--feed0", dest="feed0", default=0,
                      help="set feedrate for G0 moves")
    parser.add_option("-M", "--spindle", dest="spindle", default=0,
                      help="enable insertion of spindle start and stop with selected speed")
    parser.add_option("-C", "--crop",
                      action="store_true", dest="crop", default=False,
                      help="crop the image to only plot the used area")
    parser.add_option("-G", "--ghostscript",
                      action="store_true", dest="ghostscript", default=False,
                      help="use ghostscript to import postscript image files")
    parser.add_option("-D", "--gsDPI", dest="gsdpi", default=200,
                      help="DPI to use for ghostscript imported postscript image files")
    parser.add_option("-U", "--useSforDPI", 
                      action="store_true", dest="usedpi", default=False,
                      help="Calculate DPI to use for ghostscript from engraver size")
    parser.add_option("", "--cutout", dest="cutout", default=0,
                      help="Generate a cutout frame around the image with given depth in mm")
    parser.add_option("", "--engraver", action="store_true", dest="engraver", default=False,
                      help="Generate output for a laser engraver, M codes turn the laser on and off")

    parser.add_option("-q", "--quiet",
                      action="store_false", dest="verbose", default=True,
                      help="don't print status messages to stdout")
    
    (options, args) = parser.parse_args()

    if options.verbose:
        print "gcode4bmps.py (C)2017 D.Herrendoerfer"
        print "Version: " + str(versionstring)
    
    if options.imagefile == None:
        print "You need to (at least) specify an image file."
        print "Try " +argv[0]+ " -h" 
        sys.exit(1)

    if options.usedpi:
        options.gsdpi=str(int(round( 25.4 / float(options.stepsize))))
 
    imgfilename, imgfile_extension = os.path.splitext(options.imagefile)
    
    if imgfile_extension in '.ps' '.eps' '.pdf':
        if not options.ghostscript:
            print "You need to use ghostscript to get the image data first."
            print "Or you can try to use the -G option if ghostscript is installed."
            print imgfile_extension
            sys.exit(1)
        else:
            retval = os.system("gs -o tmp_output.png -sDEVICE=png256 -r"+str(options.gsdpi)+" "+options.imagefile+" &> /dev/null")
            if retval != 0:
                print "using ghostscript failed. Cannot continue !"
                sys.exit(1)
            else:
                options.imagefile = "tmp_output.png"
                options.dpi = options.gsdpi
 
    if options.verbose:
        print "+ Opening image file " + options.imagefile + "." 

    im = Image.open(options.imagefile)
    
    if options.verbose:
        print " - format " + im.format
        print " - size   " , im.size
        print " - mode   " , im.mode
    
    ximgsize,yimgsize = im.size
    
    if im.mode != "L":
        if options.verbose:
            print "+ Converting image to grayscale"
        im = im.convert("L")

    if options.verbose:
        print "+ Starting work on pixels." 
    px = im.load()

    if options.crop:
        if options.verbose:
            print "+ Cropping the active area"
        
        x1bound,x2bound,y1bound,y2bound = getcroparea(px,ximgsize,yimgsize)
        
        if options.verbose:
            print " - Crop suggests : X1:",x1bound
            print " -               : X2:",x2bound
            print " -               : Y1:",y1bound
            print " -               : Y2:",y2bound
            print " - Done selecting crop are"
        
        box = (x1bound,y1bound,x2bound,y2bound)
        im = im.crop(box)
        if options.verbose:
            print " - New image size:",im.size
        
        ximgsize,yimgsize = im.size
        px = im.load()
    
    if options.dpi != 0:
        xpixelwidth = 25.4 / float(options.dpi)
        ypixelwidth = 25.4 / float(options.dpi)
        xsize = ximgsize * xpixelwidth
        ysize = yimgsize * ypixelwidth
    elif options.dpcm != 0:
        xpixelwidth = 10.0 / float(options.dpcm)
        ypixelwidth = 10.0 / float(options.dpcm)
        xsize = ximgsize * xpixelwidth
        ysize = yimgsize * ypixelwidth
    elif options.xsize > 0 and options.ysize > 0:
        xpixelwidth = float(options.xsize) / ximgsize
        ypixelwidth = float(options.ysize) / yimgsize
        xsize = float(options.xsize)
        ysize = float(options.ysize)
    else:
        print "No output size or resolution info - can't continue"
        sys.exit(1)
    
    xlines = xsize / float(options.stepsize)
    ylines = ysize / float(options.stepsize)
    
    if options.verbose:
        print " - pixelwidth X :" ,xpixelwidth, "mm"
        print " -            Y :" ,ypixelwidth, "mm"
        print " - outputsize X :" ,xsize, "mm"
        print " -            Y :" ,ysize, "mm"
        print " - lines      X :" ,xlines
        print " -            Y :" ,ylines
    
    if options.upwards:
        xpos = 0
        ypos = 0
        yinc = float(options.stepsize) 
    else:
        xpos = 0
        ypos = ysize
        yinc = -1 * float(options.stepsize) 

    if options.gcodefile != "-":
        target = open(options.gcodefile, 'w')
        target.truncate()      
    else:
        target = sys.stdout  

    engraver_log("created by gcode4bmps.py (C)2017 D.Herrendoerfer", target)
    engraver_log("version: "+str(versionstring), target)
    engraver_log("", target)
    engraver_log("Settings:", target)
    invocation = ""
    for tmp in argv:
        invocation += tmp + " "
    engraver_log("Invoked as:", target)
    engraver_log("", target)
    engraver_log("  gcode4bmps.py " + invocation, target)
    engraver_log("", target)
    engraver_log("Image information:", target)
    engraver_log("  Size in pixels X:" + str(ximgsize), target)
    engraver_log("                 Y:" + str(yimgsize), target)
    engraver_log("Output information:", target)
    engraver_log("  Size in mm     X:" + str(xsize), target)
    engraver_log("                 Y:" + str(ysize), target)
    engraver_log("  Lines          Y:" + str(int(ylines)), target)
    
    engraver_log("", target)

    target.write("G1 Z%0.2f\n" % (float(options.zupper)))

    if options.feed0 != 0:
        engraver_g0f(0, 0, options.feed0, target)
    if options.feed1 != 0:
        engraver_g1f(0, 0, options.feed1, target)
    if options.spindle != 0:
        engraver_m3s(options.spindle, target)
        
    engraver_g0(0, ypos, target)

    for line in range(0, int(ylines)):
        if options.upwards:
            pixline = yimgsize - 1 - int((yimgsize / ylines) * line ) 
        else:
            pixline = int((yimgsize / ylines) * line ) 
        
        for passes in range(0, int(options.passes)):
            engraver_log("line:"+str(line+1)+" pass:"+str(passes+1) + " pixline:" + str(pixline), target)
            if options.negative:
                #engraver_down(0, ypos, options.zdown, target)
                lastpixval = 0
            else:
                lastpixval = 255
            
            if options.bidirectional:
                if (line % 2) == 0:
                    pstart = 0
                    pend = ximgsize
                    pincrement = 1
                else:
                    pstart = ximgsize -1
                    pend = 0
                    pincrement = -1
            else:
                pstart = 0
                pend = ximgsize
                pincrement = 1
                if options.backlash:
                    engraver_g0(-1, ypos, target)
             
            lastpixelcolumn=pstart
                              
            for pixelcolumn in range(pstart, pend, pincrement):
                pixval = px[pixelcolumn,pixline]
                if pixval > 128:
                    pixval = 255
                else:
                    pixval = 0
                
                if pixval != lastpixval:
                    if options.negative:
                        if pixval == 0:
                            engraver_up(lastpixelcolumn*xpixelwidth, ypos, options.zupper, target)
                        else:
                            engraver_down(pixelcolumn*xpixelwidth, ypos, options.zdown, target)
                    else:
                        if pixval == 255:
                            engraver_up(lastpixelcolumn*xpixelwidth, ypos, options.zupper, target)
                        else:
                            engraver_down(pixelcolumn*xpixelwidth, ypos, options.zdown, target)
                
                lastpixval = pixval
                lastpixelcolumn = pixelcolumn
            
            if options.negative:
                if lastpixval == 255:
                    engraver_up(pixelcolumn*xpixelwidth, ypos, options.zupper, target)
            else:
                if lastpixval == 0:
                    engraver_up(pixelcolumn*xpixelwidth, ypos, options.zupper, target)
            
        ypos = ypos + yinc

    if options.cutout != 0:
        engraver_log("cutout", target)
        generate_cutout(xsize,ysize,options.zdown,options.zupper,options.stepsize,options.cutout,target)
        
    if options.spindle != 0:
        engraver_log("spindle off", target)
        engraver_m5(target)

    if target != sys.stdout:
        engraver_log("end", target)
        target.close()

    if options.verbose:
        print "+ done !"
        print " - generated", line+1, "lines"

    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv)
