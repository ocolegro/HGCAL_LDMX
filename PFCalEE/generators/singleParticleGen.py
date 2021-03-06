#!/usr/bin/env python

import math
import random
import argparse
import os

from time import strftime 

usage = "usage: %prog [arguments]"
parser = argparse.ArgumentParser(usage)
parser.add_argument("-r", "--particle"   , dest="particle", help="choose particle PDG or name"  , default=-1)
parser.add_argument("-n", "--nevts"      , dest="nevts"   , help="number of events"             , default=100, type=int)
parser.add_argument("-m", "--multiple"   , dest="multiple", help="multiple files"               , default=1, type=int)
parser.add_argument("-e", "--energy"     , dest="energy"  , help="energy of events in GeV"      , default=4  , type=float)
parser.add_argument("-p", "--phi"        , dest="phi"     , help="set phi angle in degrees"     , default=0  , type=float)
parser.add_argument("-t", "--theta"      , dest="theta"   , help="set theta angle in degrees"   , default=0  , type=float)
parser.add_argument("-x", "--xcoordinate", dest="x"       , help="x-coordinate"                 , default=0  , type=float)
parser.add_argument("-y", "--ycoordinate", dest="y"       , help="y-coordinate"                 , default=0  , type=float)
parser.add_argument("-z", "--zcoordinate", dest="z"       , help="z-coordinate"                 , default=-50, type=float)
parser.add_argument("-o", "--outputdir"  , dest="outDir"  , help="ouput directory"              , default=os.getcwd())
arg = parser.parse_args()

particleID = -1
outDir = arg.outDir
#Check for trailing slash on ouput dir and delete
if arg.outDir.split("/")[-1] == "": outDir = arg.outDir[:-1]

if not os.path.isdir(outDir):
    print "Output directory does not exist!"
    quit()

#Determine which particle the user selected
if arg.particle == "11" or arg.particle == "electron":
    particleID  = 11
    fileoutBase = "electrons"
    outDir      = outDir+"/electrons"
    KineticE    = arg.energy - 0.000510999
elif arg.particle == "13" or arg.particle == "muon":
    particleID  = 13
    fileoutBase = "muons"
    outDir      = outDir+"/muons"
    KineticE    = arg.energy - 0.10565837
elif arg.particle == "111" or arg.particle == "pi0":
    particleID  = 111
    fileoutBase = "pi0s"
    KineticE    = arg.energy - 0.1349766
    outDir      = outDir+"/pions/pi0s"
elif arg.particle == "211" or arg.particle == "pi+":
    particleID  = 211
    fileoutBase = "pis"
    KineticE    = arg.energy - 0.1395702
    outDir      = outDir+"/pions/pis"
elif arg.particle == "2112" or arg.particle == "neutron":
    particleID  = 2112
    fileoutBase = "neutrons"
    KineticE    = arg.energy - 0.9395654
    outDir      = outDir+"/neutrons"
elif arg.particle == "-2112" or arg.particle == "antineutron":
    particleID  = -2112
    fileoutBase = "antineutrons"
    KineticE    = arg.energy - 0.9395654
    outDir      = outDir+"/antineutrons"
elif arg.particle == "130" or arg.particle == "KL":
    particleID  = 130
    fileoutBase = "KLs"
    KineticE    = arg.energy - 0.497648
    outDir      = outDir+"/KLs"
elif arg.particle == "22" or arg.particle == "photon":
    particleID  = 22
    fileoutBase = "photons"
    KineticE    = arg.energy
    outDir      = outDir+"/photons"

# Check to see that particle has changed
if particleID == -1:
    print "\nInvalid particle selected.\n"
    print "Usage can include:\n"
    print "11    or electron"
    print "13    or muon"
    print "111   or pi0"
    print "211   or pi+"
    print "2112  or neutron"
    print "-2112 or antineutron"
    print "130   or KL"
    print "22    or photon\n"
    quit()

if (arg.multiple > 1):

    for j in range(0,arg.multiple):

        lhefile = open("%s/%d_%gGeV_%d_phi%g_theta%g_x%g_y%g_z%g_%s.lhe"%(outDir,arg.nevts,arg.energy,j,arg.phi,arg.theta,arg.x,arg.y,arg.z,fileoutBase), "w")
        lhefile.write("<header>\n")
        lhefile.write("This file contains primary %s.\n"%(fileoutBase))
        lhefile.write("Do not edit this file manually.\n")
        lhefile.write("File created on %s at %s\n"%(strftime("%Y-%m-%d"),strftime("%H:%M:%S")))
        lhefile.write("</header>\n")

        px = math.sin(math.radians(arg.theta))*math.cos(math.radians(arg.phi))
        py = math.sin(math.radians(arg.theta))*math.sin(math.radians(arg.phi))
        pz = math.cos(math.radians(arg.theta))

        for i in range(0,arg.nevts):

            lhefile.write("<event>\n")

            lhefile.write("%s %g %g %g %g %g %g %g\n"%(particleID,arg.x,arg.y,arg.z,px,py,pz,KineticE))

            lhefile.write("</event>\n")

        lhefile.close()
else:

    lhefile = open("%s/%d_%gGeV_phi%g_theta%g_x%g_y%g_z%g_%s.lhe"%(outDir,arg.nevts,arg.energy,arg.phi,arg.theta,arg.x,arg.y,arg.z,fileoutBase), "w")
    lhefile.write("<header>\n")
    lhefile.write("This file contains primary %s.\n"%(fileoutBase))
    lhefile.write("Do not edit this file manually.\n")
    lhefile.write("File created on %s at %s\n"%(strftime("%Y-%m-%d"),strftime("%H:%M:%S")))
    lhefile.write("</header>\n")

    px = math.sin(math.radians(arg.theta))*math.cos(math.radians(arg.phi))
    py = math.sin(math.radians(arg.theta))*math.sin(math.radians(arg.phi))
    pz = math.cos(math.radians(arg.theta))

    for i in range(0,arg.nevts):

        lhefile.write("<event>\n")

        lhefile.write("%s %g %g %g %g %g %g %g\n"%(particleID,arg.x,arg.y,arg.z,px,py,pz,KineticE))

        lhefile.write("</event>\n")

    lhefile.close()

