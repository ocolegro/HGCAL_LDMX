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
	initLayer = ((DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction())->initLayer();
	//honeycomb
	geomConv_ = new HGCSSGeometryConversion(info->model(), CELL_SIZE_X);
	geomConv_->initialiseHoneyComb(xysize, CELL_SIZE_X);
	//square map for BHCAL
	geomConv_->initialiseSquareMap(xysize, 10.);

	tree_ = new TTree("HGCSSTree", "HGC Standalone simulation tree");
	tree_->Branch("HGCSSEvent", "HGCSSEvent", &event_);
	//Branch containing (''long lasting'') hadronic tracks
	tree_->Branch("HGCSSHadronVec", "std::vector<HGCSSGenParticle>", &hadronvec_);
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

void EventAction::EndOfEventAction(const G4Event* g4evt) {
	//return;
	bool debug(evtNb_ % printModulo == 0);

	event_.eventNumber(evtNb_);
	event_.vtx_x(g4evt->GetPrimaryVertex(0)->GetX0());
	event_.vtx_y(g4evt->GetPrimaryVertex(0)->GetY0());
	event_.vtx_z(g4evt->GetPrimaryVertex(0)->GetZ0());



	tree_->Fill();

	//reset vectors
	hadronvec_.clear();
}
