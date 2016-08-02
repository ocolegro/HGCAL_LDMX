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


	Int_t nHits = 0,cellID[500000],cellEnergy[500000],cellDT[500000],cellParentID[500000];
	t1.Branch("nHits", &nHits, "nHits/I");

	unsigned nEvts = tree->GetEntries();
	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		nHits = hitVec_->size();
		for (Int_t j = 0; j < hitVec_->size(); j++) {
			HGCSSSimHit& hit = (*hitVec_)[j];
			cellID[j]			= hit.cellid();
			cellEnergy[j]		= hit.energy();
			cellID[j]			= hit.cellid_;

		}

		t1.Fill();
	}
	t1.Write();

	return 1;
}
