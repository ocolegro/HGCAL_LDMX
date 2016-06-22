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
	std::cout << "Opening the file " << argv[1] << std::endl;
	TFile *infile = TFile::Open(argv[1]);
	TTree *tree = (TTree*) infile->Get("HGCSSTree");

	std::vector<HGCSSSamplingSection> * samplingVec = 0;
	tree->SetBranchAddress("HGCSSSamplingSectionVec", &samplingVec);

	std::vector<HGCSSSimHit> * hitVec = 0;
	tree->SetBranchAddress("HGCSSSimHitVec", &hitVec);

	std::vector<HGCSSGenParticle> * hadronVec = 0;
	tree->SetBranchAddress("HGCSSTrackVec", &hadronVec);

	std::vector<HGCSSGenParticle> * targetVec = 0;
	tree->SetBranchAddress("HGCSSGenParticleVec", &targetVec);

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("hadrons", "Hadron Study");

	Int_t nHadrons,nTargets,hadronId[500],targetId[500],nTargetPhotons,
	nTargetElectrons,nTargetHadrons,nTargetNeutrons,nProtons,nNeutrons;
	Float_t hadronKE[500],hadronTheta[500],hadronPhi[500],
	targetE[500],targetTheta[500],targetPhi[500];

	t1.Branch("nHadrons", &nHadrons, "nHadrons/I");
	t1.Branch("nProtons", &nProtons, "nProtons/I");
	t1.Branch("nNeutrons", &nNeutrons, "nNeutrons/I");

	t1.Branch("hadronId", &hadronId, "hadronId[nHadrons]/I");
	t1.Branch("hadronKE", &hadronKE, "KE[nHadrons]/F");
	t1.Branch("hadronTheta", &hadronTheta, "hadronTheta[nHadrons]/F");

	t1.Branch("nTargets", &nTargets, "nTargets/I");
	t1.Branch("nTargetHadrons", &nTargetHadrons, "nTargetHadrons/I");
	t1.Branch("nTargetElectrons", &nTargetElectrons, "nTargetElectrons/I");
	t1.Branch("nTargetPhotons", &nTargetPhotons, "nTargetPhotons/I");

	t1.Branch("targetId", &targetId, "targetid[nTargets]/I");
	t1.Branch("targetE", &targetE, "targetE[nTargets]/F");
	t1.Branch("targetTheta", &targetTheta, "targetTheta[nTargets]/I");

	unsigned nEvts = tree->GetEntries();

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);

		nHadrons = 0,nTargets = 0,nTargetHadrons = 0,nTargetNeutrons = 0,nTargetElectrons = 0,nTargetPhotons = 0;


		for (Int_t j = 0; j < targetVec->size(); j++) {
			nTargets = nTargets + 1;
			HGCSSGenParticle& target = (*targetVec)[j];
			Int_t tPdg = target.pdgid();
			targetId[j]    = tPdg;
			targetE[j] 	   = target.E();
			targetTheta[j] = target.theta();
			targetPhi[j]   = target.phi();

			if (abs(tPdg) == 11){
				nTargetElectrons += 1;
			}
			else if (abs(tPdg) == 22){
				nTargetPhotons += 1;
			}
			else if (tPdg == 2112){
				nTargetNeutrons += 1;
			}
			else if ((abs(tPdg) != 111) && (abs(tPdg) != 310) && (tPdg != -2212)){
				nTargetHadrons += 1;
			}
		}

		for (Int_t j = 0; j < hadronVec->size(); j++) {
			HGCSSGenParticle& hadron = (*hadronVec)[j];
			Int_t hPdg   = hadron.pdgid();
			hadronId[j]  = hPdg;
			hadronKE[j]	 = hadron.E() - hadron.mass();
			hadronTheta[j] = hadron.theta();
			hadronPhi[j] = hadron.phi();
			nHadrons = nHadrons + 1;
			if (hPdg == 2112){
				nNeutrons  += 1;
			}
			else if (hPdg == 2212){
				nProtons += 1;
			}
		}



		t1.Fill();
	}
	t1.Write();

	return 1;
}
