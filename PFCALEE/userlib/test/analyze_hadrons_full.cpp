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

bool checkDuplicate(std::vector<double> oldEng,double newEng){
	for (unsigned i = 0; i < oldEng.size(); i++){
		if ( (oldEng.at(i) - newEng) < 1) return false;
	}
	return true;
}
int main(int argc, char** argv) {

	//std::cout << "Opening the file " << argv[1] << std::endl;
	TFile *infile = TFile::Open(argv[1]);
	TTree *tree = (TTree*) infile->Get("HGCSSTree");
	freopen("log.txt", "w", stdout);

	HGCSSEvent* evt_ = 0;
	tree->SetBranchAddress("HGCSSEvent", &evt_);

	std::vector<HGCSSGenParticle> * incVec = 0;
	tree->SetBranchAddress("HGCSSIncAction", &incVec);

	std::vector<HGCSSGenParticle> * hadVec = 0;
	tree->SetBranchAddress("HGCSSHadAction", &hadVec);

	std::vector<HGCSSGenParticle> * escapeVec = 0;
	tree->SetBranchAddress("HGCSSEscapeAction", &escapeVec);

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("sampling", "Hadronic Study");

	Int_t nInteractions,nSecondaries[50000],nProtonSecondaries[50000],nNeutronSecondaries[50000],
	nOtherSecondaries[50000],nContainedSecondaries[50000],nUncontainedSecondaries[50000];

	t1.Branch("nInteractions", &nInteractions, "nInteractions/I");
	t1.Branch("nSecondaries", &nSecondaries, "nSecondaries[nInteractions]/I");
	t1.Branch("nProtonSecondaries", &nProtonSecondaries, "nProtonSecondaries[nInteractions]/I");
	t1.Branch("nNeutronSecondaries", &nNeutronSecondaries, "nNeutronSecondaries[nInteractions]/I");
	t1.Branch("nOtherSecondaries", &nOtherSecondaries, "nOtherSecondaries[nInteractions]/I");
	t1.Branch("nContainedSecondaries", &nContainedSecondaries, "nContainedSecondaries[nInteractions]/I");
	t1.Branch("nUncontainedSecondaries", &nUncontainedSecondaries, "nUncontainedSecondaries[nInteractions]/I");

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

	Float_t summedSen,summedSenWgt,convEng_1,accconvEng_1,lostEng_1,convEng_2,accconvEng_2,lostEng_2;

	t1.Branch("convEng_1", &convEng_1, "convEng_1/F");
	t1.Branch("accconvEng_1", &accconvEng_1, "accconvEng_1/F");
	t1.Branch("lostEng_1", &lostEng_1, "lostEng_1/F");

	t1.Branch("convEng_2", &convEng_2, "convEng_2/F");
	t1.Branch("accconvEng_2", &accconvEng_2, "accconvEng_2/F");
	t1.Branch("lostEng_2", &lostEng_2, "lostEng_2/F");


	t1.Branch("summedSen", &summedSen, "summedSen/F");
	t1.Branch("summedSenWgt", &summedSenWgt, "summedSenWgt/F");



	std::vector<double> hadronKEs;

	unsigned nEvts = tree->GetEntries();
	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		//std::cout << "The event is ievt = " << ievt << std::endl;
		tree->GetEntry(ievt);
		summedSen = evt_->dep();
		summedSenWgt = evt_->wgtDep();
		if (summedSen == 0) continue;

		lostEng_1 = 0;
		convEng_1 = 0;
		accconvEng_1 = 0;


		lostEng_2 = 0;
		convEng_2 = 0;
		accconvEng_2 = 0;


		nInteractions = 0;
		nHadrons = 0;
		nEscapes = 0;

		//std::cout << "The incVec size is = " << incVec->size() << std::endl;

		for (Int_t j = 0; j < incVec->size(); j++) {

			//std::cout << "Looping over inc part = " << j << std::endl;


			HGCSSGenParticle& incPart = (*incVec)[j];
			//std::cout << "quering vertex mom = " << j << std::endl;

			TVector3 momVec = incPart.vertexMom();
			//std::cout << "quering vertex pos = " << j << std::endl;

			TVector3 posVec = incPart.vertexPos();
			//std::cout << "quering interaction = " << j << std::endl;

			//std::cout << "incPart.layer() is " << incPart.layer() << std::endl;

			unsigned iLoc		=	-incPart.layer() - 1;


			//std::cout << "storing ke = " << j << std::endl;

			inc_KE[iLoc] = incPart.vertexKE();
			//std::cout << "storing zpos = " << j << std::endl;

			inc_zpos[iLoc] = posVec[2];
			//std::cout << "storing momvec = " << j << std::endl;

			inc_theta[iLoc] = acos(momVec[2])*180/3.14;
			//std::cout << "storing pdgid = " << j << std::endl;

			inc_pdgid[iLoc] = incPart.pdgid();
			//std::cout << "resetting counters  " << j << std::endl;

			//nInteractions = iLoc + 1;
			nSecondaries[iLoc] = 0;
			nNeutronSecondaries[iLoc] = 0;
			nProtonSecondaries[iLoc] = 0;
			nOtherSecondaries[iLoc] = 0;
			nContainedSecondaries[iLoc] = 0;
			nUncontainedSecondaries[iLoc] = 0;
			out_PE[iLoc] = 0;
			out_NE[iLoc] = 0;
			out_OE[iLoc] = 0;
			out_Eff[iLoc] = 0;
			out_KE[iLoc] = 0;

		}

		//std::cout << "The hadronvec size is = " << hadVec->size() << std::endl;
		for (Int_t j = 0; j < hadVec->size(); j++) {
			HGCSSGenParticle& hadron = (*hadVec)[j];

			//unsigned int hadronTrackLoc = std::find(hadronKEs.begin(),
			//			hadronKEs.end(), hadron.vertexKE())
			//		- hadronKEs.begin();
			//if (hadronTrackLoc != hadronKEs.size()) continue;
			//std::cout << "Looping over hadron part = " << j << std::endl;
			//if (hadron.layer() > 100)
			//		std::cout << "The hadron KE is " << hadron.vertexKE();
			if (checkDuplicate(hadronKEs,hadron.vertexKE()) == false) continue;
			hadronKEs.push_back(hadron.vertexKE());
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
			unsigned iLoc		=	hadron.layer() - 1;
			if (iLoc > nInteractions) nInteractions = iLoc;

			out_KE[iLoc] += hadron_KE[j];
			nSecondaries[iLoc] += 1;
			bool acc = false;
			if (hadron_theta[j] < 30){
				nContainedSecondaries[iLoc] += 1;
				acc = true;
			}
			else
				nUncontainedSecondaries[iLoc] += 1;

			//Do hadron stuff
			if (abs(hadron_pdgid[j]) == 2112 || abs(hadron_pdgid[j])  == 2212){
				convEng_1 += hadron_KE[j];
				convEng_2 += hadron_KE[j];
				if (acc) {
					accconvEng_1 += hadron_KE[j];
					accconvEng_2 += hadron_KE[j];
				}
				out_Eff[iLoc] += hadron_KE[j];


				// Do neutron stuff
				if (abs(hadron_pdgid[j]) == 2112){
					nNeutronSecondaries[iLoc] = nNeutronSecondaries[iLoc] + 1;
					if (hadron_pdgid[j] > 0)
						out_NE[iLoc] += hadron_KE[j];
					else{
						out_NE[iLoc] += hadron_KE[j] +  2*hadron.mass() ;
						convEng_1 +=  2*hadron.mass();

						out_Eff[iLoc]+=  2*hadron.mass();

						if (acc) accconvEng_1 += 2*hadron.mass();

					}

				}
				//Do Proton stuff
				if (abs(hadron_pdgid[j]) == 2212){
					nProtonSecondaries[iLoc] += 1;
					if (hadron_pdgid[j] > 0)
						out_PE[iLoc] += hadron_KE[j];
					else{
						out_PE[iLoc] += hadron_KE[j] +  hadron.mass() ;
						convEng_1 +=  2*hadron.mass();

						out_Eff[iLoc] +=  2*hadron.mass();

						if (acc) accconvEng_1 += 2*hadron.mass();
					}

				}

			}
			else if (abs(hadron_pdgid[j]) <10000  && abs(hadron_pdgid[j]) != 22 && abs(hadron_pdgid[j]) != 11
					&& hadron_pdgid[j] != 0){
				convEng_1 += hadron_KE[j] + hadron.mass();
				convEng_2 += hadron_KE[j];

				if (acc) {
					accconvEng_1 += hadron_KE[j]+hadron.mass();
					accconvEng_2 += hadron_KE[j];
				}

				out_OE[iLoc] += hadron_KE[j]+hadron.mass();
				out_Eff[iLoc] +=  hadron.mass();
				nOtherSecondaries[iLoc] += 1;

			}
		}
		//std::cout << "The escapeVec size is = " << incVec->size() << std::endl;

		for (Int_t j = 0; j < escapeVec->size(); j++) {
			//std::cout << "Looping over escape part = " << j << std::endl;

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
				lostEng_1 += 2* escape.mass();
				lostEng_1 += escape_FKE[j];
				lostEng_2 += escape_FKE[j];

			}
			else if (escape_pdgid[j] == 2112 || escape_pdgid[j]  == 2212){
				lostEng_1 += escape_FKE[j];
				lostEng_2 += escape_FKE[j];

			}
			else if (abs(escape_pdgid[j]) < 10000 && escape_pdgid[j] != 0){
				lostEng_1 += escape_FKE[j]+escape.mass();
				lostEng_2 += escape_FKE[j];

			}
		}
		t1.Fill();
		hadronKEs.clear();
	}
	t1.Write();

	return 1;
}
