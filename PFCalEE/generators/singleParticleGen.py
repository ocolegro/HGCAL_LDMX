#!/usr/bin/env python

import math
import random
import optparse
import os

from time import strftime 

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-r', '--particle'   , dest='particle', help='choose particle PDG or name'  , default=-1)
parser.add_option('-f', '--filename'   , dest='filename', help='lhe file name'                , default='electrons')
parser.add_option('-n', '--nevts'      , dest='nevts'   , help='number of events'             , default=100, type=int)
parser.add_option('-e', '--energy'     , dest='energy'  , help='energy of events in GeV'      , default=4  , type=float)
parser.add_option('-p', '--phi'        , dest='phi'     , help='set phi angle in degrees'     , default=0  , type=float)
parser.add_option('-t', '--theta'      , dest='theta'   , help='set theta angle in degrees'   , default=0  , type=float)
parser.add_option('-x', '--xcoordinate', dest='x'       , help='x-ccordinate'                 , default=0  , type=float)
parser.add_option('-y', '--ycoordinate', dest='y'       , help='y-ccordinate'                 , default=0  , type=float)
parser.add_option('-z', '--zcoordinate', dest='z'       , help='z-ccordinate'                 , default=-50, type=float)
parser.add_option('-o', '--outputdir'  , dest='outDir'  , help='ouput directory'              , default=os.getcwd())
(opt, args) = parser.parse_args()

particleID = -1

#Determine which particle the user selected
if opt.particle == '11' or opt.particle == 'electron':
    particleID = 11
    filename = 'electrons'
    KE = opt.energy - 0.000510999
elif opt.particle == '13' or opt.particle == 'muon':
    particleID = 13
    filename = 'muons'
    KE = opt.energy - 0.10565837
elif opt.particle == '111' or opt.particle == 'pi0':
    particleID = 111
    filename = 'pi0s'
    KE = opt.energy - 0.1349766
elif opt.particle == '211' or opt.particle == 'pi+':
    particleID = 211
    filename = 'pis'
    KE = opt.energy - 0.1395702
elif opt.particle == '2112' or opt.particle == 'neutron':
    particleID = 2112
    filename = 'neutrons'
    KE = opt.energy - 0.9395654
elif opt.particle == '-2112' or opt.particle == 'antineutron':
    particleID = -2112
    filename = 'antineutrons'
    KE = opt.energy - 0.9395654
elif opt.particle == '130' or opt.particle == 'KL':
    particleID = 130
    filename = 'KLs'
    KE = opt.energy - 0.497648
elif opt.particle == '22' or opt.particle == 'photon':
    particleID = 22
    filename = 'photons'
    KE = opt.energy

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

lhefile = open('%s/%d_%gGeV_phi%s_theta%s_x%s_y%s_z%s_%s.lhe'%(opt.outDir,opt.nevts,opt.energy,opt.phi,opt.theta,opt.x,opt.y,opt.z,filename), 'w')
lhefile.write('<header>\n')
lhefile.write('This file contains primary %s.\n'%(filename))
lhefile.write('Do not edit this file manually.\n')
lhefile.write('File created on %s at %s\n'%(strftime('%Y-%m-%d'),strftime('%H:%M:%S')))
lhefile.write('</header>\n')

px = math.sin(math.radians(opt.theta))*math.cos(math.radians(opt.phi))
py = math.sin(math.radians(opt.theta))*math.sin(math.radians(opt.phi))
pz = math.cos(math.radians(opt.theta))

for i in range(0,opt.nevts):

    lhefile.write('<event>\n')

    lhefile.write('%s %g %g %g %g %g %g %g\n'%(particleID,opt.x,opt.y,opt.z,px,py,pz,KE))

    lhefile.write('</event>\n')

lhefile.close()

