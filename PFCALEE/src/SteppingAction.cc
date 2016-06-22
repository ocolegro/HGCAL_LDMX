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
	//G4int trackID = lTrack->GetTrackID();
	G4double kinEng = lTrack->GetKineticEnergy();
	G4int pdgID = lTrack->GetDefinition()->GetPDGEncoding();

	G4VPhysicalVolume* volume = thePreStepPoint->GetPhysicalVolume();



	G4double eRawDep = aStep->GetTotalEnergyDeposit();


	const G4ThreeVector & position = thePreStepPoint->GetPosition();
	HGCSSGenParticle genPart;


	if ( (abs(pdgID) == 11) ||  (abs(pdgID) == 22) && kinEng < 100) {
		lTrack->SetTrackStatus(fStopAndKill);
	}

	eventAction_->Detect(eRawDep,pdgID,kinEng, volume);
}
