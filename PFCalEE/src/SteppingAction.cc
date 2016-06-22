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
	G4double eNonIonDep = aStep->GetNonIonizingEnergyDeposit();

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

	if ((thePrePVname == "W1phys" && thePostPVname == "G4_Galactic1phys"))
	{
		const G4ThreeVector & postposition = thePostStepPoint->GetPosition();
		G4ParticleDefinition *pd = lTrack->GetDefinition();
		genPart.setPosition(postposition[0], postposition[1], postposition[2]);
		genPart.setMomentum(p[0], p[1], p[2]);
		genPart.mass(pd->GetPDGMass());
		genPart.time(globalTime);
		genPart.pdgid(pdgId);
		genPart.charge(pd->GetPDGCharge());
		genPart.trackID(trackID);
		genPart.layer(getLayer(thePostPVname) - ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->initLayer());
		isTargetParticle = true;
		const G4ThreeVector &posvec = lTrack->GetVertexPosition();
		const G4ThreeVector &pvec   = lTrack->GetVertexMomentumDirection();

		TVector3 posVec(posvec[0], posvec[1], posvec[2] - zOff);
		genPart.vertexPos(posVec);

		TVector3 momVec(pvec[0], pvec[1], pvec[2]);
		genPart.vertexMom(momVec);
		genPart.vertexKE(lTrack->GetVertexKineticEnergy());

		eventAction_->targetTrackIds.push_back(trackID);
	}
	unsigned int hadronTrackLoc = std::find(eventAction_->novelTrackIds.begin(),
			eventAction_->novelTrackIds.end(), trackID)
			- eventAction_->novelTrackIds.begin();
	bool isNovelTrack = false;
	//Select new (non ele, non gamma) tracks with kin. energy > 10 MeV
	if ((hadronTrackLoc == eventAction_->novelTrackIds.size()) && (kineng>10)) {
		if ((abs(pdgId) != 11) && (abs(pdgId) != 22 )){
		G4ParticleDefinition *pd    = lTrack->GetDefinition();
		const G4ThreeVector &posvec = lTrack->GetVertexPosition();
		const G4ThreeVector &pvec   = lTrack->GetVertexMomentumDirection();

		TVector3 posVec(posvec[0], posvec[1], posvec[2] - zOff);
		genPart.vertexPos(posVec);

		TVector3 momVec(pvec[0], pvec[1], pvec[2]);
		genPart.vertexMom(momVec);
		genPart.vertexKE(lTrack->GetVertexKineticEnergy());

		eventAction_->novelTrackIds.push_back(trackID);
		genPart.layer(getLayer(thePostPVname) - ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->initLayer());
		genPart.mass(pd->GetPDGMass());
		genPart.pdgid(pdgId);

		isNovelTrack = true;
		}
	}
	unsigned int targetTrackLoc = std::find(eventAction_->targetTrackIds.begin(),
			eventAction_->targetTrackIds.end(), trackID)
			- eventAction_->targetTrackIds.begin();
	bool isPrimaryTrack = (targetTrackLoc != eventAction_->targetTrackIds.size());

	G4bool isForward = (p[2] > 0);
	eventAction_->Detect(kineng, eRawDep, eNonIonDep, stepl, globalTime, pdgId, volume,
			position, trackID, parentID, genPart, isNovelTrack,isTargetParticle,isForward,isPrimaryTrack);
}
