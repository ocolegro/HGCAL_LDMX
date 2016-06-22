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

	G4Track* lTrack = aStep->GetTrack();
	//G4int trackID = lTrack->GetTrackID();
	G4double kinEng = lTrack->GetKineticEnergy();
	G4int pdgID = lTrack->GetDefinition()->GetPDGEncoding();

	G4VPhysicalVolume* volume = thePreStepPoint->GetPhysicalVolume();



	G4double eRawDep = aStep->GetTotalEnergyDeposit();


	const G4ThreeVector & position = thePreStepPoint->GetPosition();
	HGCSSGenParticle genPart;

	const G4TrackVector* secondaries= aStep->GetSecondary();
	if(secondaries->size() > 0){
	G4String theProcessName=secondaries->at(0)->GetCreatorProcess()->GetProcessName();
	//if (theProcessName != "eIoni" && theProcessName != "eBrem" && theProcessName!="hadElastic"
		//	&& theProcessName!="neutronElastic" && theProcessName!="conv" && theProcessName != "ionIoni"){ //(theProcessName == "photoNuclear" || theProcessName == "electroNuclear"){
		if (theProcessName == "PhotonInelastic" || theProcessName == "ElectroNuclear" || theProcessName == "PositronNuclear" ){
		bool trackSurvives=(lTrack->GetTrackStatus()==fAlive);
		int nFinalState=secondaries->size() + (trackSurvives?1:0);

		G4cout << "Process " << theProcessName << " The Number of final particles is " << nFinalState << G4endl;
		if(trackSurvives) {
			G4cout << "The primary track is : " ;
			printParticle(lTrack);
		}
		for(G4TrackVector::const_iterator i=secondaries->begin(); i!=secondaries->end(); ++i){
			G4Track* aTrack = *i;
			G4int secID = aTrack->GetDefinition()->GetPDGEncoding();
			if (secID != 11 && secID != 22)
				printParticle(*i);
		}

		eventAction_->Detect(eRawDep,pdgID,kinEng, volume);
		}
	}
}

void SteppingAction::printParticle(G4Track* aTrack)
{
  G4cout << aTrack->GetParticleDefinition()->GetParticleName() << "  "
	<< aTrack->GetDefinition()->GetPDGEncoding() << "  "
	<< aTrack->GetTotalEnergy() << "  "
	<< aTrack->GetKineticEnergy() << "  "
	<< aTrack->GetMomentum().x() << "  "
	<< aTrack->GetMomentum().y() << "  "
	<< aTrack->GetMomentum().z() << "  "
	<< aTrack->GetParticleDefinition()->GetPDGMass() << G4endl;
  return;
}
