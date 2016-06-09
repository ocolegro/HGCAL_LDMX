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
	G4int lTrackID = lTrack->GetTrackID();


	const G4StepPoint *thePreStepPoint = aStep->GetPreStepPoint();
	const G4StepPoint *thePostStepPoint = aStep->GetPostStepPoint();
	G4VPhysicalVolume* volume = thePreStepPoint->GetPhysicalVolume();

	std::string thePrePVname("null");
	if (volume == 0) {
	} else {
		thePrePVname = volume->GetName();
	}

	unsigned int lloc = std::find(eventAction_->parentIDs.begin(),
			eventAction_->parentIDs.end(), lTrackID)
			- eventAction_->parentIDs.begin();
	if (lloc == eventAction_->parentIDs.size()){
		eventAction_->parentIDs.push_back(lTrackID);
		eventAction_->parentInfo.push_back(std::make_pair(lPdgId,lKinEng));
	}

	if (lKinEng > 0 ){
		if((abs(lPdgId) != 11) && (abs(lPdgId) != 22 ) && (lPdgId != -2112) && (lPdgId != -2212)){
			G4cout << "The particle pdgId = "  << lPdgId << G4endl;
			unsigned int loc = std::find(eventAction_->parentIDs.begin(),
					eventAction_->parentIDs.end(), lTrack->GetParentID() )
					- eventAction_->parentIDs.begin();

			G4cout << "The parent pdgid and ke are " << eventAction_->parentInfo.at(loc).first << " and " <<  eventAction_->parentInfo.at(loc).second;

		}
		const std::vector<const G4Track*>* secondaryTracks = aStep->GetSecondaryInCurrentStep();
			std::cout << "The size of secondaryTracks->size() is " << secondaryTracks->size() << std::endl;
			for (unsigned iT(0); iT < secondaryTracks->size() < iT; iT++){
				const G4Track* sTrack = secondaryTracks->at(iT);
				G4double sKinEng = sTrack->GetKineticEnergy();
				G4int sTrackID = sTrack->GetTrackID();
				G4int sPdgId  = sTrack->GetDefinition()->GetPDGEncoding();
				std::cout << "The secondary id is " << sPdgId << std::endl;
				//store good lasting hadrons!
				if((sPdgId == 2112)){
						//&& (abs(sPdgId) != 310) && (abs(sPdgId) != 111)
						//&& (sKinEng > 10)){
					G4cout << "The parent pdgId = "  << lPdgId << G4endl;

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
	if (lKinEng < 100 && (abs(lPdgId) == 11  ||  abs(lPdgId) != 22 ) ) {
		lTrack->SetTrackStatus(fStopAndKill);}
}

