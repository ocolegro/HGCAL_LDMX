#!/usr/bin/env python

import os,sys
import optparse
import commands
import math
import random
import subprocess

random.seed()

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-m', '--model'       ,    dest='model'              , help='set model'                    , default=0,      type=int)
parser.add_option('-b', '--Bfield'      ,    dest='Bfield'             , help='B field value in Tesla'       , default=0,      type=float)
parser.add_option('-f', '--datafile'    ,    dest='datafile'           , help='name of files in directory')
parser.add_option('-d', '--directory'   ,    dest='lheDir'          , help='directory containing lhe files')
parser.add_option('-S', '--no-submit'   ,    action="store_true",  dest='nosubmit'           , help='Do not submit batch job.')
parser.add_option('-t', '--particle'   ,     dest='type'           , help='type of events')
(opt, args) = parser.parse_args()

bval="BOFF"
if opt.Bfield>0 : bval="BON" 

#Extract number of events and file name from path name
pathtoFile = opt.datafile.split('/')
filename = str(pathtoFile[-1])
temp = filename.split('_')
nevts = int(temp[0])

temp = filename.split('.lhe')
outFilename = str(temp[0])

dataDir = '/data/cmszfs1/user/hiltbran/HGCAL_LDMX/PFCalEE/events/rootFiles'
if opt.type=='e':
    dataDir = '%s/electrons'%(dataDir)
if opt.type=='n':
    dataDir = '%s/neutrons'%(dataDir)
if opt.type=='p':
    dataDir = '%s/photons'%(dataDir)
outDir = os.getcwd()
outTag='model%d_%s'%(opt.model,outFilename)

#wrapper
scriptFile = open('%s/runJob.sh'%outDir, 'w')
scriptFile.write('#!/bin/bash\n')
scriptFile.write('source /data/cmszfs1/sw/HGCAL_SIM_A/setup.sh\n')
scriptFile.write('cp %s/g4steer.mac .\n'%outDir)
scriptFile.write('PFCalEE g4steer.mac | tee g4.log\n')
scriptFile.write('mv PFcal.root %s/HGcal_%s.root\n'%(dataDir,outTag))
scriptFile.write('localdir=`pwd`\n')
scriptFile.write('echo "--Local directory is " $localdir >> g4.log\n')
scriptFile.write('ls * >> g4.log\n')
scriptFile.write('echo "All done"\n')
scriptFile.close()

#write geant 4 macro
g4Macro = open('%s/g4steer.mac'%(outDir), 'w')
g4Macro.write('/control/verbose 0\n')
g4Macro.write('/run/verbose 0\n')
g4Macro.write('/event/verbose 0\n')
g4Macro.write('/tracking/verbose 0\n')
g4Macro.write('/N03/det/setField %1.1f T\n'%opt.Bfield)
g4Macro.write('/N03/det/setModel %d\n'%opt.model)
g4Macro.write('/filemode/inputFilename %s/%s\n'%(opt.lheDir,filename))
g4Macro.write('/run/initialize\n')
print opt.lheDir
g4Macro.write('/run/beamOn %d\n'%(nevts))
g4Macro.close()

#submit
condorSubmit = open('%s/condorSubmit'%(outDir), 'w')
condorSubmit.write('Executable          =  %s\n' % scriptFile.name)
condorSubmit.write('Universe            =  vanilla\n')
condorSubmit.write('Requirements        =  Arch=="X86_64"  &&  (Machine  !=  "zebra01.spa.umn.edu")  &&  (Machine  !=  "zebra02.spa.umn.edu")  &&  (Machine  !=  "zebra03.spa.umn.edu")  &&  (Machine  !=  "zebra04.spa.umn.edu")\n')
condorSubmit.write('+CondorGroup        =  "cmsfarm"\n')
condorSubmit.write('getenv              =  True\n')
condorSubmit.write('Request_Memory      =  4 Gb\n')
condorSubmit.write('Log         =  %s.log\n' % outDir)
condorSubmit.write('Queue 1\n')
condorSubmit.close()

os.system('chmod u+rwx %s/runJob.sh'%outDir)
command = "condor_submit " + condorSubmit.name + '\n'
subprocess.call(command.split())
