#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "TFile.h"
#include "TColor.h"
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

        std::string inputf = argv[1];
	TFile *infile = TFile::Open(inputf.c_str());
	TTree *tree = (TTree*) infile->Get("RecoTree");

	std::vector<HGCSSRecoHit> * hitVec = 0;
	tree->SetBranchAddress("HGCSSRecoHitVec", &hitVec);
        TFile *outfile = TFile::Open("analyzed_digi.root","RECREATE");

        for (int layer = 0; layer < 42; layer++) {
                
                std::string cName = (boost::format("Hits in Layer %s")  %layer).str();
                std::string pName = (boost::format("nHits_ECAL_%s") %layer).str();
                std::string eName = (boost::format("EmipHits_ECAL_%s") %layer).str();

                const char *canvasName = cName.c_str();
                const char *nhitsName  = pName.c_str();
                const char *energyName = eName.c_str();

                TH2Poly *hits   = (TH2Poly*)infile->Get(nhitsName);
                TH2Poly *energy = (TH2Poly*)infile->Get(energyName);
                          
                for (int i = 0; i < hits->GetNbinsX()-2; i++) {
                    for (int j = 0; j < hits->GetNbinsY()-2; j++) {

                        //std::cout << "BINX: " << i << " BINY: " << j << " BIN: " << hits->GetBin(i,j) << " CONTENT: " << hits->GetBinContent(hits->GetBin(i,j)) << std::endl;
                        double Bin = hits->GetBin(i,j);
                        double newBinContent = std::ceil(hits->GetBinContent(Bin)/3);
                        hits->SetBinContent(Bin,newBinContent);
                    }
                }

                TCanvas *myCanvas = new TCanvas(canvasName, canvasName, 800, 800);

                hits->GetYaxis()->SetRangeUser(-200,190);
                energy->GetYaxis()->SetRangeUser(-200,190);
                hits->GetXaxis()->SetRangeUser(-200,200);
                energy->GetXaxis()->SetRangeUser(-200,200);
                hits->GetXaxis()->SetTitle("Position (cm)");
                energy->GetXaxis()->SetTitle("Position (cm)");
                hits->GetYaxis()->SetTitle("Position (cm)");
                energy->GetYaxis()->SetTitle("Position (cm)");
                hits->SetTitle(canvasName);
                energy->SetTitle(energyName);

                const Int_t NRGBs = 5;
                const Int_t NCont = 255;

                Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
                Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
                Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
                Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
                TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
                hits->SetContour(NCont);
                gStyle->Draw("colz"); 
                outfile->cd();
                hits->Write();
                energy->Write();
        }
	
	/*
        TH1F* h_energies = new TH1F("h_energies","; hit energy (MeV); N", 100, 0, 1000); 
	TH1F* h_summedE = new TH1F("h_summedE","; sum of hit energy (MeV); N", 100, 0, 1000); 
	TH1F* h_hits = new TH1F("h_hits","; n hits; N", 100, 0, 200); 
	TH2F* h_energy_layer = new TH2F("h_energy_layer","; layer; hit energy (MeV)", 26, 0.5, 26.5, 100, 0, 500); 
	TH1F* h_hits_layer = new TH1F("h_hits_layer","; layer; n hits", 26, 0.5, 26.5);

	double cur_summedE = 0.;
	unsigned nEvts = tree->GetEntries();
	//std::cout << "nEvts = " << nEvts << std::endl;
	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);
		// std::cout << "number of hits = " << hitVec->size() << std::endl;
		h_hits->Fill(hitVec->size());
		cur_summedE = 0.;
		for (unsigned int j = 0; j < hitVec->size(); ++j ){
			h_energies->Fill(hitVec->at(j).energy());
			h_energy_layer->Fill(hitVec->at(j).layer(), hitVec->at(j).energy());
			h_hits_layer->Fill(hitVec->at(j).layer());
			cur_summedE += hitVec->at(j).energy();
		}
		h_summedE->Fill(cur_summedE);
		if (ievt > 20000) break;
	}
*/
//	outfile->cd();
/*	h_summedE->Write();
	h_energies->Write();
	h_energy_layer->Write();
	h_hits->Write();
	h_hits_layer->Write();
        hits->Write();*/
	outfile->Close();

	return 1;
}
