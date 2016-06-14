#!/usr/bin/env python
import math
import random
import optparse
import os

from time import strftime

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-f', '--filename', dest='filename', help='lhe file name', default='antineutrons')
parser.add_option('-n', '--nevts', dest='nevts', help='number of events', default=100, type=int)
parser.add_option('-e', '--energy', dest='energy', help='energy of events in GeV', default=4, type=float)
parser.add_option('-p', '--phi', dest='phi', help='set phi angle in degrees', default=0, type=float)
parser.add_option('-t', '--theta', dest='theta', help='set theta angle in degrees', default=0, type=float)
parser.add_option('-x', '--xcoordinate', dest='x', help='x-ccordinate', default=0, type=float)
parser.add_option('-y', '--ycoordinate', dest='y', help='y-ccordinate', default=0, type=float)
parser.add_option('-z', '--zcoordinate', dest='z', help='z-ccordinate', default=-50, type=float)
parser.add_option('-o', '--outputdir', dest='outDir', help='ouput directory', default=os.getcwd())
(opt, args) = parser.parse_args()

lhefile = open('%s/%d_%gGeV_phi%s_theta%s_x%s_y%s_z%s_%s.lhe'%(opt.outDir,opt.nevts,opt.energy,opt.phi,opt.theta,opt.x,opt.y,opt.z,opt.filename), 'w')
lhefile.write('<header>\n')
lhefile.write('This file contains primary antineutrons.\n')
lhefile.write('Do not edit this file manually.\n')
lhefile.write('File created on %s at %s\n'%(strftime('%Y-%m-%d'),strftime('%H:%M:%S')))
lhefile.write('</header>\n')

px = math.sin(math.radians(opt.theta))*math.cos(math.radians(opt.phi))
py = math.sin(math.radians(opt.theta))*math.sin(math.radians(opt.phi))
pz = math.cos(math.radians(opt.theta))

for i in range(0,opt.nevts):

    lhefile.write('<event>\n')

    lhefile.write('-2112 %g %g %g %g %g %g %g\n'%(opt.x,opt.y,opt.z,px,py,pz,opt.energy))

    lhefile.write('</event>\n')

lhefile.close()

