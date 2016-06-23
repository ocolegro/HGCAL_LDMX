#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

#include "HGCSSGenParticle.hh"

//
SteppingAction::SteppingAction(std::string data) {
	eventAction_ =
			(EventAction*) G4RunManager::GetRunManager()->GetUserEventAction();
	eventAction_->Add(
			((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getStructure());
	saturationEngine = new G4EmSaturation();
	version_ = ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getVersion();

	DetectorConstruction*  Detector =
			(DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
	zOff = -0.5 * (Detector->GetCalorSizeZ());
	secondPass = (data == "") ? false : true;


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
	eventAction_->Detect(eRawDep,pdgID,kinEng, volume);


	bool trackEscapes = (lTrack->GetTrackStatus()!=fAlive && lTrack->GetKineticEnergy() > 10 && secondPass);
	if (trackEscapes){
		HGCSSGenParticle escapePart;
		escapePart.vertexKE(lTrack->GetVertexKineticEnergy()); //- aStep->GetDeltaEnergy());
		escapePart.finalKE(lTrack->GetKineticEnergy()); //- aStep->GetDeltaEnergy());

		const G4ThreeVector &p = lTrack->GetVertexMomentumDirection() ;//- aStep->GetDeltaMomentum();
		const G4ThreeVector &pos = lTrack->GetVertexPosition();
		TVector3 momVec(p[0], p[1], p[2]);
		escapePart.vertexMom(momVec);
		TVector3 posVec(pos[0], pos[1], pos[2] - zOff);
		escapePart.vertexPos(posVec);
		escapePart.pdgid(pdgID);
		escapePart.mass(lTrack->GetDefinition()->GetPDGMass());

		eventAction_->escapevec_.push_back(escapePart);
	}
	const G4TrackVector* secondaries= aStep->GetSecondary();
	if(secondaries->size() > 0){
	G4String theProcessName=secondaries->at(0)->GetCreatorProcess()->GetProcessName();
	//if (theProcessName != "eIoni" && theProcessName != "eBrem" && theProcessName!="hadElastic"
		//	&& theProcessName!="neutronElastic" && theProcessName!="conv" && theProcessName != "ionIoni"){ //(theProcessName == "photoNuclear" || theProcessName == "electroNuclear"){
		if (theProcessName == "PhotonInelastic" || theProcessName == "ElectroNuclear" || theProcessName == "PositronNuclear" ){
			eventAction_->hadronicInts = eventAction_->hadronicInts  + 1;
			HGCSSGenParticle targPart;
			targPart.vertexKE(lTrack->GetKineticEnergy() - aStep->GetDeltaEnergy());


			const G4ThreeVector &p = lTrack->GetMomentum() + -1.*aStep->GetDeltaMomentum();
			const G4ThreeVector &pos = lTrack->GetPosition();
			TVector3 momVec(p[0]/p.mag(), p[1]/p.mag(), p[2]/p.mag());
			targPart.vertexMom(momVec);
			TVector3 posVec(pos[0], pos[1], pos[2] - zOff);
			targPart.vertexPos(posVec);
			targPart.pdgid(pdgID);
			targPart.layer(-eventAction_->hadronicInts);
			eventAction_->hadvec_.push_back(targPart);

			for(G4TrackVector::const_iterator i=secondaries->begin(); i!=secondaries->end(); ++i){
				G4Track* iTrack = *i;
				HGCSSGenParticle genPart;

				genPart.vertexKE(iTrack->GetKineticEnergy());
				const G4ThreeVector &p = iTrack->GetMomentumDirection();
				const G4ThreeVector &pos = iTrack->GetPosition();
				TVector3 momVec(p[0], p[1], p[2]);
				genPart.vertexMom(momVec);
				TVector3 posVec(pos[0], pos[1], pos[2] - zOff);
				genPart.vertexPos(posVec);
				genPart.mass(iTrack->GetDefinition()->GetPDGMass());
				genPart.pdgid(iTrack->GetDefinition()->GetPDGEncoding());
				genPart.layer(eventAction_->hadronicInts);
				eventAction_->hadvec_.push_back(genPart);
			}



		}
	}
}

void SteppingAction::printParticle(G4Track* aTrack)
{
  G4cout << aTrack->GetParticleDefinition()->GetParticleName() << "  "
	<< aTrack->GetDefinition()->GetPDGEncoding() << "  "
	<< aTrack->GetTrackID()<< "  "

	<< aTrack->GetTotalEnergy() << "  "
	<< aTrack->GetKineticEnergy() << "  "
	<< aTrack->GetMomentum().x() << "  "
	<< aTrack->GetMomentum().y() << "  "
	<< aTrack->GetMomentum().z() << "  "
	<< aTrack->GetParticleDefinition()->GetPDGMass() << G4endl;
  return;
}
