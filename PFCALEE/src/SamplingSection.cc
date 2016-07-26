#include "G4VPhysicalVolume.hh"
#include "SamplingSection.hh"

//
std::pair<G4bool,G4bool> SamplingSection::add(G4double depositRawE,G4VPhysicalVolume* vol,G4Track *lTrack,G4ThreeVector position) {
	std::string lstr = vol->GetName();
	bool breakSwitch = false;
	bool isSens = false;
	for (unsigned ie(0); ie < n_elements * n_sectors; ++ie) {
			if (breakSwitch) return std::make_pair(breakSwitch,isSens);
		if (sublayer_vol[ie] && lstr == sublayer_vol[ie]->GetName()) {
			breakSwitch = true;
			unsigned idx = getSensitiveLayerIndex(lstr);
			unsigned eleidx = ie % n_elements;
			isSens = isSensitiveElement(eleidx);
			sublayer_RawDep[eleidx] += depositRawE;
			if (eventAction_->firstPass() == false){
				G4SiHit lHit;
				std::cout << "The deposited raw energy is getting stored " << std::endl;
				lHit.energyDep = depositRawE;
				std::cout << "The pdgid is " << std::endl;
				lHit.pdgId = lTrack->GetDefinition()->GetPDGEncoding();
				std::cout << "The parent KE is " << std::endl;
				lHit.parentKE = lTrack->GetKineticEnergy();
				std::cout << "The hit_x is " << std::endl;
				lHit.hit_x = position.x();
				std::cout << "The hit_y is " << std::endl;
				lHit.hit_y = position.y();
				std::cout << "The hit_z is " << std::endl;
				lHit.hit_z = position.z();

				sens_HitVec[idx].push_back(lHit);
			}

			} //if in right material
		} //loop on available materials

	return std::make_pair(breakSwitch,isSens);
	}

//
void SamplingSection::report(bool header) {
	if (header)
		G4cout
				<< "E/[MeV]\t  Si\tAbsorber\tTotal\tSi g frac\tSi e frac\tSi mu frac\tSi had frac\tSi <t> \t nG4SiHits"
				<< G4endl;
		G4cout << std::setprecision(3) << "\t  " << getTotalSensE()
		<< G4endl;
	}

const G4SiHitVec & SamplingSection::getSiHitVec(const unsigned & idx) const {
	return sens_HitVec[idx];
}
G4double SamplingSection::getTotalSensE() {
	double etot = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (isSensitiveElement(ie))
			etot += sublayer_RawDep[ie];
	}
	return etot;
}
