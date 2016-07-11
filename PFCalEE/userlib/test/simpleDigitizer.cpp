#include<string>
#include<set>
#include<iostream>
#include<fstream>
#include<sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <random>

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
        std::cout << "argc: " << argc << std::endl;
	std::cout << "TChain files: " << argv[1] << std::endl;
	std::cout << "Output file tag: " << argv[2] << std::endl;
	TChain* tree = new TChain("HGCSSTree");
	tree->Add(argv[1]);

	std::vector<HGCSSSamplingSection> * samplingVec = 0;
	tree->SetBranchAddress("HGCSSSamplingSectionVec", &samplingVec);

	unsigned nEvts = tree->GetEntries();

	char outFileName[256];
	sprintf(outFileName,"simpleHCalDigis_%s.root",argv[2]);
	TFile hfile(outFileName,"RECREATE");

	TTree t1("hcal_digi", "");

	Int_t nLayers,layerPEs[15],layerNum[15];
	
	t1.Branch("nLayers", &nLayers, "nLayers/I");
	t1.Branch("layerPEs", &layerPEs, "layerPEs[nLayers]/I");
	t1.Branch("layerNum", &layerNum, "layerNum[nLayers]/I");

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);

		nLayers = 15;
		for (Int_t j = 43; j < 58 ; j++) {
			HGCSSSamplingSection& sec = (*samplingVec)[j];
			float MeVperMIP = 1.40;
			float PEperMIP = 13.5*6./4.;
			float depEnergy = sec.sensDep();
			float meanPE = depEnergy/MeVperMIP*PEperMIP;
			std::default_random_engine generator;
			std::poisson_distribution<int> distribution(meanPE);
			layerPEs[j-43] = distribution(generator);
			layerNum[j-43] = j-43;

			/*
			std::cout << "- - - - - " << j-42 << " - - - - - - - " << std::endl;
			std::cout << "depEnergy: " << depEnergy << std::endl;
			std::cout << "meanPE: " << meanPE << std::endl;
			std::cout << "PE: " << layerPEs[j-43] << std::endl;
			*/

		}

		t1.Fill();
	}

	t1.Write();
	hfile.Close();
	return 1;
}
