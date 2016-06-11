//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id$
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "HepMCG4AsciiReader.hh"
#include "HepMCG4PythiaInterface.hh"

#import "TFile.h"
#import "TTree.h"

#include <fstream>
#include <string>
#include <sstream>
#define PI 3.1415926535

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



PrimaryGeneratorAction::PrimaryGeneratorAction(G4int mod, std::string hadronFile, G4int run, G4int nEvents) {
	model_ = mod;
	run_ = run;
	nEvents_ = nEvents;
	G4cout << "Opening root file now " << G4endl;
	file_ = TFile::Open(hadronFile.c_str());

	G4cout << "Setting tree now " << G4endl;
	tree_  = (TTree*) file_->Get("HGCSSTree");

	G4cout << "Pointing the tree now " << G4endl;
	tree_->SetBranchAddress("HGCSSHadronVec",&hadrons_);

	hadrons_ = 0;
	G4int n_particle = 1;

	eventAction_ =
			(EventAction*) G4RunManager::GetRunManager()->GetUserEventAction();
	eventAction_->Add(
			((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getStructure());

	// default generator is particle gun.
	currentGenerator = particleGun = new G4ParticleGun(n_particle);
	currentGeneratorName = "particleGun";
	hepmcAscii = new HepMCG4AsciiReader();
#ifdef G4LIB_USE_PYTHIA
	pythiaGen= new HepMCG4PythiaInterface();
#else
	pythiaGen = 0;
#endif
	gentypeMap["particleGun"] = particleGun;
	gentypeMap["hepmcAscii"] = hepmcAscii;
	gentypeMap["pythia"] = pythiaGen;

	Detector =
			(DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();

	//create a messenger for this class
	gunMessenger = new PrimaryGeneratorMessenger(this);

	// default particle kinematic

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* particle = particleTable->FindParticle("proton");
	particleGun->SetParticleDefinition(particle);
	particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
	particleGun->SetParticleEnergy(4. * GeV);
	G4double position = -0.5 * (Detector->GetWorldSizeZ());
	particleGun->SetParticlePosition(G4ThreeVector(0. * cm, 0. * cm, position));

	G4cout << " -- Gun position set to: 0,0," << position << G4endl;

	rndmFlag = "off";

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
	delete particleGun;
	delete hepmcAscii;
	delete pythiaGen;
	delete gunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	HGCSSGenParticleVec hadronvec_;
	int currentEvt = anEvent->GetEventID();
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	tree_->GetEntry(run_*nEvents_ + currentEvt);
	for (Int_t j = 0; j < hadrons_->size(); j++) {

			HGCSSGenParticle& parton = (*hadrons_)[j];
			eventAction_->genvec_.push_back(parton);

			G4ParticleDefinition* particle = particleTable->FindParticle("proton");//parton.pdgid());
			particleGun->SetParticleDefinition(particle);//particle);
			particleGun->SetParticleEnergy(100*MeV);//parton.vertexKE() * MeV);
			TVector3 pos = parton.vertexPos();
			TVector3 mom = parton.vertexMom();
			particleGun->SetParticleMomentumDirection(G4ThreeVector(mom[0],mom[1],mom[2]));
			G4double z0 = -0.5 * (Detector->GetCalorSizeZ());
			particleGun->SetParticlePosition(G4ThreeVector(0,0,z0));
			currentGenerator->GeneratePrimaryVertex(anEvent);
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

