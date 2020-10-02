#!/usr/bin/python

# This program is free to use or modify
# There is no warranty
# If you make improvements and send me those improvement I'll at least send a thank you. :)
# Version 0.5   (I got to start somewhere)
# cllsjd at gmail  Hopefully you can figure out how make this into an address that really works.

import tkinter
from tkinter import *
from tkinter import messagebox
from tkinter import filedialog
import serial
import time
import os

# set jog distances
X_jog= 0.5
Y_jog= 0.5
Z_jog= 0.1


# get a serial instance and open it later
ser = serial.Serial()
ser.baudrate = 9600
ser.port = "COM6"
ser.timeout = 1

class GUIFramework(Frame):
    """This is the GUI"""

# make the variables gobal
    filename = ''
    serialClosed = True
    unit = 'mm'

    X= 0.0
    Y= 0.0
    Z= 0.0

    def __init__(self,master=None):
        """Initialize yourself"""

        """Initialise the base class"""
        Frame.__init__(self,master)

        """Set Window Title"""
        self.master.title("Arduino G Code Driver")

        """Display the main window with a little bit of padding"""
        self.grid(padx=10,pady=10, sticky=E+N)
        self.CreateWidgets()

    def checkSum(self, cline):
#this is the same calculation the arduino is doing.  The arduino is expecting the G command
#followed by a '*' and the csum value
        csum = 0
        for i in range(0,len(cline),1):
            csum = csum ^ ord(cline[i])
        return csum

    def connectSerial(self):
        ser.open()
        time.sleep(2)
# if the serial port is not open attempt to connect.  The arduino takes at least 2 seconds
# to reset itself.  The arduino replies with "start".
        if  self.serialClosed:
            reply = ""
            counter = 0
            while not 'ready' in reply or 'start' in reply:
                reply = ser.readline()
                time.sleep(1)
                counter += 1
                if counter == 10:
                    return
            if 'start' or 'ready' in reply:
                self.printJogLine('Connection to Arduino established\n')
                self.serialClosed = False
                if self.unit == 'inch':
                    self.setInch()
                else:
                    self.setmm()
            else:
                self.printJogLine('Not Connected\n')
                time.sleep(2)
        #self.serialClosed = True

    def sendLine(self, sline):
# send one line to the arduino
        if not ser.isOpen():
# if serial port is not open - open the serial port by calling connectSerial function
            self.connectSerial()
        jog_line = "Send - " + sline
        self.printJogLine(jog_line)
# print the line
        ser.write(sline)
# send the line on the serial port

    def writeline(self, wline):
# prepare one line to send to the arduino - pass it to the sendline function
# then read what come back from the arduino
        # print 'Made it to writeline: ' + wline
        OKAY = False
        wline = wline.strip()
        wline = wline +  '*' + str(self.checkSum(wline)) + '\n'
        # print 'Send it to sendLine: ' + wline
#this line is what the arduino is looking for - command plus * plus checksum
        self.sendLine(wline)
        while not OKAY:
            wRespond = ser.readline()
            if 'ok' in wRespond:
                jog_line = 'Line Processed' + '\n'
                self.printJogLine(jog_line)
# the arduino processed the line and wants another line
                OKAY = True
            elif wRespond == '':
                time.sleep(1)
# no responence wait and try again
            elif '~' in wRespond:
                pass
# the arduino is trying to keep the connection alive
            elif 'rp' in wRespond:
                jog_line = 'Line repeated: Line Ignored'
                self.printJogLine(jog_line)
                OKAY = True
# the last line was a repeat so ignore it and go on
            elif 'rs' in wRespond:
                jog_line = 'Checksum Failed: Line Resent'
                self.printJogLine(jog_line)
                wRespond = self.sendLine(wline)
                OKAY = False
# the checksum failed resend the line and start check again
            else:
# didn't get a return that the program knew what to do with
# will resend the last line
                jog_line = 'Huh....' + wRespond
                self.printJogLine(jog_line)
                jog_line = 'Resend?'
                self.printJogLine(jog_line)
                wRespond = self.sendLine(wline)
                OKAY = False

#routine to print the jog line
    def printJogLine(self,jogLine):
    #1.0 means to insert at line 1 char 0
        self.feedback.insert(1.0, jogLine)
        self.update_idletasks()


#**********************************************************************************
# Deal with the buttons
#**********************************************************************************

# write the file to the arduino when the run button is pushed
    def writefile(self):
        for line in open(self.filename, 'r'):
            line = line.strip()
            self.writeline(line)
        #Move out
        jog_line = 'G0 Z1.0000 \n'
        self.writeline(jog_line)

# open file button command
    def getFileName(self):
        self.filename = filedialog.askopenfilename(filetypes=[("allfiles","*")])
##        print self.filename
        self.fileBox.delete(1.0,'end')
        self.fileBox.insert('end', open(self.filename,'r').read())

# set units ########################################################
    def setmm(self):
        if self.unit == 'inch':
            self.X *= 25.4
            self.Y *= 25.4
            self.Z *= 25.4
        self.unit = 'mm'
        unitLine = 'G21 \n'
        self.writeline(unitLine)

    def setInch(self):
        if self.unit == 'mm':
            self.X /= 25.4
            self.Y /= 25.4
            self.Z /= 25.4
        self.unit = 'inch'
        unitLine = 'G20 \n'
        self.writeline(unitLine)

#####################################################################

######################### connectSerial #############################
    def conSerial(self):
        self.connectSerial()

######################################################################
#  Still can't get the jog buttons to work in a consistent manner
#  kind of frustrating but.....
######################################################################

# X- button command
    def run_jogXneg(self):
        self.X -= float(self.xEntry.get())
        jog_line = 'G1 X' + str(self.X) + ' \n'
        self.writeline(jog_line)

# X+ button command
    def run_jogXpos(self):
        self.X += float(self.xEntry.get())
        jog_line = 'G1 X' + str(self.X) + ' \n'
        self.writeline(jog_line)

# Y- button command
    def run_jogYneg(self):
        self.Y -= float(self.yEntry.get())
        jog_line = 'G1 Y' + str(self.Y) + ' \n'
        self.writeline(jog_line)

# Y+ button command
    def run_jogYpos(self):
        self.Y += float(self.yEntry.get())
        jog_line = 'G1 Y' + str(self.Y) + ' \n'
        self.writeline(jog_line)

# Z- button command
    def run_jogZneg(self):
        self.Z -= float(self.zEntry.get())
        jog_line = 'G1 Z' + str(self.Z) + ' \n'
        self.writeline(jog_line)

# Z+ button command
    def run_jogZpos(self):
        self.Z += float(self.zEntry.get())
        jog_line = 'G1 Z' + str(self.Z) + ' \n'
        self.writeline(jog_line)

# X- Y- button command
    def run_jogXYnn(self):
        self.X -= float(self.xEntry.get())
        self.Y -= float(self.yEntry.get())
        jog_line = 'G1 X' + str(self.X) + ' Y' + str(self.Y) + ' \n'
        self.writeline(jog_line)

# X- Y+ button command
    def run_jogXYnp(self):
        self.X -= float(self.xEntry.get())
        self.Y += float(self.yEntry.get())
        jog_line = 'G1 X' + str(self.X) + ' Y' + str(self.Y) + ' \n'
        self.writeline(jog_line)

# X+ Y- button command
    def run_jogXYpn(self):
        self.X += float(self.xEntry.get())
        self.Y -= float(self.yEntry.get())
        jog_line = 'G1 X' + str(self.X) + ' Y' + str(self.Y) + ' \n'
        self.writeline(jog_line)

# X+ Y+ button command
    def run_jogXYpp(self):
        self.X += float(self.xEntry.get())
        self.Y += float(self.yEntry.get())
        jog_line = 'G1 X' + str(self.X) + ' Y' + str(self.Y) + ' \n'
        self.writeline(jog_line)

########### Set Zero #################
    def setZero(self):
        self.X = 0.0
        self.Y = 0.0
        self.Z = 0.0
        jog_line = 'G92 \n'
        self.writeline(jog_line)

########### Go Home #################
    def setHome(self):
        self.X = 0.0
        self.Y = 0.0
        self.Z = 0.0
        jog_line = 'G0 Z1.0000 \n'
        self.writeline(jog_line)
        jog_line = 'G0 X0.0000 Y0.0000 \n'
        self.writeline(jog_line)
        jog_line = 'G0 Z0.000 \n'
        self.writeline(jog_line)

########### start spindle ################
    def startSpindle(self):
        jog_line = 'M03 \n'
        self.writeline(jog_line)
########### stop spindel #################
    def stopSpindle(self):
        jog_line = 'M05 \n'
        self.writeline(jog_line)
########### time to quit #################
    def qTime(self):
        sys.exit()
# ********************************************************************************
# Create the GUI
# ********************************************************************************
    def CreateWidgets(self):

# create a text box to display file
        self.fileBox=Text(self, height=25, width = 60)
        self.fileBox.grid(row=0, column=0, columnspan =4)

# create a text box to print feedback
        self.feedback = Text (self, height=25, width = 60)
        self.feedback.grid(row=0, column=4, columnspan=4)

# open file button
        self.openFile = Button(self, text = "Open File",bg = "blue", fg = "white", command=self.getFileName )
        self.openFile.grid(row=1, column=1 )

# start feeding the G code file to the arduino
        self.runFile = Button(self, text = "Run File",bg = "yellow", fg = "blue", command=self.writefile )
        self.runFile.grid(row=1, column=3 )

# set the units
        self.unitin = Radiobutton(self, text = "mm", bg = "white", value = 1, command=self.setmm)
        self.unitin.grid (row=1, column=6)
        self.unitmm = Radiobutton(self, text = "inch", bg = "white", value = 0, command=self.setInch)
        self.unitmm.grid(row=1, column=7)

# Flush serial port
        self.connser = Button(self, text = "Connect Serial",bg = "white", fg = "blue", command=self.conSerial )
        self.connser.grid(row=1, column=5 )

# X jog button
        self.jogXneg = Button(self, text = ' X-' , bg = 'orange', repeatdelay = 500, repeatinterval = 20, command=self.run_jogXneg )
        self.jogXneg.grid(row=3, column=0)
        self.jogXpos = Button(self, text = ' X+ ', bg = 'orange', repeatdelay = 500, repeatinterval = 20, command=self.run_jogXpos )
        self.jogXpos.grid(row=3, column=3)

# X- Y- jog button
        self.jogXYnn = Button(self, text = 'X-Y-', bg = 'cyan', repeatdelay = 500, repeatinterval = 20, command=self.run_jogXYnn)
        self.jogXYnn.grid(row=2, column=0)

# X- Y+ jog button
        self.jogXYnp = Button(self, text = 'X-Y+', bg = 'cyan', repeatdelay = 500, repeatinterval = 20, command=self.run_jogXYnp)
        self.jogXYnp.grid(row=2, column=3)

# Y jog button
        self.jogYneg = Button(self, text = ' Y- ', bg = 'orange', repeatdelay = 500, repeatinterval = 20, command=self.run_jogYneg)
        self.jogYneg.grid(row=2, column=1)
        self.jogYpos = Button(self, text = ' Y+ ', bg = 'orange', repeatdelay = 500, repeatinterval = 20, command=self.run_jogYpos)
        self.jogYpos.grid(row=4, column=1)

# X+ Y- jog button
        self.jogXYpn = Button(self, text = 'X+Y-', bg = 'cyan', repeatdelay = 500, repeatinterval = 20, command=self.run_jogXYpn)
        self.jogXYpn.grid(row=4, column=0)

# X+ Y+ jog button
        self.jogXYpp = Button(self, text = 'X+Y+', bg = 'cyan', repeatdelay = 500, repeatinterval = 20, command=self.run_jogXYpp)
        self.jogXYpp.grid(row=4, column=3)

# Z jog button
        self.jogZpos = Button(self, text = 'Z+', bg ='red', command=self.run_jogZpos )
        self.jogZpos.grid(row=2, column=5)
        self.jogZneg = Button(self, text = 'Z-', bg = 'red', command=self.run_jogZneg )
        self.jogZneg.grid(row=4, column=5)

# create entry for jog distance
# X jog distance
        self.xLabel = Label(self, text = '  X Jog Distance')
        self.xLabel.grid(row= 2, column=6)
        self.xEntry = Entry(self)
        self.xEntry.insert(0, X_jog)
        self.xEntry.grid ( row=2, column = 7)
# Y jog distance
        self.yLabel = Label(self, text = '  Y Jog Distance')
        self.yLabel.grid(row= 3, column=6)
        self.yEntry = Entry(self)
        self.yEntry.insert(1, Y_jog)
        self.yEntry.grid ( row=3, column = 7)

# Z jog distance
        self.zLabel = Label(self, text = '  Z Jog Distance')
        self.zLabel.grid(row= 4, column=6)
        self.zEntry = Entry(self)
        self.zEntry.insert(1, Z_jog)
        self.zEntry.grid ( row=4, column = 7)
################## Go X0 Y0 Z0 - Home ##############################
        self.goHome = Button(self, text = 'Home', bg= 'pink', command = self.setHome)
        self.goHome.grid( row = 3, column= 1)
################## start & stop spindle #############################
        self.zero = Button(self, text = 'Set Home', bg= 'pink', command = self.setZero)
        self.zero.grid( row = 5, column= 1)
        self.spindleStart = Button(self, text = 'Spindle Start', bg = 'green', command=self.startSpindle)
        self.spindleStart.grid(row = 5, column = 3, columnspan = 2)
        self.spindleStop = Button(self, text = 'Spindle Stop', bg = 'red', command=self.stopSpindle)
        self.spindleStop.grid(row=5, column=5, columnspan = 2)
################## quit program ##############################
        self.progQuit = Button(self, text = "QUIT", bg = 'orange', command=self.qTime )
        self.progQuit.grid (row = 5, column = 8)
##############################################################

if __name__ == "__main__":
    guiFrame = GUIFramework()
    guiFrame.mainloop()
