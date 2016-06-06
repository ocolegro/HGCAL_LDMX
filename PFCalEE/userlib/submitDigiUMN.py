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
parser.add_option('-o', '--out'         ,    dest='out'                , help='output directory'             , default=os.getcwd() )
parser.add_option('-e', '--eos'         ,    dest='eos'                , help='eos path to save root file to EOS',         default='')
parser.add_option('-E', '--eosin'       ,    dest='eosin'              , help='eos path to read input root file from EOS',  default='')
parser.add_option('-g', '--gun'         ,    action="store_true",  dest='dogun'              , help='use particle gun.')
parser.add_option('-S', '--no-submit'   ,    action="store_true",  dest='nosubmit'           , help='Do not submit batch job.')
(opt, args) = parser.parse_args()


nSiLayers=2

#INPATHPU="root://eoscms//eos/cms/store/user/msun/V12/MinBias/"
INPATHPU="root://eoscms//eos/cms/store/cmst3/group/hgcal/Standalone/V12/MinBias/"

#nPuVtxlist=[0,140,200]
nPuVtxlist=[0]

#in %
interCalibList=[3];#0,1,2,3,4,5,10,15,20,50]

granularity='0-27:4'
noise='0-27:0.15'
threshold='0-27:5'

for nPuVtx in nPuVtxlist:

    for interCalib in interCalibList:
        if nPuVtx>0 :
           suffix='Pu%d_IC%d'%(nPuVtx,interCalib)
        else :
            suffix='IC%d'%(interCalib)
            
        if opt.model!=2 : suffix='%s_Si%d'%(suffix,nSiLayers)
            
        bval="BOFF"
        if opt.Bfield>0 : bval="BON" 
        
        #eosDirIn='%s'%(opt.eosin)

        outDir = os.getcwd()
    
        if len(opt.eos)>0:
            eosDirIn='root://eoscms//eos/cms%s'%(opt.eosin)
        else:
            eosDir='%s/'%(outDir)
            eosDirIn='%s/'%(outDir)

        outlog='%s/digitizer%s.log'%(outDir,suffix)
        g4log='digijob%s.log'%(suffix)
        os.system('mkdir -p %s'%outDir)
        
        #wrapper
        scriptFile = open('%s/runDigiJob%s.sh'%(outDir,suffix), 'w')
        scriptFile.write('#!/bin/bash\n')
        scriptFile.write('source /data/cmszfs1/sw/HGCAL_SIM_A/setup.sh\n')
        #scriptFile.write('cd %s\n'%(outDir))
        outTag='model%d'%(opt.model)
        scriptFile.write('localdir=`pwd`\n')
        scriptFile.write('%s/bin/digitizer %d %sHGcal_%s.root $localdir/ %s %s %s %d %d %d %s | tee %s\n'%(os.getcwd(),opt.nevts,eosDirIn,outTag,granularity,noise,threshold,interCalib,nSiLayers,nPuVtx,INPATHPU,outlog))
        scriptFile.write('echo "--Local directory is " $localdir >> %s\n'%(g4log))
        scriptFile.write('ls * >> %s\n'%(g4log))
        if len(opt.eos)>0:
            scriptFile.write('grep "alias eos=" /afs/cern.ch/project/eos/installation/cms/etc/setup.sh | sed "s/alias /export my/" > eosenv.sh\n')
            scriptFile.write('source eosenv.sh\n')
            scriptFile.write('$myeos mkdir -p %s\n'%eosDir)
            scriptFile.write('cmsStage -f DigiPFcal.root %s/Digi%s_%s.root\n'%(eosDir,suffix,outTag))
            scriptFile.write('if (( "$?" != "0" )); then\n')
            scriptFile.write('echo " --- Problem with copy of file DigiPFcal.root to EOS. Keeping locally." >> %s\n'%(g4log))
            scriptFile.write('else\n')
            scriptFile.write('eossize=`$myeos ls -l %s/Digi%s_%s.root | awk \'{print $5}\'`\n'%(eosDir,suffix,outTag))
            scriptFile.write('localsize=`ls -l DigiPFcal.root | awk \'{print $5}\'`\n')
            scriptFile.write('if (( "$eossize" != "$localsize" )); then\n')
            scriptFile.write('echo " --- Copy of digi file to eos failed. Localsize = $localsize, eossize = $eossize. Keeping locally..." >> %s\n'%(g4log))
            scriptFile.write('else\n')
            scriptFile.write('echo " --- Size check done: Localsize = $localsize, eossize = $eossize" >> %s\n'%(g4log))
            scriptFile.write('echo " --- File DigiPFcal.root successfully copied to EOS: %s/Digi%s_%s.root" >> %s\n'%(eosDir,suffix,outTag,g4log))
            scriptFile.write('rm DigiPFcal.root\n')
            scriptFile.write('fi\n')
            scriptFile.write('fi\n')
        else:
            scriptFile.write('mv DigiPFcal.root Digi%s_%s.root\n'%(suffix,outTag))

        scriptFile.write('echo "--deleting core files: too heavy!!"\n')
        #scriptFile.write('rm core.*\n')
        #scriptFile.write('cp * %s/\n'%(outDir))
        scriptFile.write('echo "All done"\n')
        scriptFile.close()
        
        #submit
        """
        os.system('chmod u+rwx %s/runDigiJob%s.sh'%(outDir,suffix))
        if opt.nosubmit : os.system('echo bsub -q %s %s/runDigiJob%s.sh'%(myqueue,outDir,suffix)) 
        else: os.system("bsub -q %s \'%s/runDigiJob%s.sh\'"%(myqueue,outDir,suffix))
        """


        #submit
        """
        condorSubmit = open('%s/condorSubmit'%(outDir,suffix), 'w')
        condorSubmit.write('Executable          =  %s\n' % scriptFile.name)
        condorSubmit.write('Universe            =  vanilla\n')
        condorSubmit.write('Requirements        =  Arch=="X86_64"  &&  (Machine  !=  "zebra01.spa.umn.edu")  &&  (Machine  !=  "zebra02.spa.umn.edu")  &&  (Machine  !=  "zebra03.spa.umn.edu")  &&  (Machine  !=  "zebra04.spa.umn.edu")\n')
        condorSubmit.write('+CondorGroup        =  "cmsfarm"\n')
        condorSubmit.write('getenv              =  True\n')
        condorSubmit.write('Log         =  %s.log\n' %(outDir)
        condorSubmit.write('Queue 1\n')
        condorSubmit.close()
        """
        os.system('chmod u+rwx %s/runDigiJob%s.sh'%(outDir,suffix))
        #command = "condor_submit " + condorSubmit.name + '\n'
        #if opt.nosubmit : os.system('echo ' + command) 
        #else: subprocess.call(command.split())
