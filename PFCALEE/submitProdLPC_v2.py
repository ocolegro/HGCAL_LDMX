#!/usr/bin/env python

import os,sys
import optparse
import commands
import math
import random

random.seed()

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
#Long Que is 2nw
parser.add_option('-q', '--queue' ,    dest='queue'             , help='batch queue'            , default='1nd')
parser.add_option('-t', '--git-tag'     ,    dest='gittag'             , help='git tag version'              , default='hexaV02-01-01')
parser.add_option('-r', '--run'         ,    dest='run'                , help='stat run'                     , default=-1,      type=int)
parser.add_option('-v', '--version'     ,    dest='version'            , help='detector version'             , default=1,      type=int)
parser.add_option('-m', '--model'       ,    dest='model'              , help='detector model'               , default=2,      type=int)
parser.add_option('-f', '--fast'      ,    dest='fast'             , help='fast flag'                  , default=1,      type=int)
parser.add_option('-n', '--nevts'       ,    dest='nevts'              , help='number of events to generate' , default=1000,    type=int)
parser.add_option('-o', '--out'         ,    dest='out'                , help='output directory'             , default=os.getcwd() )
parser.add_option('-e', '--eos'         ,    dest='eos'                , help='eos path to save root file to EOS',         default='')
parser.add_option('-p', '--pass'         ,    dest='pass_'                , help='stat pass'                     , default=0,      type=int)

##Location to a txt file containing a csv [particle_energy,dir_x,dir_y,dir_z]
parser.add_option('-S', '--no-submit'   ,    action="store_true",  dest='nosubmit'           , help='Do not submit batch job.')

(opt, args) = parser.parse_args()

label=''


nevents=opt.nevts
myqueue=opt.queue

thickness_ = [1]#,2,3,4,5,6,7,8,9,10,12,14,16,18,20]
print 'creating the job'

for thickness in thickness_:
    outDir='%s/git_%s/version_%d/model_%d'%(opt.out,opt.gittag,opt.version,opt.model)
    outDir='%s/%s'%(outDir,label)
    eosDir='%s/git%s'%(opt.eos,opt.gittag)
    if opt.fast>0 : outDir='%s/fast_%3.3f/'%(outDir,opt.fast)
    if (opt.run>=0) : outDir='%s/run_%d/'%(outDir,opt.run)

    os.system('xrdfs root://cmseos.fnal.gov mkdir %s'%outDir)
    os.system('xrdfs root://cmseos.fnal.gov rm  /%s/PFCalEE' % outDir)
    os.system('xrdfs root://cmseos.fnal.gov rm  /%s/g4env4lpc.csh' % outDir)
    os.system('xrdfs root://cmseos.fnal.gov rm  /%s/libPFCalEE.so' % outDir)
    os.system('xrdfs root://cmseos.fnal.gov rm  /%s/libPFCalEEuserlib.so' % outDir)
    os.system('xrdfs root://cmseos.fnal.gov rm  /%s/runJob.sh' % outDir)

    os.system('eosmkdir -p %s'%outDir)
    os.system('xrdcp $HOME/geant4_workdir/bin/Linux-g++/PFCalEE root://cmseos.fnal.gov/%s/' % outDir)
    os.system('xrdcp g4env4lpc.sh root://cmseos.fnal.gov/%s/' % outDir)
    os.system('xrdcp $HOME/geant4_workdir/tmp/Linux-g++/PFCalEE/libPFCalEE.so root://cmseos.fnal.gov/%s/' % outDir)
    os.system('xrdcp userlib/lib/libPFCalEEuserlib.so root://cmseos.fnal.gov/%s/' % outDir)


    #wrapper
    scriptFile = open('%s/runJob.sh'%(outDir), 'w')
    scriptFile.write('#!/bin/bash\n')
    scriptFile.write('source g4env4lpc.sh\n')#%(os.getcwd()))
    outTag='%s_version%d_model%d_thick%s'%(label,opt.version,opt.model,thickness)
    if (opt.run>=0) : outTag='%s_run%d'%(outTag,opt.run)

    if (opt.pass_ == 0):
        scriptFile.write('./PFCalEE g4steer.mac %d %d %f %s | tee g4.log\n'%(opt.version,opt.model,opt.fast,thickness))
    else:
        scriptFile.write('./PFCalEE g4steer.mac %d %d %f %s HGcal_%s.root | tee g4.log\n'%(opt.version,opt.model,opt.fast,thickness,outTag))

    if (opt.pass_ == 0):
        scriptFile.write('xrdcp -f PFcal.root root://cmseos.fnal.gov/%s/HGcal_%s.root\n'%(outDir,outTag))
    else:
        scriptFile.write('xrdcp -f PFcal.root root://cmseos.fnal.gov/%s/HGcal_%s_second.root\n'%(outDir,outTag))

    scriptFile.write('localdir=`pwd`\n')
    scriptFile.write('echo "--Local directory is " $localdir >> g4.log\n')
    scriptFile.write('ls * >> g4.log\n')



    scriptFile.write('echo "--deleting core files: too heavy!!"\n')
    scriptFile.write('rm core.*\n')
    #scriptFile.write('cp HGcal_%s.root %s/\n'%(outTag,outDir))
    scriptFile.write('echo "All done"\n')
    scriptFile.close()
    print 'submitting to the cluster'
#write geant 4 macro
    g4Macro = open('%s/g4steer.mac'%(outDir), 'w')
    g4Macro.write('/control/verbose 0\n')
    g4Macro.write('/control/saveHistory\n')
    g4Macro.write('/run/verbose 0\n')
    g4Macro.write('/event/verbose 0\n')
    g4Macro.write('/tracking/verbose 0\n')
    g4Macro.write('/N03/det/setModel %d\n'%opt.model)
    g4Macro.write('/random/setSeeds %d %d\n'%( random.uniform(0,100000), random.uniform(0,100000) ) )
    g4Macro.write('/run/beamOn %d\n'%(nevents))
    g4Macro.close()

    #submit
#os.system('echo %s ' %('chmod 777 %s/runJob.sh'%outDir))
    os.system('chmod 777 %s/runJob.sh'%outDir)
    os.system('chmod 777 %s/PFCalEE'%outDir)

    if opt.nosubmit : os.system('LSB_JOB_REPORT_MAIL=N echo bsub -q %s -N %s/runJob.sh'%(myqueue,outDir))
    else:
        #os.system("LSB_JOB_REPORT_MAIL=N bsub -q %s -N \'%s/runJob.sh\'"%(myqueue,outDir))
        name = "submitRun%s" % (opt.run)
        f2n = "%s/submit.jdl" % (outDir);
        outtag = "out_%s_$(Cluster)" % (name)
        f2=open(f2n, 'w')
        f2.write("universe = vanilla \n");
        f2.write("Executable = %s \n" % ('%s/runJob.sh'%(outDir)) );
        f2.write('Requirements = OpSys == "LINUX" && (Arch != "DUMMY" )\n');
        f2.write("request_disk = 100000\n");
        f2.write("request_memory = 1000\n");
        f2.write("Should_Transfer_Files = YES \n");
        if (opt.pass_ == 0):
            f2.write("Transfer_Input_Files = g4env4lpc.sh,libPFCalEE.so,libPFCalEEuserlib.so,PFCalEE,g4steer.mac \n" );
        else:
            f2.write("Transfer_Input_Files = g4env4lpc.sh,libPFCalEE.so,libPFCalEEuserlib.so,PFCalEE,g4steer.mac,HGcal_%s.root \n" %(outTag) );
        f2.write("WhenToTransferOutput  = ON_EXIT_OR_EVICT \n");
        f2.write("Output = "+outtag+".stdout \n");
        f2.write("Error = "+outtag+".stderr \n");
        f2.write("Log = "+outtag+".log \n");
        f2.write("Notification    = Error \n");
        f2.write("x509userproxy = $ENV(X509_USER_PROXY) \n")
        f2.write("Queue 1 \n");
        f2.close();
        print 'Changing dir to %s' % (outDir)
        os.chdir("%s" % (outDir));
        os.system("condor_submit submit.jdl");# % (submit.jdl));