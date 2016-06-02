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

	//honeycomb
	geomConv_ = new HGCSSGeometryConversion(info->model(), CELL_SIZE_X);
	geomConv_->initialiseHoneyComb(xysize, CELL_SIZE_X);
	//square map for BHCAL
	geomConv_->initialiseSquareMap(xysize, 10.);

	tree_ = new TTree("HGCSSTree", "HGC Standalone simulation tree");
	tree_->Branch("HGCSSEvent", "HGCSSEvent", &event_);
	tree_->Branch("HGCSSSamplingSectionVec",
			"std::vector<HGCSSSamplingSection>", &ssvec_);
	tree_->Branch("HGCSSSimHitVec", "std::vector<HGCSSSimHit>", &hitvec_);
	tree_->Branch("HGCSSGenParticleVec", "std::vector<HGCSSGenParticle>",
			&genvec_);
	tree_->Branch("HGCSSTrackVec", "std::vector<HGCSSGenParticle>", &trackvec_);
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
	//fout_ << "Event " << evtNb_ << std::endl;

}

//
void EventAction::Detect(G4double eng, G4double edep, G4double stepl,
		G4double globalTime, G4int pdgId, G4VPhysicalVolume *volume,
		const G4ThreeVector & position, G4int trackID, G4int parentID,
		const HGCSSGenParticle & genPart, G4bool targetParticle) {
	for (size_t i = 0; i < detector_->size(); i++)
		(*detector_)[i].add(eng, edep, stepl, globalTime, pdgId, volume,
				position, trackID, parentID, i);

	if (genPart.isIncoming()){
		if (targetParticle){
			genvec_.push_back(genPart);
		}
		else{
			trackvec_.push_back(genPart);
		}
	}
}

//
void EventAction::EndOfEventAction(const G4Event* g4evt) {
	//return;
	bool debug(evtNb_ % printModulo == 0);
	hitvec_.clear();

	event_.eventNumber(evtNb_);
	event_.vtx_x(g4evt->GetPrimaryVertex(0)->GetX0());
	event_.vtx_y(g4evt->GetPrimaryVertex(0)->GetY0());
	event_.vtx_z(g4evt->GetPrimaryVertex(0)->GetZ0());

	ssvec_.clear();
	ssvec_.reserve(detector_->size());

	for (size_t i = 0; i < detector_->size(); i++) {
		HGCSSSamplingSection lSec;
		lSec.volNb(i);
		lSec.volX0trans((*detector_)[i].getAbsorberX0());
		lSec.voldEdx((*detector_)[i].getAbsorberdEdx());
		lSec.volLambdatrans((*detector_)[i].getAbsorberLambda());
		lSec.absorberE((*detector_)[i].getAbsorbedEnergy());
		lSec.measuredE((*detector_)[i].getMeasuredEnergy(false));
		lSec.totalE((*detector_)[i].getTotalEnergy());
		lSec.gFrac((*detector_)[i].getPhotonFraction());
		lSec.eFrac((*detector_)[i].getElectronFraction());
		lSec.muFrac((*detector_)[i].getMuonFraction());
		lSec.neutronFrac((*detector_)[i].getNeutronFraction());
		lSec.hadFrac((*detector_)[i].getHadronicFraction());
		lSec.avgTime((*detector_)[i].getAverageTime());
		lSec.nSiHits((*detector_)[i].getTotalSensHits());

		lSec.neutronKin((*detector_)[i].getKinNeutron());
		lSec.hadKin((*detector_)[i].getKinHadron());
		lSec.muKin((*detector_)[i].getKinMuon());

		lSec.neutronCount((*detector_)[i].getNeutronCount());
		lSec.hadCount((*detector_)[i].getHadronCount());
		lSec.muCount((*detector_)[i].getMuonCount());
		if (evtNb_ == 1)
			std::cout << "if (layer==" << i << ") return " << lSec.voldEdx()
					<< ";" << std::endl;

		TVector3 eleCnt_, hadCnt_, neutCnt_, muCnt_;
		double eleWgt = 0, muWgt = 0, neutWgt = 0, hadWgt = 0, eleWgtCnt_ = 0,
				hadWgtCnt_ = 0, neutWgtCnt_ = 0, muWgtCnt_ = 0;
		bool is_scint = (*detector_)[i].hasScintillator;
		for (unsigned idx(0); idx < (*detector_)[i].n_sens_elements; ++idx) {
			std::map<unsigned, HGCSSSimHit> lHitMap;
			std::pair<std::map<unsigned, HGCSSSimHit>::iterator, bool> isInserted;

			for (unsigned iSiHit(0);
					iSiHit < (*detector_)[i].getSiHitVec(idx).size();
					++iSiHit) {
				G4SiHit lSiHit = (*detector_)[i].getSiHitVec(idx)[iSiHit];
				HGCSSSimHit lHit(lSiHit, idx,
						is_scint ?
								geomConv_->squareMap() :
								geomConv_->hexagonMap());

				isInserted = lHitMap.insert(
						std::pair<unsigned, HGCSSSimHit>(lHit.cellid(), lHit));
				if (!isInserted.second)
					isInserted.first->second.Add(lSiHit);
				if (idx == 0) {
					TVector3 v1(lSiHit.hit_x, lSiHit.hit_y, lSiHit.hit_z);
					Int_t pdgId_ = lSiHit.pdgId;
					Double_t parentEng = lSiHit.parentEng;
					if ((abs(pdgId_) == 11) || (abs(pdgId_) == 22)) {
						eleCnt_ += v1 * parentEng;
						eleWgtCnt_ += parentEng;
					} else if (abs(pdgId_) == 13) {
						muCnt_ += v1 * parentEng;
						muWgtCnt_ += parentEng;

					} else if (pdgId_ == 2112) {
						neutCnt_ += v1 * parentEng;
						neutWgtCnt_ += parentEng;
					} else if ((abs(pdgId_) != 111) && (abs(pdgId_) != 310)
							&& (pdgId_ != -2212)) {
						hadCnt_ += v1 * parentEng;
						hadWgtCnt_ += parentEng;
					}
				}
			}

			std::map<unsigned, HGCSSSimHit>::iterator lIter = lHitMap.begin();
			hitvec_.reserve(hitvec_.size() + lHitMap.size());
			for (; lIter != lHitMap.end(); ++lIter) {
				(lIter->second).calculateTime();
				hitvec_.push_back(lIter->second);
			}

		} //loop on sensitive layers

		if ((*detector_)[i].n_sens_elements > 0) {

			unsigned absSize_ = (*detector_)[i].getSiHitVec(0).size();

			for (unsigned jAbsHit(0); jAbsHit < absSize_; ++jAbsHit) {
				G4SiHit lAbsHit = (*detector_)[i].getSiHitVec(0)[jAbsHit];
				TVector3 v1(lAbsHit.hit_x, lAbsHit.hit_y, lAbsHit.hit_z);
				Int_t pdgId_ = lAbsHit.pdgId;
				Double_t parentEng = lAbsHit.parentEng;
				if (jAbsHit == 0) {
					if (eleWgtCnt_ > 0)
						eleCnt_ = eleCnt_ * (1.0 / eleWgtCnt_);
					if (muWgtCnt_ > 0)
						muCnt_ = muCnt_ * (1.0 / muWgtCnt_);
					if (neutWgtCnt_ > 0)
						neutCnt_ = neutCnt_ * (1.0 / neutWgtCnt_);
					if (hadWgtCnt_ > 0)
						hadCnt_ = hadCnt_ * (1.0 / hadWgtCnt_);
				}
				if ((abs(pdgId_) == 11) || (abs(pdgId_) == 22)) {
					eleWgt += TMath::Power((v1 - eleCnt_).Mag() * parentEng, 2);
				} else if (abs(pdgId_) == 13) {
					muWgt += TMath::Power((v1 - muCnt_).Mag() * parentEng, 2);
				} else if (pdgId_ == 2112) {
					neutWgt += TMath::Power((v1 - neutCnt_).Mag() * parentEng,
							2);
				} else if ((abs(pdgId_) != 111) && (abs(pdgId_) != 310)
						&& (pdgId_ != -2212)) {
					hadWgt += TMath::Power((v1 - hadCnt_).Mag() * parentEng, 2);
				}
			}
			(eleWgtCnt_ > 0) ?
					lSec.eleWgtCnt(TMath::Sqrt(eleWgt) / eleWgtCnt_) :
					lSec.eleWgtCnt(-1);
			(muWgtCnt_ > 0) ?
					lSec.muWgtCnt(TMath::Sqrt(muWgt) / muWgtCnt_) :
					lSec.muWgtCnt(-1);
			(hadWgtCnt_ > 0) ?
					lSec.hadWgtCnt(TMath::Sqrt(hadWgt) / hadWgtCnt_) :
					lSec.hadWgtCnt(0);
			(neutWgtCnt_ > 0) ?
					lSec.neutWgtCnt(TMath::Sqrt(neutWgt) / neutWgtCnt_) :
					lSec.neutWgtCnt(0);

		}
		ssvec_.push_back(lSec);

		if (debug) {
			(*detector_)[i].report((i == 0));
		}

		(*detector_)[i].resetCounters();
	}
	if (debug) {
		G4cout << " -- Number of truth particles = " << genvec_.size() << G4endl<< " -- Number of simhits = " << hitvec_.size() << G4endl
		<< " -- Number of sampling sections = " << ssvec_.size() << G4endl;

	}

	tree_->Fill();

	//reset vectors
	genvec_.clear();
	hitvec_.clear();
	ssvec_.clear();
	trackvec_.clear();
	trackids.clear();
}
