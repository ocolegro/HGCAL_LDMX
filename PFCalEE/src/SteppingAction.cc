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
	timeLimit_ = 20000000000; //ns
	version_ = ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getVersion();
}

//
SteppingAction::~SteppingAction() {
}

//
void SteppingAction::UserSteppingAction(const G4Step* aStep) {

	G4Track* lTrack = aStep->GetTrack();
	G4double kineng = lTrack->GetKineticEnergy();
	G4int pdgId = lTrack->GetDefinition()->GetPDGEncoding();
	if ( (abs(pdgId) == 11 || abs(pdgId) == 12 ||abs(pdgId) == 13 || abs(pdgId) == 14 ||  abs(pdgId) == 22 ) ){
		if (kineng < 100)
		lTrack->SetTrackStatus(fStopAndKill);
	}
	else{
		G4int trackID = lTrack->GetTrackID();
		G4int parentID = lTrack->GetParentID();
		// get rid of non-long lasting hadrons

		if ((pdgId != -2112) && (pdgId != -2212)  && (abs(pdgId) != 310) && (abs(pdgId) != 111) && (pdgId < 1e5)){
			unsigned int parentTrackLoc = std::find(eventAction_->hadronTrackIds.begin(),
					eventAction_->hadronTrackIds.end(), parentID)
					- eventAction_->hadronTrackIds.begin();
			if (parentTrackLoc == eventAction_->hadronTrackIds.size()) {

				// get PreStepPoint
				const G4StepPoint *thePreStepPoint = aStep->GetPreStepPoint();
				const G4StepPoint *thePostStepPoint = aStep->GetPostStepPoint();
				G4VPhysicalVolume* postvolume = thePostStepPoint->GetPhysicalVolume();
				std::string thePostPVname("null");
				if (postvolume == 0) {
				} else {
					thePostPVname = postvolume->GetName();
				}

				G4VPhysicalVolume* volume = thePreStepPoint->GetPhysicalVolume();
				std::string thePrePVname("null");
				if (volume == 0) {
				} else {
					thePrePVname = volume->GetName();
				}

				HGCSSGenParticle genPart;
				unsigned int hadronTrackLoc = std::find(eventAction_->hadronTrackIds.begin(),
						eventAction_->hadronTrackIds.end(), trackID)
						- eventAction_->hadronTrackIds.begin();
				//Only select new hadronic tracks with kin. energy > 10 MeV
				if ((hadronTrackLoc == eventAction_->hadronTrackIds.size()) && (kineng>10)) {
					//Only select hadrons
					{
					const G4ThreeVector & postposition = thePostStepPoint->GetPosition();
					G4ParticleDefinition *pd = lTrack->GetDefinition();
					const G4ThreeVector &p = lTrack->GetMomentum();
					const G4StepPoint *thePreStepPoint = aStep->GetPreStepPoint();
					const G4ThreeVector & position = thePreStepPoint->GetPosition();
					genPart.setPosition(postposition[0], postposition[1], postposition[2]);
					genPart.setMomentum(p[0], p[1], p[2]);
					genPart.mass(pd->GetPDGMass());
					G4double globalTime = lTrack->GetGlobalTime();
					genPart.time(globalTime);
					genPart.pdgid(pdgId);
					genPart.charge(pd->GetPDGCharge());
					genPart.trackID(trackID);
					genPart.layer(getLayer(thePrePVname) - ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->initLayer());
					eventAction_->hadronTrackIds.push_back(trackID);
					eventAction_->hadronvec_.push_back(genPart);


					}
				}

				lTrack->SetTrackStatus(fStopAndKill);
			}
		}
	}
}
