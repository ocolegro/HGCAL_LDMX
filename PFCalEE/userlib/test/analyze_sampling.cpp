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
	tree->SetBranchAddress("HGCSSHadronVec", &hadronVec);

	std::vector<HGCSSGenParticle> * targetVec = 0;
	tree->SetBranchAddress("HGCSSTargetVec", &targetVec);
	Int_t firstLayer = 0;
	unsigned nEvts = tree->GetEntries();

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("sampling", "Sampling Study");

	Float_t layerAvgDep, layerAvgSen, layerAvgHFlux, layerAvgNFlux, layerAvgMFlux,maxHadronKe,genKin;
	Float_t layerHFlux[500], layerNFlux[500], layerMFlux[500],
			layerHWgtCnt[500], layerEWgtCnt[500], layerDep[500], layerSen[500],layerHCount[500],layerNCount[500],layerMCount[500],
			layerAvgNCount,layerAvgHCount,layerAvgMcount,layerHShowerSize,layerEWgtAvg,hardestEarlyHadron;

	Int_t layer[500], caloLen, maxHadronProdLayer,genCounter;
	t1.Branch("caloLen", &caloLen, "caloLen/I");

	t1.Branch("layerAvgDep", &layerAvgDep, "layerAvgDep/F");
	t1.Branch("layerAvgSen", &layerAvgSen, "layerAvgSen/F");

	t1.Branch("layerAvgHFlux", &layerAvgHFlux, "layerAvgHFlux/F");
	t1.Branch("layerAvgNFlux", &layerAvgNFlux, "layerAvgNFlux/F");
	t1.Branch("layerAvgMFlux", &layerAvgMFlux, "layerAvgMFlux/F");


	t1.Branch("layerAvgHCount", &layerAvgHCount, "layerAvgHCount/F");
	t1.Branch("layerAvgNCount", &layerAvgNCount, "layerAvgNCount/F");
	t1.Branch("layerAvgMcount", &layerAvgMcount, "layerAvgMcount/F");

	t1.Branch("layerHShowerSize", &layerHShowerSize, "layerHShowerSize/F");
	t1.Branch("layerEWgtAvg", &layerEWgtAvg, "layerEWgtAvg/F");

	t1.Branch("layerHFlux", &layerHFlux, "layerHFlux[caloLen]/F");
	t1.Branch("layerNFlux", &layerNFlux, "layerNFlux[caloLen]/F");
	t1.Branch("layerMFlux", &layerMFlux, "layerMFlux[caloLen]/F");


	t1.Branch("layerHCount", &layerHCount, "layerHCount[caloLen]/F");
	t1.Branch("layerNCount", &layerNCount, "layerNCount[caloLen]/F");
	t1.Branch("layerMCount", &layerMCount, "layerMCount[caloLen]/F");

	t1.Branch("layerHWgtCnt", &layerHWgtCnt, "layerHWgtCnt[caloLen]/F");
	t1.Branch("layerEWgtCnt", &layerEWgtCnt, "layerEWgtCnt[caloLen]/F");


	t1.Branch("maxHadronKe", &maxHadronKe, "maxHadronKe/F");
	t1.Branch("genKin", &genKin, "genKin/F");

	t1.Branch("genCounter", &genCounter, "genCounter/I");
	t1.Branch("maxHadronProdLayer", &maxHadronProdLayer, "maxHadronProdLayer/I");

	t1.Branch("layerSen", &layerSen, "layerSen[caloLen]/F");
	t1.Branch("layerDep", &layerDep, "layerDep[caloLen]/F");


	t1.Branch("layer", &layer, "layer[caloLen]/I");


	t1.Branch("hardestEarlyHadron", &hardestEarlyHadron, "hardestEarlyHadron/F");

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);

		layerAvgSen = 0, layerAvgDep = 0, layerAvgHFlux = 0, layerAvgNFlux = 0, layerAvgMFlux =
				0, caloLen = 0,layerAvgHCount=0,layerAvgNCount=0,layerAvgMcount=0,maxHadronKe=0,genCounter = 0,genKin=0,layerHShowerSize=0,layerEWgtAvg=0;

		hardestEarlyHadron = 0;
		if (ievt > 10000)
			break;
		Float_t nSens = 3.0;
		Float_t nLayers = 26.0;
		for (Int_t j = firstLayer; j < samplingVec->size(); j++) {
			HGCSSSamplingSection& sec = (*samplingVec)[j];
			layerAvgSen += sec.measuredE();
			layerAvgDep += sec.totalE();

			layerAvgHFlux += sec.hadKin()/(nSens * nLayers);
			layerAvgNFlux += sec.neutronKin()/(nSens * nLayers);
			layerAvgMFlux += sec.muKin()/(nSens * nLayers);

			layerAvgHCount += sec.hadCount()/(nSens * nLayers);
			layerAvgNCount += sec.neutronCount()/(nSens * nLayers);
			layerAvgMcount += sec.muCount()/(nSens * nLayers);

			layerSen[j - firstLayer] = sec.measuredE()/nSens;
			layerDep[j - firstLayer] = sec.totalE()/nSens;

			layerHFlux[j - firstLayer] = sec.hadKin()/nSens;
			layerNFlux[j - firstLayer] = sec.neutronKin()/nSens;
			layerMFlux[j - firstLayer] = sec.muKin()/nSens;

			layerHCount[j - firstLayer] = sec.hadCount()/nSens;
			layerNCount[j - firstLayer] = sec.neutronCount()/nSens;
			layerMCount[j - firstLayer] = sec.muCount()/nSens;

			layerHWgtCnt[j - firstLayer] = sec.hadWgtCnt();
			layerEWgtCnt[j - firstLayer] = sec.eleWgtCnt();

			layerHShowerSize += sec.hadWgtCnt()/nLayers;
			layerEWgtAvg += sec.eleWgtCnt()/nLayers;

			layer[j - firstLayer] = j - firstLayer;
			caloLen = caloLen + 1;
		}
			for (Int_t j = 0; j < hadronVec->size(); j++) {
				HGCSSGenParticle& parton = (*hadronVec)[j];
				genCounter += 1;
				Float_t engK = parton.E() -parton.mass();
				genKin += engK;
				if (parton.layer() < 12){
					if (engK > hardestEarlyHadron)
						hardestEarlyHadron = engK;
				}
				if (engK> maxHadronKe){
					maxHadronKe = engK;
					maxHadronProdLayer  = parton.layer();
				}

		}

		t1.Fill();
	}
	t1.Write();
	return 1;
}
