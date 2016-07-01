#! /usr/bin/env python

import os,sys
import argparse
import commands
import math
import random
import subprocess

random.seed()

usage = "usage: %prog [options]"
parser = argparse.ArgumentParser(usage)
parser.add_argument("-m", "--model"     ,    dest="model"      , help="detector model"         , default=0, type=int)
parser.add_argument("-b", "--Bfield"    ,    dest="Bfield"     , help="B field value in Tesla" , default=0, type=float)
parser.add_argument("-d", "--directory ",    dest="rootDir"    , help="directory containing files", required=True)
parser.add_argument("-f", "--datafile"  ,    dest="datafile"   , help="name of file", required=True)
parser.add_argument("-o", "--dataOutDir",    dest="dataOutDir" , help="directory to output digiroot files", required=True)

parser.add_argument("-S", "--no-submit" ,    action="store_true",  dest="nosubmit", help="Do not submit batch job.")
arg = parser.parse_args()

nSiLayers=3

rootDir = arg.rootDir
dataOutDir = arg.dataOutDir
# Check for trailing slash on rootdir and outdir and delete
if arg.rootDir.split("/")[-1] == "": rootDir = arg.rootDir[:-1]
if arg.dataOutDir.split("/")[-1] == "": dataOutDir = arg.dataOutDir[:-1]

# Extract number of events, file name and outfile name from path name
filename = str(arg.datafile.split("/")[-1])
if not os.path.isfile(rootDir+"/"+filename):
    print "Provided file \"%s\" does not exist!"%(filename)
    quit()

temp = filename.split("_")
for i in range(len(temp)):

    if temp[i][:5] == "model":
        model = int(temp[i][5:])
    if temp[i][:7] == "version":
        version = int(temp[i][7:])
    try:
        nevts = int(temp[i])
    except:
        continue
    else:
        nevts = int(temp[i])

# Extract output filename from input filename
outFilename = str(filename.split(".root")[0])

# Check that the root and output directories exist
if not os.path.exists(rootDir):
    print "Provided root directory \"%s\" does not exist!"%(rootDir)
    quit()

if not os.path.exists(dataOutDir):
    print "Provided output directory \"%s\" does not exist!"%(dataOutDir)
    quit()

# Check for temp directory and create one if not
if not os.path.exists("./temp"): os.mkdir("temp")

#in %
interCalibList=[3];

granularity="0-39:4"
noise="0-39:0.15"
threshold="0-39:5"

for interCalib in interCalibList:
    suffix="IC%d"%(interCalib)
        
    if arg.model!=2 : suffix="%s_Si%d"%(suffix,nSiLayers)
        
    bval="BOFF"
    if arg.Bfield>0 : bval="BON" 
    
    outDir = os.getcwd()

    outlog="%s/digitizer%s.log"%(outDir,suffix)
    g4log="digijob%s.log"%(suffix)
    os.system("mkdir -p %s"%(outDir))
    
    #wrapper
    scriptFile = open("%s/runDigiJob%s.sh"%(outDir,outFilename), "w")
    scriptFile.write("#!/bin/bash\n")
    scriptFile.write("source /data/cmszfs1/sw/HGCAL_SIM_A/setup.sh\n")
    scriptFile.write("localdir=`pwd`\n")
    scriptFile.write("cd temp\n")
    scriptFile.write("mkdir %s\n"%(outFilename))
    scriptFile.write("cd %s\n"%(outFilename))
    scriptFile.write("../../bin/digitizer 0 %s/%s $PWD %s %s %s %d %d | tee %s\n"%(rootDir,filename,granularity,noise,threshold,interCalib,nSiLayers,outlog))
    scriptFile.write("echo \"--Local directory is \" %s/%s.root >> %s\n"%(g4log,dataOutDir,outFilename))
    scriptFile.write("ls * >> %s\n"%(g4log))
    scriptFile.write("mv DigiPFcal.root %s/Digi_%s.root\n"%(dataOutDir,outFilename))
    scriptFile.write("cd ..\n")
    scriptFile.write("rm -r %s\n"%(outFilename))
    scriptFile.write("cd ..\n")
    scriptFile.write("echo \"All done\"\n")
    scriptFile.close()
    
    #submit
    condorSubmit = open("%s/condorSubmit"%(outDir), "w")
    condorSubmit.write("Executable          =  %s\n"%(scriptFile.name))
    condorSubmit.write("Universe            =  vanilla\n")
    condorSubmit.write("Requirements        =  Arch==\"X86_64\"  &&  (Machine  !=  \"zebra01.spa.umn.edu\")  &&  (Machine  !=  \"zebra02.spa.umn.edu\")  &&  (Machine  !=  \"zebra03.spa.umn.edu\")  &&  (Machine  !=  \"zebra04.spa.umn.edu\")\n")
    condorSubmit.write("+CondorGroup        =  \"cmsfarm\"\n")
    condorSubmit.write("getenv              =  True\n")
    condorSubmit.write("Request_Memory      =  4 Gb\n")
    condorSubmit.write("Log         =  %s.log\n"%(outDir))
    condorSubmit.write("Queue 1\n")
    condorSubmit.close()

    os.system("chmod u+rwx %s/runDigiJob%s.sh"%(outDir,outFilename))
    command = "condor_submit " + condorSubmit.name + "\n"
    if arg.nosubmit : os.system("echo " + command) 
    else: subprocess.call(command.split())
       
