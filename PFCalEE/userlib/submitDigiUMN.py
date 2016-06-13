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
parser.add_option('-m', '--model'       ,    dest='model'              , help='detector model'               , default=0,      type=int)
parser.add_option('-b', '--Bfield'      ,    dest='Bfield'             , help='B field value in Tesla'       , default=0,      type=float)
parser.add_option('-n', '--nevts'       ,    dest='nevts'              , help='number of events to generate' , default=15,    type=int)
parser.add_option('-d', '--directory'   ,    dest='rootDir'      , help='directory containing files')
parser.add_option('-f', '--datafile'   ,    dest='datafile'      , help='name of file')
parser.add_option('-o', '--dataOutDir'   ,    dest='dataOutDir'                , help='directory to output digiroot files',         default='/data/cmszfs1/user/hiltbran/HGCAL_LDMX/PFCalEE/events/digirootFiles/')

parser.add_option('-S', '--no-submit'   ,    action="store_true",  dest='nosubmit'           , help='Do not submit batch job.')
parser.add_option('-t', '--particle'   ,    dest='type'           , help='type of events')
(opt, args) = parser.parse_args()

nSiLayers=3

if opt.type=='e':
    dataOutDir = '%selectrons'%(opt.dataOutDir)
if opt.type=='n':
    dataOutDir = '%sneutrons'%(opt.dataOutDir)
if opt.type=='p':
    dataOutDir = '%sphotons'%(opt.dataOutDir)

pathtoFile = opt.datafile.split('/')
filename = str(pathtoFile[-1])

temp = filename.split('.root')
outFilename = str(temp[0])

#nPuVtxlist=[0,140,200]
nPuVtxlist=[0]

#in %
interCalibList=[3];#0,1,2,3,4,5,10,15,20,50]

granularity='0-25:4'
noise='0-25:0.15'
threshold='0-25:5'

for nPuVtx in nPuVtxlist:

    for interCalib in interCalibList:
        if nPuVtx>0 :
           suffix='Pu%d_IC%d'%(nPuVtx,interCalib)
        else :
            suffix='IC%d'%(interCalib)
            
        if opt.model!=2 : suffix='%s_Si%d'%(suffix,nSiLayers)
            
        bval="BOFF"
        if opt.Bfield>0 : bval="BON" 
        
        outDir = os.getcwd()
    
        outlog='%s/digitizer%s.log'%(outDir,suffix)
        g4log='digijob%s.log'%(suffix)
        os.system('mkdir -p %s'%outDir)
        
        #wrapper
        scriptFile = open('%s/runDigiJob%s.sh'%(outDir,outFilename), 'w')
        scriptFile.write('#!/bin/bash\n')
        scriptFile.write('source /data/cmszfs1/sw/HGCAL_SIM_A/setup.sh\n')
        outTag='model%d'%(opt.model)
        scriptFile.write('localdir=`pwd`\n')
        scriptFile.write('cd temp\n')
        scriptFile.write('mkdir %s\n'%(outFilename))
        scriptFile.write('cd %s\n'%(outFilename))
        scriptFile.write('../../bin/digitizer %d %s%s $PWD %s %s %s %d %d %d | tee %s\n'%(opt.nevts,opt.rootDir,filename,granularity,noise,threshold,interCalib,nSiLayers,nPuVtx,outlog))
        scriptFile.write('echo "--Local directory is " %s/%s.root >> %s\n'%(g4log,dataOutDir,outFilename))
        scriptFile.write('ls * >> %s\n'%(g4log))
        scriptFile.write('mv DigiPFcal.root %s/Digi_%s.root\n'%(dataOutDir,outFilename))
        scriptFile.write('cd ..\n')
        scriptFile.write('rm -r %s\n'%(outFilename))
        scriptFile.write('cd ..\n')
        scriptFile.write('echo "All done"\n')
        scriptFile.close()
        
        #submit
        condorSubmit = open('%s/condorSubmit'%(outDir), 'w')
        condorSubmit.write('Executable          =  %s\n'%(scriptFile.name))
        condorSubmit.write('Universe            =  vanilla\n')
        condorSubmit.write('Requirements        =  Arch=="X86_64"  &&  (Machine  !=  "zebra01.spa.umn.edu")  &&  (Machine  !=  "zebra02.spa.umn.edu")  &&  (Machine  !=  "zebra03.spa.umn.edu")  &&  (Machine  !=  "zebra04.spa.umn.edu")\n')
        condorSubmit.write('+CondorGroup        =  "cmsfarm"\n')
        condorSubmit.write('getenv              =  True\n')
        condorSubmit.write('Request_Memory      =  4 Gb\n')
        condorSubmit.write('Log         =  %s.log\n'%(outDir))
        condorSubmit.write('Queue 1\n')
        condorSubmit.close()

        os.system('chmod u+rwx %s/runDigiJob%s.sh'%(outDir,outFilename))
        command = "condor_submit " + condorSubmit.name + '\n'
        if opt.nosubmit : os.system('echo ' + command) 
        else: subprocess.call(command.split())
