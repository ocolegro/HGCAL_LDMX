#ifndef EventAction_h
#define EventAction_h 1

#include "SamplingSection.hh"

#include "G4ThreeVector.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "SamplingSection.hh"
#include "G4SiHit.hh"
#include "HGCSSEvent.hh"
#include "HGCSSSamplingSection.hh"
#include "HGCSSSimHit.hh"
#include "HGCSSGenParticle.hh"
#include "HGCSSGeometryConversion.hh"

#include <vector>
#include <map>
#include "fstream"

class RunAction;
class EventActionMessenger;

class EventAction: public G4UserEventAction {
public:
	EventAction();
	virtual ~EventAction();
	void BeginOfEventAction(const G4Event*);
	void EndOfEventAction(const G4Event*);

	void Detect(G4double eRawDep,G4VPhysicalVolume *volume);

	void SetPrintModulo(G4int val) {
		printModulo = val;
	}
	;
	void Add(std::vector<SamplingSection> *newDetector) {
		detector_ = newDetector;
	}
	HGCSSGenParticleVec genvec_;

private:
	RunAction* runAct;
	std::vector<SamplingSection> *detector_;
	G4int evtNb_, printModulo;
	G4bool storeSeeds_;
	HGCSSGeometryConversion* geomConv_;

	TFile *outF_;
	TTree *tree_;
	HGCSSEvent event_;

	EventActionMessenger* eventMessenger;

	//std::ofstream fout_;

};

#endif

