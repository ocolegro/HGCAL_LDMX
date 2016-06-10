#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

#include "HGCSSGenParticle.hh"
#include "DetectorConstruction.hh"

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
	zOff = -0.5 * (Detector->GetWorldSizeZ());


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

	//Only select new hadronic tracks with kin. energy > 10 MeV
	//Only select hadrons
	else if ((abs(pdgId) != 11) && (abs(pdgId) != 12) && (abs(pdgId) != 13) && (abs(pdgId) != 14) &&  (abs(pdgId) != 22)  &&
			(pdgId != -2112) && (pdgId != -2212)  && (abs(pdgId) != 310) && (abs(pdgId) != 111) &&
			(pdgId != 0) && (pdgId < 1e5)) {
		HGCSSGenParticle genPart;
		G4ParticleDefinition *def = lTrack->GetDefinition();
		G4ThreeVector &pos = lTrack->GetVertexPosition();
		const G4ThreeVector &p = lTrack->GetVertexMomentumDirection();

		TVector3 posVec(pos[0], pos[1], pos[2] - zOff);
		genPart.vertexPos(posVec);

		TVector3 momVec(p[0], p[1], p[2]);
		genPart.vertexMom(momVec);

		genPart.vertexKE(lTrack->GetVertexKineticEnergy());
		genPart.pdgid(pdgId);
		genPart.mass(def->GetPDGMass());
		eventAction_->hadronvec_.push_back(genPart);
		lTrack->SetTrackStatus(fStopAndKill);
		}

	//IF it is in the shower we will now kill it.
	else if ((abs(pdgId) != 11) && (abs(pdgId) != 22)){
		lTrack->SetTrackStatus(fStopAndKill);
	}
}


