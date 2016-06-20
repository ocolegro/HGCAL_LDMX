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

	DetectorConstruction*  Detector =
			(DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
	zOff = -0.5 * (Detector->GetCalorSizeZ());

}

//
SteppingAction::~SteppingAction() {
}

//
void SteppingAction::UserSteppingAction(const G4Step* aStep) {
	// get PreStepPoint
	const G4StepPoint *thePreStepPoint = aStep->GetPreStepPoint();
	const G4StepPoint *thePostStepPoint = aStep->GetPostStepPoint();

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

	G4double eRawDep = aStep->GetTotalEnergyDeposit();

	G4double stepl = 0.;
	if (lTrack->GetDefinition()->GetPDGCharge() != 0.)
		stepl = aStep->GetStepLength();

	G4int pdgId = lTrack->GetDefinition()->GetPDGEncoding();
	G4double globalTime = lTrack->GetGlobalTime();
	G4double kineng = lTrack->GetKineticEnergy();

	const G4ThreeVector & position = thePreStepPoint->GetPosition();
	HGCSSGenParticle genPart;
	G4bool isTargetParticle = false;

	const G4ThreeVector &p = lTrack->GetMomentum();


	eventAction_->Detect(eRawDep, volume);
}
