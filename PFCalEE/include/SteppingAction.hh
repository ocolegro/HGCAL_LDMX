#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4EmSaturation.hh"

class EventAction;

class SteppingAction: public G4UserSteppingAction {
public:
	SteppingAction();
	virtual ~SteppingAction();

	void UserSteppingAction(const G4Step*);
	inline unsigned getLayer(std::string astr) {
		size_t pos = astr.find("phys");
		unsigned num = 0;
		if (astr.find("_")== astr.npos) {
			std::string truncated = astr.substr(0,pos);
			size_t last_index = truncated.find_last_not_of("0123456789");
			num = std::atoi(truncated.substr(last_index+1).c_str());

		}
		else{
			pos = astr.find("_");
			std::string truncated = astr.substr(0,pos);
			size_t last_index = truncated.find_last_not_of("0123456789");
			num = std::atoi(truncated.substr(last_index+1).c_str());
		}

		return num;

	};
private:
	EventAction *eventAction_;
	//to correct the energy in the scintillator
	G4EmSaturation* saturationEngine;
	G4double timeLimit_;

};

#endif
