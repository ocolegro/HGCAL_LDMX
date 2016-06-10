#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

#include "HGCSSGenParticle.hh"

//
SteppingAction::SteppingAction() {
	eventAction_ =
			(EventAction*) G4RunManager::GetRunManager()->GetUserEventAction();
	eventAction_->Add(
			((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getStructure());
	saturationEngine = new G4EmSaturation();
	version_ = ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getVersion();
}

//
SteppingAction::~SteppingAction() {
}

//
void SteppingAction::UserSteppingAction(const G4Step* aStep) {
	G4Track* lTrack = aStep->GetTrack();
	G4int trackID = lTrack->GetTrackID();

	G4double kineng = lTrack->GetKineticEnergy();
	G4int pdgId = lTrack->GetDefinition()->GetPDGEncoding();

	if (kineng < 100) {
		lTrack->SetTrackStatus(fStopAndKill);
	}
	else if ((abs(pdgId) != 11) && (abs(pdgId) != 12) && (abs(pdgId) != 13) && (abs(pdgId) != 14) &&  (abs(pdgId) != 22)  &&
			(pdgId != -2112) && (pdgId != -2212)  && (abs(pdgId) != 310) && (abs(pdgId) != 111) &&
			(pdgId != 0) && (pdgId < 1e5)) {
		// get PreStepPoint
		const G4StepPoint *thePreStepPoint = aStep->GetPreStepPoint();
		const G4StepPoint *thePostStepPoint = aStep->GetPostStepPoint();


		G4int parentID = lTrack->GetParentID();

		G4VPhysicalVolume* volume = thePreStepPoint->GetPhysicalVolume();
		std::string thePrePVname("null");
		if (volume == 0) {
		} else {
			thePrePVname = volume->GetName();
		}


		G4double stepl = 0.;
		if (lTrack->GetDefinition()->GetPDGCharge() != 0.)
			stepl = aStep->GetStepLength();
		std::cout << "The step length is " << stepl << std::endl;
		G4double globalTime = lTrack->GetGlobalTime();

		const G4ThreeVector & position = thePreStepPoint->GetPosition();
		HGCSSGenParticle genPart;

		const G4ThreeVector &p = lTrack->GetMomentum();

		//Only select new hadronic tracks with kin. energy > 10 MeV
		//Only select hadrons
		const G4ThreeVector & postposition = thePostStepPoint->GetPosition();
		G4ParticleDefinition *pd = lTrack->GetDefinition();
		genPart.setPosition(postposition[0], postposition[1], postposition[2]);
		genPart.setMomentum(p[0], p[1], p[2]);
		genPart.mass(pd->GetPDGMass());
		genPart.time(globalTime);
		genPart.pdgid(pdgId);
		genPart.charge(pd->GetPDGCharge());
		genPart.trackID(trackID);
		genPart.layer(getLayer(thePrePVname) - ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->initLayer());
		lTrack->SetTrackStatus(fStopAndKill);
		eventAction_->hadronvec_.push_back(genPart);
		}
	else if ((abs(pdgId) != 11) && (abs(pdgId) != 22)){
		lTrack->SetTrackStatus(fStopAndKill);
	}
}


