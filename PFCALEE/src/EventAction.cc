#include "EventAction.hh"

#include "RunAction.hh"
#include "EventActionMessenger.hh"
#include "DetectorConstruction.hh"

#include "HGCSSInfo.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>
#include "TVector3.h"
#include "TMath.h"
//
EventAction::EventAction() {
	runAct = (RunAction*) G4RunManager::GetRunManager()->GetUserRunAction();
	eventMessenger = new EventActionMessenger(this);
	printModulo = 100;
	outF_ = TFile::Open("PFcal.root", "RECREATE");
	outF_->cd();

	double xysize =
			((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetCalorSizeXY();

	//save some info
	HGCSSInfo *info = new HGCSSInfo();
	info->calorSizeXY(xysize);
	info->cellSize(CELL_SIZE_X);
	info->model(
			((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getModel());
	info->version(
			((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getVersion());

	std::cout << " -- check Info: version = " << info->version() << " model = "
			<< info->model() << std::endl;
	outF_->WriteObjectAny(info, "HGCSSInfo", "Info");

	tree_ = new TTree("HGCSSTree", "HGC Standalone simulation tree");
	tree_->Branch("HGCSSEvent", "HGCSSEvent", &event_);
	tree_->Branch("HGCSSGenAction", "std::vector<HGCSSGenParticle>",
			&genvec_);

	// }
}

//
EventAction::~EventAction() {
	outF_->cd();
	tree_->Write();
	outF_->Close();
	//fout_.close();
	delete eventMessenger;
}

//
void EventAction::BeginOfEventAction(const G4Event* evt) {
	evtNb_ = evt->GetEventID();
	if (evtNb_ % printModulo == 0) {
		G4cout << "\n---> Begin of event: " << evtNb_ << G4endl;
		CLHEP::HepRandom::showEngineStatus();
	}

}

//
void EventAction::Detect(G4double eDepRaw, G4VPhysicalVolume *volume) {
	G4bool stopIter = false;
	for (size_t i = 0; i < detector_->size(); i++)
		if (stopIter) break;
		(*detector_)[i].add( eDepRaw, volume);
}

//
void EventAction::EndOfEventAction(const G4Event* g4evt) {
	//return;
	bool debug(evtNb_ % printModulo == 0);

	event_.eventNumber(evtNb_);
	event_.vtx_x(g4evt->GetPrimaryVertex(0)->GetX0());
	event_.vtx_y(g4evt->GetPrimaryVertex(0)->GetY0());
	event_.vtx_z(g4evt->GetPrimaryVertex(0)->GetZ0());
	event_.steelThick(((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->GetSteelThick());

	G4String fileN = "currentEvent.rndm";
	CLHEP::HepRandom::saveEngineStatus(fileN);
	std::ifstream input(fileN);
	std::string currentLine;
	Double_t stat_x = 0,stat_y = 0,seed_x = 0,seed_y = 0;
    for(int count = 0; count < 5; count++ ){
    	getline( input, currentLine );
    	if (count == 1)
    		stat_x = std::atoi(currentLine.c_str());
    	if (count == 2)
    		stat_y = std::atoi(currentLine.c_str());

        if (count == 3)
        	seed_x = std::atoi(currentLine.c_str());
        if (count == 4)
        	seed_y = std::atoi(currentLine.c_str());
    }
    TVector3 status(stat_x,stat_y,0);
    TVector3 seeds(seed_x,seed_y,0);

    event_.seeds(seeds);
    event_.status(status);
	//Changing initLayer because initial layers contain tracking sections.
	double totalSens = 0;
	for (size_t i = initLayer; i < detector_->size(); i++) {

		totalSens += (*detector_)[i].getMeasuredEnergy(false);

		} //loop on sensitive layers
    event_.dep(totalSens);


	tree_->Fill();

	//reset vectors
	genvec_.clear();
}
