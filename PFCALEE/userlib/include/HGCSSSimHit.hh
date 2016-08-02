#ifndef _hgcsssimhit_hh_
#define _hgcsssimhit_hh_

#include <iomanip>
#include <vector>
#include "Rtypes.h"
#include <sstream>
#include <iostream>
#include <map>
#include "G4RunManager.hh"

#include "G4SiHit.hh"
#include "HGCSSGeometryConversion.hh"

#include "Math/Point3D.h"
#include "Math/Point3Dfwd.h"
#include "TH2Poly.h"

//for hexagons: side size.
static const float CELL_SIZE_X = 4.59360; //2.5;//mm
static const float CELL_SIZE_Y = CELL_SIZE_X;

class HGCSSSimHit {

public:
	HGCSSSimHit() :
			energy_(0), time_(0), zpos_(0), layer_(0), cellid_(0), nGammas_(0), nElectrons_(
					0), nMuons_(0), nNeutrons_(0), nProtons_(0), nHadrons_(0), trackIDMainParent_(
					0), eDepMainParent_(0), pdgIDMainParent_(0), parentEng_(0) {

	}
	;
	HGCSSSimHit(const G4SiHit & aSiHit,  TH2Poly* map,
			const float cellSize = CELL_SIZE_X);

	~HGCSSSimHit() {
	}
	;

	inline G4double energy() const {
		return energy_;
	}
	;

	inline G4double time() const {
		return time_;
	}
	;

	inline void calculateTime() {
		if (energy_ > 0)
			time_ = time_ / energy_;
	}
	;

	inline unsigned layer() const {
		return layer_ / 3;
	}
	;

	inline unsigned silayer() const {
		return layer_ % 3;
	}
	;

	//re-encode local layer into det layer + si layer if several sensitive layers (up to 3...)
	inline void setLayer(const unsigned & layer, const unsigned & silayer) {
		if (silayer > 4) {
			std::cerr << " ERROR! Trying to add silayer " << silayer
					<< ", should be less than 3..." << std::endl;
			exit(1);
		}
		layer_ = 3 * layer + silayer;
		//if (silayer>0) std::cout << layer_ << " " << layer << " " << silayer << std::endl;
	}
	;

	inline unsigned cellid() const {
		return cellid_;
	}
	;

	inline unsigned nGammas() const {
		return nGammas_;
	}
	;

	inline unsigned nElectrons() const {
		return nElectrons_;
	}
	;

	inline unsigned nMuons() const {
		return nMuons_;
	}
	;

	inline unsigned nNeutrons() const {
		return nNeutrons_;
	}
	;

	inline unsigned nProtons() const {
		return nProtons_;
	}
	;
	inline unsigned nHadrons() const {
		return nHadrons_;
	}
	;
	inline unsigned numberOfParticles() const {
		return nGammas_ + nElectrons_ + nMuons_ + nNeutrons_ + nProtons_
				+ nHadrons_;
	}
	;

	inline G4double gFrac() const {
		return nGammas_ / numberOfParticles();
	}
	;

	inline G4double eFrac() const {
		return nElectrons_ / numberOfParticles();
	}
	;

	inline G4double muFrac() const {
		return nMuons_ / numberOfParticles();
	}
	;

	inline G4double neutronFrac() const {
		return nNeutrons_ / numberOfParticles();
	}
	;

	inline G4double protonFrac() const {
		return nProtons_ / numberOfParticles();
	}
	;

	inline G4double hadFrac() const {
		return nHadrons_ / numberOfParticles();
	}
	;

	void Add(const G4SiHit & aSiHit);

	std::pair<G4double, G4double> get_xy(const bool isScintillator,
			const HGCSSGeometryConversion & aGeom) const;

	ROOT::Math::XYZPoint position(const bool isScintillator,
			const HGCSSGeometryConversion & aGeom) const;

	inline G4double get_z() const {
		return zpos_;
	}
	;

	G4double eta(const bool isScintillator,
			const HGCSSGeometryConversion & aGeom) const;
	G4double theta(const bool isScintillator,
			const HGCSSGeometryConversion & aGeom) const;
	G4double phi(const bool isScintillator,
			const HGCSSGeometryConversion & aGeom) const;

	inline unsigned getGranularity() const {
		return 1;
	}
	;

	inline int mainParentTrackID() const {
		return trackIDMainParent_;
	}
	;

	inline G4double mainParentEfrac() const {
		return eDepMainParent_ / energy_;
	}
	;

	void Print(std::ostream & aOs) const;

private:

	G4double energy_;
	G4double parentEng_;
	G4double time_;
	G4double zpos_;
	G4double pdgIDMainParent_;
	unsigned layer_;
	unsigned cellid_;
	unsigned nGammas_;
	unsigned nElectrons_;
	unsigned nMuons_;
	unsigned nNeutrons_;
	unsigned nProtons_;
	unsigned nHadrons_;
	int trackIDMainParent_;
	G4double eDepMainParent_;
	G4double KEMainParent_;

ClassDef(HGCSSSimHit,1)
	;

};

typedef std::vector<HGCSSSimHit> HGCSSSimHitVec;

#endif
