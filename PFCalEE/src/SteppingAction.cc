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
	timeLimit_ = 20000000000; //ns
	version_ = ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getVersion();
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
	/*
	if (trackID == 1){
		std::cout << "The main particle is now at thePrePVname " << thePrePVname  << std::endl;
		std::cout << "The main particle is now at thePostPVname " << thePostPVname  << std::endl;

	}
	*/
	const G4ThreeVector &p = lTrack->GetMomentum();

	if ((globalTime < timeLimit_)
			//Select target particles from model v2
			&& ((version_ == 2 && thePrePVname == "Wphys" && thePostPVname == "Si1_0phys") ||
					//Select target particles from model v1,v>=3
					((version_ == 1 || version_ > 2) && thePrePVname == "Wphys" && thePostPVname == "W1phys") ||
					(thePrePVname == "W1phys" && thePostPVname == "G4_Galactic1phys")))
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
		eventAction_->targetTrackIds.push_back(trackID);
	}
	unsigned int hadronTrackLoc = std::find(eventAction_->hadronTrackIds.begin(),
			eventAction_->hadronTrackIds.end(), trackID)
			- eventAction_->hadronTrackIds.begin();
	bool isInitHadron = false;
	//Only select new hadronic tracks with kin. energy > 10 MeV
	if ((hadronTrackLoc == eventAction_->hadronTrackIds.size()) && (kineng>10)) {
		//Only select hadrons
		if ((abs(pdgId) != 11) && (abs(pdgId) != 22 ) && (pdgId != -2112) && (pdgId != -2212)  && (abs(pdgId) != 310) && (abs(pdgId) != 111)){
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
		eventAction_->hadronTrackIds.push_back(trackID);
		isInitHadron = true;
		}
	}
	unsigned int targetTrackLoc = std::find(eventAction_->targetTrackIds.begin(),
			eventAction_->targetTrackIds.end(), trackID)
			- eventAction_->targetTrackIds.begin();
	bool isPrimaryTrack = (targetTrackLoc != eventAction_->targetTrackIds.size());

	G4bool isForward = (p[2] > 0);
	eventAction_->Detect(kineng, eRawDep, eNonIonDep, stepl, globalTime, pdgId, volume,
			position, trackID, parentID, genPart, isInitHadron,isTargetParticle,isForward,isPrimaryTrack);
}
