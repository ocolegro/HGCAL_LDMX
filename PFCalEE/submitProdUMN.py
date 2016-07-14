#!/usr/bin/env python

import os,sys
import argparse
import commands
import math
import random
import subprocess

random.seed()

usage = "usage: %prog [options]"
parser = argparse.ArgumentParser(usage)
parser.add_argument("-m", "--model"      , dest="model"      , help="set detector model (0-3)"      , default=0, type=int)
parser.add_argument("-v", "--version"    , dest="version"    , help="set detector version (1-10)"   , default=8, type=int)
parser.add_argument("-b", "--Bfield"     , dest="Bfield"     , help="B field value in Tesla"        , default=0, type=float)
parser.add_argument("-f", "--datafile"   , dest="datafile"   , help="name of files in directory", required=True)
parser.add_argument("-d", "--directory"  , dest="lheDir"     , help="directory containing lhe files", required=True)
parser.add_argument("-o", "--dataOutDir" , dest="dataOutDir" , help="directory to output root files", required=True)
parser.add_argument("-S", "--no-submit"  , action="store_true", dest="nosubmit" , help="Do not submit batch job.")
arg = parser.parse_args()

lheDir = arg.lheDir
dataOutDir = arg.dataOutDir
# Check for trailing slash on lhedir and outdir and delete
if arg.lheDir.split("/")[-1] == "": lheDir = arg.lheDir[:-1]
if arg.dataOutDir.split("/")[-1] == "": dataOutDir = arg.dataOutDir[:-1]

# Extract number of events, file name and outfile name from path name
filename = str(arg.datafile.split("/")[-1])
if not os.path.isfile(lheDir+"/"+filename):
    print "Provided file \"%s\" does not exist!"%(lheDir+"/"+filename)
    quit()

nevts = int(filename.split("_")[0])
outFilename = str(filename.split(".lhe")[0])

# Create subdirectory based on particle type
particle = filename.split("_")[-1].split(".lhe")[0]
if particle == "pi0s":
    dataOutDir = "%s/pions/%s"%(dataOutDir,particle)
elif particle == "pis":
    dataOutDir = "%s/pions/%s"%(dataOutDir,particle)
else:
    dataOutDir = "%s/%s"%(dataOutDir,particle)

# Check that the lhe and output directories exist
if not os.path.exists(lheDir):
    print "Provided lhe directory \"%s\" does not exist!"%(lheDir)
    quit()

if not os.path.exists(dataOutDir):
    print "Provided output directory \"%s\" does not exist!"%(dataOutDir)
    quit()

# Check for temp directory and create one if not
if not os.path.exists("./temp"): os.mkdir("temp")

bval="BOFF"
if arg.Bfield>0 : bval="BON" 

outDir = os.getcwd()
outTag="version%d_model%d_%s"%(arg.version,arg.model,outFilename)

# Wrapper
scriptFile = open("%s/runJob%s.sh"%(outDir,outFilename), "w")
scriptFile.write("#!/bin/bash\n")
scriptFile.write("source /data/cmszfs1/sw/HGCAL_SIM_A/setup.sh\n")
scriptFile.write("cp %s/g4steer.mac .\n"%outDir)
scriptFile.write("cd temp\n")
scriptFile.write("mkdir %s\n"%(outFilename))
scriptFile.write("cd %s\n"%(outFilename))
scriptFile.write("PFCalEE ./../../g4steer.mac %d %d 1 | tee g4.log\n"%(arg.version,arg.model))
scriptFile.write("mv PFcal.root %s/HGcal_%s.root\n"%(dataOutDir,outTag))
scriptFile.write("localdir=`pwd`\n")
scriptFile.write("echo \"--Local directory is \" $localdir >> g4.log\n")
scriptFile.write("ls * >> g4.log\n")
scriptFile.write("cd ..\n")
scriptFile.write("rm -r %s\n"%(outFilename))
scriptFile.write("cd ../..\n")
scriptFile.write("echo \"All done\"\n")
scriptFile.close()

# Write GEANT4 macro
g4Macro = open("%s/g4steer.mac"%(outDir), "w")
g4Macro.write("/control/verbose 0\n")
g4Macro.write("/run/verbose 0\n")
g4Macro.write("/event/verbose 0\n")
g4Macro.write("/tracking/verbose 0\n")
g4Macro.write("/N03/det/setField %1.1f T\n"%(arg.Bfield))
g4Macro.write("/random/setSeeds %d %d\n"%(random.uniform(0,1000000),random.uniform(0,1000000)))
g4Macro.write("/filemode/inputFilename %s/%s\n"%(lheDir,filename))
g4Macro.write("/run/initialize\n")
g4Macro.write("/run/beamOn %d\n"%(nevts))
g4Macro.close()

# Submit to condor
condorSubmit = open("%s/condorSubmit"%(outDir), "w")
condorSubmit.write("Executable          =  %s\n" % scriptFile.name)
condorSubmit.write("Universe            =  vanilla\n")
condorSubmit.write("Requirements        =  Arch==\"X86_64\"  &&  (Machine  !=  \"zebra01.spa.umn.edu\")  &&  (Machine  !=  \"zebra02.spa.umn.edu\")  &&  (Machine  !=  \"zebra03.spa.umn.edu\")  &&  (Machine  !=  \"zebra04.spa.umn.edu\")\n")
condorSubmit.write("+CondorGroup        =  \"cmsfarm\"\n")
condorSubmit.write("getenv              =  True\n")
condorSubmit.write("Request_Memory      =  4 Gb\n")
condorSubmit.write("Log         =  %s.log\n" % outDir)
condorSubmit.write("Queue 1\n")
condorSubmit.close()

os.system("chmod u+rwx %s/runJob%s.sh"%(outDir,outFilename))

command = "condor_submit " + condorSubmit.name + "\n"
subprocess.call(command.split())

