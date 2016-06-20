#include "G4VPhysicalVolume.hh"

#include "SamplingSection.hh"

//
G4bool SamplingSection::add(G4double depositRawE,  G4VPhysicalVolume* vol) {
	std::string lstr = vol->GetName();
	bool breakSwitch = false;
	for (unsigned ie(0); ie < n_elements * n_sectors; ++ie) {
			if (breakSwitch) return true;
		if (sublayer_vol[ie] && lstr == sublayer_vol[ie]->GetName()) {
			breakSwitch = true;
			unsigned idx = getSensitiveLayerIndex(lstr);
			unsigned eleidx = ie % n_elements;
			sublayer_RawDep[eleidx] += depositRawE;

			} //if in right material
		} //loop on available materials
	return false;
	}

//
void SamplingSection::report(bool header) {
	if (header)
		G4cout
				<< "E/[MeV]\t  Si\tAbsorber\tTotal\tSi g frac\tSi e frac\tSi mu frac\tSi had frac\tSi <t> \t nG4SiHits"
				<< G4endl;
		G4cout << std::setprecision(3) << "\t  " << getMeasuredEnergy(false)
		<< G4endl;
	}

G4double SamplingSection::getMeasuredEnergy(bool weighted) {

	return getTotalSensE();

}
void SamplingSection::trackParticleHistory(const unsigned & idx,
		const G4SiHitVec & incoming) {
	for (unsigned iP(0); iP < sens_HitVec[idx].size(); ++iP) { //loop on g4hits
		G4int parId = sens_HitVec[idx][iP].parentId;
		for (unsigned iI(0); iI < incoming.size(); ++iI) { //loop on previous layer
			G4int trId = incoming[iI].trackId;
			if (trId == parId)
				sens_HitVec[idx][iP].parentId = incoming[iI].parentId;
		} //loop on previous layer
	} //loop on g4hits

}
