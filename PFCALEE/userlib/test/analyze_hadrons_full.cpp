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


	std::vector<HGCSSGenParticle> * hadVec = 0;
	tree->SetBranchAddress("HGCSSHadAction", &hadVec);

	std::vector<HGCSSGenParticle> * escapeVec = 0;
	tree->SetBranchAddress("HGCSSEscapeAction", &escapeVec);

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("sampling", "Hadronic Study");

	Int_t nInteractions,nSecondaries[50000],nProtonSecondaries[50000],nNeutronSecondaries[50000],
	nOtherSecondaries[50000],nContainedSecondaries[50000],nUncontainedSecondaries[50000];

	t1.Branch("nInteractions", &nInteractions, "nInteractions/I");
	t1.Branch("nSecondaries", &nSecondaries[nInteractions], "nSecondaries[nInteractions]/I");
	t1.Branch("nProtonSecondaries", &nProtonSecondaries[nInteractions], "nProtonSecondaries[nInteractions]/I");
	t1.Branch("nNeutronSecondaries", &nNeutronSecondaries[nInteractions], "nNeutronSecondaries[nInteractions]/I");
	t1.Branch("nOtherSecondaries", &nOtherSecondaries[nInteractions], "nOtherSecondaries[nInteractions]/I");
	t1.Branch("nContainedSecondaries", &nContainedSecondaries[nInteractions], "nContainedSecondaries[nInteractions]/I");
	t1.Branch("nUncontainedSecondaries", &nUncontainedSecondaries[nInteractions], "nUncontainedSecondaries[nInteractions]/I");

	Float_t inc_KE[50000],inc_zpos[50000],inc_theta[50000];
	Int_t   inc_pdgid[50000];

	t1.Branch("inc_pdgid", &inc_pdgid, "inc_pdgid[nInteractions]/I");
	t1.Branch("inc_KE", &inc_KE, "inc_KE[nInteractions]/F");
	t1.Branch("inc_zpos", &inc_zpos, "inc_zpos[nInteractions]/F");
	t1.Branch("inc_theta", &inc_theta, "inc_theta[nInteractions]/F");

	Float_t out_KE[50000],out_NE[50000],out_PE[50000],out_Eff[50000],out_OE[50000];

	t1.Branch("out_KE", &out_KE, "out_KE[nInteractions]/F");
	t1.Branch("out_NE", &out_NE, "out_NE[nInteractions]/F");
	t1.Branch("out_PE", &out_PE, "out_PE[nInteractions]/F");
	t1.Branch("out_OE", &out_OE, "out_OE[nInteractions]/F");
	t1.Branch("out_Eff", &out_Eff, "out_Eff[nInteractions]/F");

	Float_t hadron_zpos[50000],
	hadron_theta[50000],hadron_px[50000]  , hadron_py[50000]  ,hadron_pz[50000],
	hadron_KE[50000];
	Int_t nHadrons,hadron_pdgid[50000];

	t1.Branch("nHadrons", &nHadrons, "nHadrons/I");
	t1.Branch("hadron_pdgid", &hadron_pdgid, "hadron_pdgid[nHadrons]/I");

	t1.Branch("hadron_zpos", &hadron_zpos, "hadron_zpos[nHadrons]/F");
	t1.Branch("hadron_px", &hadron_px, "hadron_px[nHadrons]/F");
	t1.Branch("hadron_py", &hadron_py, "hadron_py[nHadrons]/F");
	t1.Branch("hadron_pz", &hadron_pz, "hadron_pz[nHadrons]/F");
	t1.Branch("hadron_theta", &hadron_theta, "hadron_theta[nHadrons]/F");
	t1.Branch("hadron_KE", &hadron_KE, "hadron_KE[nHadrons]/F");

	Float_t escape_zpos[50000],escape_xpos[50000],escape_ypos[50000],
	escape_theta[50000],escape_px[50000]  , escape_py[50000]  ,escape_pz[50000],
	escape_VKE[50000],escape_FKE[50000];
	Int_t nEscapes,escape_pdgid[50000];

	t1.Branch("nEscapes", &nEscapes, "nEscapes/I");
	t1.Branch("escape_pdgid", &escape_pdgid, "escape_pdgid[nEscapes]/I");
	t1.Branch("escape_theta", &escape_theta, "escape_theta[nEscapes]/F");
	t1.Branch("escape_px", &escape_px, "escape_px[nEscapes]/F");
	t1.Branch("escape_py", &escape_py, "escape_py[nEscapes]/F");
	t1.Branch("escape_pz", &escape_pz, "escape_pz[nEscapes]/F");
	t1.Branch("escape_xpos", &escape_xpos, "escape_xpos[nEscapes]/F");
	t1.Branch("escape_ypos", &escape_ypos, "escape_ypos[nEscapes]/F");
	t1.Branch("escape_zpos", &escape_zpos, "escape_zpos[nEscapes]/F");
	t1.Branch("escape_VKE", &escape_VKE, "escape_VKE[nEscapes]/F");
	t1.Branch("escape_FKE", &escape_FKE, "escape_FKE[nEscapes]/F");

	Float_t summedSen,summedSenWgt,convEng,accConvEng,lostEnergy;

	t1.Branch("convEng", &convEng, "convEng/F");
	t1.Branch("accConvEng", &accConvEng, "accConvEng/F");
	t1.Branch("lostEnergy", &lostEnergy, "lostEnergy/F");
	t1.Branch("summedSen", &summedSen, "summedSen/F");
	t1.Branch("summedSenWgt", &summedSenWgt, "summedSenWgt/F");




	unsigned nEvts = tree->GetEntries();
	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);
		summedSen = evt_->dep();
		summedSenWgt = evt_->wgtDep();
		if (summedSen == 0) continue;

		lostEnergy = 0;
		convEng = 0;
		accConvEng = 0;
		nInteractions = 0;
		nHadrons = 0;
		nEscapes = 0;


		for (Int_t j = 0; j < hadVec->size(); j++) {
			HGCSSGenParticle& hadron = (*hadVec)[j];
			if(hadron.layer() > 0){
				nHadrons = nHadrons + 1;
				TVector3 momVec = hadron.vertexMom();
				TVector3 posVec = hadron.vertexPos();

				hadron_zpos[j]   	= posVec[2];
				hadron_px[j]   		= momVec[0];
				hadron_py[j]   		= momVec[1];
				hadron_pz[j]   		= momVec[2];
				hadron_theta[j]   	= acos(momVec[2]) * 180/3.14;
				hadron_pdgid[j]   	= hadron.pdgid();
				hadron_KE[j]		= hadron.vertexKE();
				std::cout << "The hadron_KE[j] at j =  " << j << " is being filled with " << hadron.vertexKE()<< std::endl;

				std::cout << "The hadron_KE[j] at j = " << j << " is " << hadron_KE[j] << std::endl;

				out_KE[nInteractions - 1] += hadron_KE[j];
				nSecondaries[nInteractions - 1] += 1;
				bool acc = false;
				if (hadron_theta[j] < 30){
					nContainedSecondaries[nInteractions - 1] += 1;
					acc = true;
				}
				else
					nUncontainedSecondaries[nInteractions - 1] += 1;


				if (abs(hadron_pdgid[j]) == 2112 || abs(hadron_pdgid[j])  == 2212){
					convEng += hadron_KE[j];
					if (acc) accConvEng += hadron_KE[j];
					out_Eff[nInteractions - 1] += hadron_KE[j];
					if (abs(hadron_pdgid[j]) == 2112){
						nNeutronSecondaries[nInteractions - 1] += 1;
						if (hadron_pdgid[j] > 0)
							out_NE[nInteractions - 1] += hadron_KE[j];
						else{
							out_NE[nInteractions - 1] += hadron_KE[j] +  hadron.mass() ;
							//convEng +=  hadron.mass();
							if (acc) accConvEng += hadron_KE[j];
							out_Eff[nInteractions - 1]+=  hadron.mass();
						}

					}

					if (abs(hadron_pdgid[j]) == 2212){
						nProtonSecondaries[nInteractions - 1] += 1;
						if (hadron_pdgid[j] > 0)
							out_PE[nInteractions - 1] += hadron_KE[j];
						else{
							out_PE[nInteractions - 1] += hadron_KE[j] +  hadron.mass() ;
							//convEng +=  hadron.mass();
							if (acc) accConvEng += hadron_KE[j];
							out_Eff[nInteractions - 1] +=  hadron.mass();
						}

					}

				}
				else if (abs(hadron_pdgid[j]) <10000  && abs(hadron_pdgid[j]) != 22 && abs(hadron_pdgid[j]) != 11
						&& hadron_pdgid[j] != 0){
					convEng += hadron_KE[j];//+hadron.mass();
					if (acc) accConvEng += hadron_KE[j]+hadron.mass();

					out_OE[nInteractions - 1] += hadron_KE[j]+hadron.mass();
					out_Eff[nInteractions - 1] +=  hadron.mass();
					nOtherSecondaries[nInteractions - 1] += 1;

				}

			}
			else{
				TVector3 momVec = hadron.vertexMom();
				TVector3 posVec = hadron.vertexPos();

				inc_KE[nInteractions] = hadron.vertexKE();
				inc_zpos[nInteractions] = posVec[2];
				inc_theta[nInteractions] = acos(momVec[2])*180/3.14;
				inc_pdgid[nInteractions] = hadron.pdgid();


				nInteractions = nInteractions + 1;
				nSecondaries[nInteractions - 1] = 0;
				nNeutronSecondaries[nInteractions - 1] = 0;
				nOtherSecondaries[nInteractions - 1] = 0;
				nContainedSecondaries[nInteractions - 1] = 0;
				nUncontainedSecondaries[nInteractions - 1] = 0;
				out_PE[nInteractions - 1] = 0;
				out_NE[nInteractions - 1] = 0;
				out_OE[nInteractions - 1] = 0;
				out_Eff[nInteractions - 1] = 0;
				out_KE[nInteractions - 1] = 0;

			}
		}

		for (Int_t j = 0; j < escapeVec->size(); j++) {
			HGCSSGenParticle& escape = (*escapeVec)[j];
			nEscapes = nEscapes + 1;
			TVector3 momVec = escape.vertexMom();
			TVector3 posVec = escape.vertexPos();
			escape_xpos[j]   	= posVec[0];
			escape_ypos[j]   	= posVec[1];
			escape_zpos[j]   	= posVec[2];
			escape_px[j]   		= momVec[0];
			escape_py[j]   		= momVec[1];
			escape_pz[j]   		= momVec[2];
			escape_theta[j]   	= acos(momVec[2]) * 180/3.14;

			escape_pdgid[j]   	= escape.pdgid();
			escape_VKE[j]		= escape.vertexKE();
			escape_FKE[j]			= escape.finalKE();

			if (escape_pdgid[j] == -2112 || escape_pdgid[j]  == -2212){
				//lostEnergy += 2* escape.mass();
				lostEnergy += escape_FKE[j];
			}
			else if (escape_pdgid[j] == 2112 || escape_pdgid[j]  == 2212){
				lostEnergy += escape_FKE[j];
			}
			else if (abs(escape_pdgid[j]) < 10000 && escape_pdgid[j] != 0){
				lostEnergy += escape_FKE[j]+escape.mass();
			}
		}
		t1.Fill();
	}
	t1.Write();

	return 1;
}
