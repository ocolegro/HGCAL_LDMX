#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

#include "HGCSSGenParticle.hh"

//
SteppingAction::SteppingAction() {
	eventAction_ =
			(EventAction*) G4RunManager::GetRunManager()->GetUserEventAction();
	eventAction_->Add(
			((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getStructure());
	saturationEngine = new G4EmSaturation();
	timeLimit_ = 20000000000; //ns
}

//
SteppingAction::~SteppingAction() {
}

//
void SteppingAction::UserSteppingAction(const G4Step* aStep) {
	// get PreStepPoint
	const G4StepPoint *thePreStepPoint = aStep->GetPreStepPoint();
	const G4StepPoint *thePostStepPoint = aStep->GetPostStepPoint();
	// get TouchableHandle

	//G4TouchableHandle theTouchable = thePreStepPoint->GetTouchableHandle();
	const G4Track* lTrack = aStep->GetTrack();
	G4int trackID = lTrack->GetTrackID();
	G4int parentID = lTrack->GetParentID();

	G4VPhysicalVolume* volume = thePreStepPoint->GetPhysicalVolume();
	std::string thePrePVname("null");
	if (volume == 0) {
	} else {
		thePrePVname = volume->GetName();
	}

	G4VPhysicalVolume* postvolume = thePostStepPoint->GetPhysicalVolume();
	std::string thePostPVname("null");
	if (postvolume == 0) {
	} else {
		thePostPVname = postvolume->GetName();
	}

	G4double edep = aStep->GetTotalEnergyDeposit();

	G4double stepl = 0.;
	if (lTrack->GetDefinition()->GetPDGCharge() != 0.)
		stepl = aStep->GetStepLength();

	G4int pdgId = lTrack->GetDefinition()->GetPDGEncoding();
	G4double globalTime = lTrack->GetGlobalTime();
	G4double kineng = lTrack->GetKineticEnergy();

	const G4ThreeVector & position = thePreStepPoint->GetPosition();

	HGCSSGenParticle genPart;
	G4bool targetParticle = false;
	if ((globalTime < timeLimit_)
			&& ((thePrePVname == "Wphys" && thePostPVname == "W1phys")
					|| (thePrePVname == "W1phys"
							&& thePostPVname == "G4_Galactic1phys")))
	{
		targetParticle = true;
		const G4ThreeVector & postposition = thePostStepPoint->GetPosition();
		const G4ThreeVector &p = lTrack->GetMomentum();
		G4ParticleDefinition *pd = lTrack->GetDefinition();
		genPart.setPosition(postposition[0], postposition[1], postposition[2]);
		genPart.setMomentum(p[0], p[1], p[2]);
		genPart.mass(pd->GetPDGMass());
		genPart.time(globalTime);
		genPart.pdgid(pdgId);
		genPart.charge(pd->GetPDGCharge());
		genPart.trackID(trackID);
	}
	unsigned int id_ = std::find(eventAction_->trackids.begin(),
			eventAction_->trackids.end(), trackID)
			- eventAction_->trackids.begin();

	if ((id_ == eventAction_->trackids.size()) && (kineng>10)) {
		if ((abs(pdgId) != 11) && (abs(pdgId) != 22 ) && (pdgId != -2112) && (pdgId != -2212)  && (abs(pdgId) != 310) && (abs(pdgId) != 111)){
		const G4ThreeVector & postposition = thePostStepPoint->GetPosition();

		const G4ThreeVector &p = lTrack->GetMomentum();
		G4ParticleDefinition *pd = lTrack->GetDefinition();
		genPart.setPosition(postposition[0], postposition[1], postposition[2]);
		genPart.setMomentum(p[0], p[1], p[2]);
		genPart.mass(pd->GetPDGMass());
		genPart.time(globalTime);
		genPart.pdgid(pdgId);
		genPart.charge(pd->GetPDGCharge());
		genPart.trackID(trackID);
		genPart.layer(getLayer(thePostPVname));
		eventAction_->trackids.push_back(trackID);
		}
	}
	eventAction_->Detect(kineng, edep, stepl, globalTime, pdgId, volume,
			position, trackID, parentID, genPart, targetParticle);
}
