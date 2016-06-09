#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

#include "HGCSSGenParticle.hh"
#include "G4TrackStatus.hh"
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
	G4double lKinEng = lTrack->GetKineticEnergy();
	G4int lPdgId = lTrack->GetDefinition()->GetPDGEncoding();


	const G4StepPoint *thePreStepPoint = aStep->GetPreStepPoint();
	const G4StepPoint *thePostStepPoint = aStep->GetPostStepPoint();
	G4VPhysicalVolume* volume = thePreStepPoint->GetPhysicalVolume();

	std::string thePrePVname("null");
	if (volume == 0) {
	} else {
		thePrePVname = volume->GetName();
	}

	if (lKinEng > 100 && (abs(lPdgId) == 11  ||  abs(lPdgId) == 22 ) ){
		G4cout << "Looking at a primary "  << G4endl;
		const std::vector<const G4Track*>* secondaryTracks = aStep->GetSecondaryInCurrentStep();
			for (unsigned iT(0); iT < secondaryTracks->size() < iT; iT++){
				G4cout << "Looking at a secondary "  << G4endl;

				const G4Track* sTrack = secondaryTracks->at(iT);
				G4double sKinEng = sTrack->GetKineticEnergy();
				G4int sTrackID = sTrack->GetTrackID();
				G4int sPdgId  = sTrack->GetDefinition()->GetPDGEncoding();
				//store good lasting hadrons!
				if((abs(sPdgId) != 11) && (abs(sPdgId) != 22 ) && (sPdgId != -2112) && (sPdgId != -2212)
						&& (abs(sPdgId) != 310) && (abs(sPdgId) != 111)
						&& (sKinEng > 10)){
					G4cout << "Looking at a hadron "  << G4endl;

					HGCSSGenParticle genPart;
					const G4ThreeVector & postposition = thePostStepPoint->GetPosition();
					G4ParticleDefinition *pd = sTrack->GetDefinition();
					const G4ThreeVector &p = sTrack->GetMomentum();
					const G4ThreeVector & position = thePreStepPoint->GetPosition();
					genPart.setPosition(postposition[0], postposition[1], postposition[2]);
					genPart.setMomentum(p[0], p[1], p[2]);
					genPart.mass(pd->GetPDGMass());
					G4double globalTime = sTrack->GetGlobalTime();
					genPart.time(globalTime);
					genPart.pdgid(sPdgId);
					genPart.charge(pd->GetPDGCharge());
					genPart.trackID(sTrackID);
					genPart.layer(getLayer(thePrePVname) - ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->initLayer());
					eventAction_->hadronvec_.push_back(genPart);
				}

			}
		}
	//Delete tracks that have no hope of making us happy
	else {
		lTrack->SetTrackStatus(fStopAndKill);}
}

