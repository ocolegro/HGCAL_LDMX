#include<string>
#include<set>
#include<iostream>
#include<fstream>
#include<sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TNtuple.h"
#include "TH2D.h"
#include "TH2Poly.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "Math/Vector4D.h"

#include "HGCSSEvent.hh"
#include "HGCSSInfo.hh"
#include "HGCSSSimHit.hh"
#include "HGCSSRecoHit.hh"
#include "HGCSSGenParticle.hh"
#include "HGCSSRecoJet.hh"
#include "HGCSSCalibration.hh"
#include "HGCSSDigitisation.hh"
#include "HGCSSDetector.hh"
#include "HGCSSGeometryConversion.hh"
#include "HGCSSSamplingSection.hh"
#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif

int main(int argc, char** argv) {

	TFile *infile = TFile::Open(argv[1]);
	TTree *tree = (TTree*) infile->Get("HGCSSTree");
	freopen("log.txt", "w", stdout);

	HGCSSEvent* evt_ = 0;
	tree->SetBranchAddress("HGCSSEvent", &evt_);

	std::vector<HGCSSSimHit> * hitVec_ = 0;
	tree->SetBranchAddress("HGCSSSimHitVec", &hitVec_);


	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("sampling", "Hadronic Study");


	unsigned nHits = 0,cellID[500000],cellLayer[500000];
	Float_t cellEnergy[500000],cellParentID[500000],cellParentKE[500000],cellParentTrack[500000];
	t1.Branch("nHits", &nHits, "nHits/I");
	t1.Branch("cellID", &cellID, "cellID[nHits]/I");
	t1.Branch("cellLayer", &cellLayer, "cellLayer[nHits]/I");

	t1.Branch("cellEnergy", &cellEnergy, "cellEnergy[nHits]/F");
	t1.Branch("cellParentID", &cellParentID, "cellParentID[nHits]/F");
	t1.Branch("cellParentKE", &cellParentKE, "cellParentKE[nHits]/F");
	t1.Branch("cellParentTrack", &cellParentTrack, "cellParentTrack[nHits]/F");

	unsigned nEvts = tree->GetEntries();
	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);
		nHits = hitVec_->size();
		std::cout << "The size of hitVec_ is " << nHits << std::endl;
		for (unsigned j = 0; j < hitVec_->size(); j++) {
			HGCSSSimHit& hit = (*hitVec_)[j];
			cellLayer[j] 			= hit.layer_;
			cellID[j]			= hit.cellid_;
			cellEnergy[j]		= hit.energy_;
			cellParentID[j]		= hit.pdgIDMainParent_;
			cellParentKE[j]		= hit.KEMainParent_;
			cellParentTrack[j]	= hit.trackIDMainParent_;

		}

		t1.Fill();
	}
	t1.Write();

	return 1;
}
