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
	freopen("log.txt", "w", stdout);

	HGCSSEvent* evt_ = 0;
	tree->SetBranchAddress("HGCSSEvent", &evt_);

	std::vector<HGCSSSamplingSection> * samplingVec = 0;
	tree->SetBranchAddress("HGCSSSamplingSectionVec", &samplingVec);

	std::vector<HGCSSSimHit> * hitVec = 0;
	tree->SetBranchAddress("HGCSSSimHitVec", &hitVec);

	std::vector<HGCSSGenParticle> * hadronVec = 0;
	tree->SetBranchAddress("HGCSSHadronVec", &hadronVec);

	std::vector<HGCSSGenParticle> * targetVec = 0;
	tree->SetBranchAddress("HGCSSGenAction", &targetVec);
	Int_t firstLayer = 0;

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("sampling", "Sampling Study");
	std::cout << "The steel thickness is " << evt_->steelThick() << std::endl;
	Int_t nHadrons,nTargetParticles,nLayers,goodEvt,thickness;



	t1.Branch("nHadrons", &nHadrons, "nHadrons/I");
	t1.Branch("nTargetParticles", &nTargetParticles, "nTargetParticles/I");
	t1.Branch("nLayers", &nLayers, "nLayers/I");
	t1.Branch("goodEvt", &goodEvt, "goodEvt/I");
	t1.Branch("thickness", &thickness, "thickness/I");

	Float_t summedSen,summedTotal,summedTotalEcal,layerAvgEGFlux,summedSenEcal,layerHShowerSizeAvgHcal,layerEGFlux[500],layerHShowerSize[500],
	hadron_time[500],hadron_xpos[500],hadron_ypos[500],hadron_zpos[500],
	hadron_mass[500],hadron_px[500],hadron_py[500],hadron_pz[500],
	hadron_pdgid[500],hadron_layer[500],hadron_charge[500],hadron_trackid[500],hadron_KE[500];

	t1.Branch("hadron_time", &hadron_time, "hadron_time[nHadrons]/F");
	t1.Branch("hadron_xpos", &hadron_xpos, "hadron_xpos[nHadrons]/F");
	t1.Branch("hadron_ypos", &hadron_ypos, "hadron_ypos[nHadrons]/F");
	t1.Branch("hadron_zpos", &hadron_zpos, "hadron_zpos[nHadrons]/F");
	t1.Branch("hadron_mass", &hadron_mass, "hadron_mass[nHadrons]/F");
	t1.Branch("hadron_px", &hadron_px, "hadron_px[nHadrons]/F");
	t1.Branch("hadron_py", &hadron_py, "hadron_py[nHadrons]/F");
	t1.Branch("hadron_pz", &hadron_pz, "hadron_pz[nHadrons]/F");
	t1.Branch("hadron_pdgid", &hadron_pdgid, "hadron_pdgid[nHadrons]/F");
	t1.Branch("hadron_charge", &hadron_charge, "hadron_charge[nHadrons]/F");
	t1.Branch("hadron_trackid", &hadron_trackid, "hadron_trackid[nHadrons]/F");
	t1.Branch("hadron_layer", &hadron_layer, "hadron_layer[nHadrons]/F");
	t1.Branch("hadron_KE", &hadron_KE, "hadron_KE[nHadrons]/F");


	Float_t layerAvgEGFluxEcal,target_time[500],target_xpos[500],target_ypos[500],target_zpos[500],
	target_mass[500],target_px[500],target_py[500],target_pz[500],maxAng,
	target_pdgid[500],target_charge[500],target_trackid[500],target_KE[500],
	layerHFlux[500],layerNFlux[500],summedTotalHcal,summedSenHcal,layerHShowerSizeAvg,layerAvgEGFluxHcal;


	t1.Branch("target_time", &target_time, "target_time[nTargetParticles]/F");
	t1.Branch("target_xpos", &target_xpos, "target_xpos[nTargetParticles]/F");
	t1.Branch("target_ypos", &target_ypos, "target_ypos[nTargetParticles]/F");
	t1.Branch("target_zpos", &target_zpos, "target_zpos[nTargetParticles]/F");
	t1.Branch("target_mass", &target_mass, "target_mass[nTargetParticles]/F");
	t1.Branch("target_px", &target_px, "target_px[nTargetParticles]/F");
	t1.Branch("target_py", &target_py, "target_py[nTargetParticles]/F");
	t1.Branch("target_pz", &target_pz, "target_pz[nTargetParticles]/F");
	t1.Branch("target_pdgid", &target_pdgid, "target_pdgid[nTargetParticles]/F");
	t1.Branch("target_charge", &target_charge, "target_charge[nTargetParticles]/F");
	t1.Branch("target_trackid", &target_trackid, "target_trackid[nTargetParticles]/F");
	t1.Branch("target_KE", &target_KE, "target_KE[nHadrons]/F");

	t1.Branch("layerAvgEGFlux", &layerAvgEGFlux, "layerAvgEGFlux/F");
	t1.Branch("layerAvgEGFluxEcal", &layerAvgEGFluxEcal, "layerAvgEGFluxEcal/F");
	t1.Branch("summedSen", &summedSen, "summedSen/F");
	t1.Branch("summedSenEcal", &summedSenEcal, "summedSenEcal/F");
	t1.Branch("summedSenHcal", &summedSenHcal, "summedSenHcal/F");

	t1.Branch("summedTotal", &summedTotal, "summedTotal/F");
	t1.Branch("summedTotalEcal", &summedTotalEcal, "summedTotalEcal/F");
	t1.Branch("summedTotalHcal", &summedTotalHcal, "summedTotalHcal/F");
	t1.Branch("layerHShowerSizeAvg", &layerHShowerSizeAvg, "layerHShowerSizeAvg/F");
	t1.Branch("layerHShowerSizeAvgHcal", &layerHShowerSizeAvgHcal, "layerHShowerSizeAvgHcal/F");
	t1.Branch("layerAvgEGFluxHcal", &layerAvgEGFluxHcal, "layerAvgEGFluxHcal/F");
	t1.Branch("maxAng", &maxAng, "maxAng/F");



	t1.Branch("layerEGFlux", &layerEGFlux, "layerEGFlux[nLayers]/F");
	t1.Branch("layerHFlux", &layerHFlux, "layerHFlux[nLayers]/F");
	t1.Branch("layerNFlux", &layerNFlux, "layerNFlux[nLayers]/F");
	t1.Branch("layerHShowerSize", &layerHShowerSize, "layerHShowerSize[nLayers]/F");


	summedSen = 0,summedTotal = 0,summedTotalEcal = 0,
			summedSenEcal=0,layerAvgEGFluxEcal=0,layerHShowerSizeAvgHcal=0;


	Float_t nSens = 3.0,nLayersECal = 26.0,nLayersHCal = 15.0;

	unsigned nEvts = tree->GetEntries();

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);
		nLayers = samplingVec->size();
		thickness = evt_->steelThick();
		nTargetParticles = 0, nHadrons = 0;
		goodEvt = 0;
		maxAng = -1e6;
		for (Int_t j = 0; j < targetVec->size(); j++) {
			nTargetParticles = nTargetParticles + 1;
			HGCSSGenParticle& target = (*targetVec)[j];
			TVector3 momVec = target.vertexMom();
			TVector3 posVec = target.vertexPos();
			target_time[j]      = target.time();
			target_xpos[j] 	    = posVec[0];
			target_ypos[j] 		= posVec[1];
			target_zpos[j]   	= posVec[2];
			target_mass[j]   	= target.mass();
			target_px[j]   		= momVec[0];
			target_py[j]   		= momVec[1];
			target_pz[j]   		= momVec[2];
			Double_t targAng	= TMath::ATan2(target_pz[j],sqrt(target_px[j] * target_px[j] +target_py[j] * target_py[j]))*180/3.14;
			if (targAng > 30)
				goodEvt = 1;
			if(targAng > maxAng)
				maxAng = targAng;

			target_pdgid[j]   	= target.pdgid();
			target_charge[j]   	= target.charge();
			target_trackid[j]   = target.trackID();
			target_KE[j]		= target.vertexKE();
		}

		for (Int_t j = 0; j < hadronVec->size(); j++) {
			HGCSSGenParticle& hadron = (*hadronVec)[j];
			nHadrons = nHadrons + 1;
			TVector3 momVec = hadron.vertexMom();
			TVector3 posVec = hadron.vertexPos();

			hadron_time[j]      = hadron.time();
			hadron_xpos[j] 	    = posVec[0];
			hadron_ypos[j] 		= posVec[1];
			hadron_zpos[j]   	= posVec[2];
			hadron_mass[j]   	= hadron.mass();
			hadron_px[j]   		= momVec[0];
			hadron_py[j]   		= momVec[1];
			hadron_pz[j]   		= momVec[2];
			hadron_pdgid[j]   	= hadron.pdgid();
			hadron_charge[j]   	= hadron.charge();
			hadron_trackid[j]   = hadron.trackID();
			hadron_trackid[j]   = hadron.layer();
			hadron_KE[j]		= hadron.vertexKE();

		}
		summedSen = 0,summedTotal = 0,summedTotalEcal = 0,
				summedSenEcal=0,layerAvgEGFluxEcal=0,layerHShowerSizeAvgHcal=0,
				summedTotalHcal=0,summedSenHcal=0,layerHShowerSizeAvg=0,layerAvgEGFlux=0,
				layerAvgEGFluxHcal=0;
		for (Int_t j = firstLayer; j < samplingVec->size(); j++) {
					HGCSSSamplingSection& sec = (*samplingVec)[j];
					summedSen += sec.sensDep();
					summedTotal += sec.totalDep();
					layerHShowerSizeAvg += sec.hadronShowerSize()/nLayersHCal;
					layerAvgEGFlux += (sec.eleKinFlux()+sec.gamKinFlux())/(nSens * nLayersECal + nLayersHCal);
					if (j < nLayersECal){
						summedSenEcal += sec.sensDep();
						summedTotalEcal += sec.totalDep();
						layerAvgEGFluxEcal += (sec.eleKinFlux()+sec.gamKinFlux())/(nSens * nLayersECal);
					}
					if (j >= nLayersECal){
						summedTotalHcal += sec.totalDep();
						summedSenHcal += sec.sensDep();
						layerHShowerSizeAvgHcal += sec.hadronShowerSize()/nLayersHCal;
						layerAvgEGFluxHcal += (sec.eleKinFlux()+sec.gamKinFlux())/(nSens * nLayersHCal);

					}
					layerEGFlux[j] = (sec.eleKinFlux()+sec.gamKinFlux())/nSens;
					layerHShowerSize[j] = sec.hadronShowerSize();
					layerNFlux[j] = sec.neutronKinFlux()/nSens;
					layerHFlux[j] = sec.hadKinFlux()/nSens;

		}

		t1.Fill();
	}
	t1.Write();

	return 1;
}
