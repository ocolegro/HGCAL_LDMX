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

int main() {
	freopen("hadron_log.txt", "w", stdout);

	TFile *infile = TFile::Open("/afs/cern.ch/work/m/mullin/signal/git_hexaV02-01-01/version_1/model_2/signal_1.000/run_1/HGcal__version1_model2_run1.root");

	//TFile *infile = TFile::Open("/afs/cern.ch/work/o/ocolegro/electron_high_stats_model1/git_hexaV02-01-01/version_30/model_1/e-/BOFF/et_30/eta_10000.000/run_0/HGcal__version30_model1_BOFF_et30_eta10000.000_run0.root");

	TTree *tree = (TTree*) infile->Get("HGCSSTree");

	std::vector<HGCSSSamplingSection> * simhits = 0;
	tree->SetBranchAddress("HGCSSSamplingSectionVec", &simhits);

	unsigned nEvts = tree->GetEntries();

	TFile hfile("simhits.root", "RECREATE");
	TTree t1("hadrons", "Hadron Study");

	Float_t Full_dep, Full_sen, Hadron_dep, Neutron_Dep, Muon_Dep;

	t1.Branch("Full_dep", &Full_dep, "Full_dep/F");
	t1.Branch("Full_sen", &Full_sen, "Full_sen/F");
	t1.Branch("Hadron_dep", &Hadron_dep, "Hadron_dep/F");
	t1.Branch("Neutron_Dep", &Neutron_Dep, "Neutron_Dep/F");
	t1.Branch("Muon_Dep", &Muon_Dep, "Muon_Dep/F");

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);

		Full_sen = 0;
		Full_dep = 0;
		Hadron_dep = 0;
		Neutron_Dep = 0;
		Muon_Dep = 0;

		if (ievt > 10000)
			break;

		for (Int_t j = 0; j < simhits->size(); j++) {
			HGCSSSamplingSection& sec = (*simhits)[j];
			Full_sen += sec.measuredE();
			Full_dep += sec.totalE();
			Hadron_dep += sec.totalE() * sec.hadFrac();
			Neutron_Dep += sec.totalE() * sec.neutronFrac();
			Muon_Dep += sec.totalE() * sec.muFrac();
		}

		t1.Fill();
	}
	t1.Write();

	return 1;
}
