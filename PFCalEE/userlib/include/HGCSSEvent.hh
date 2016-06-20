#ifndef _hgcssevent_hh_
#define _hgcssevent_hh_
#include <iomanip>
#include <vector>
#include "Rtypes.h"
#include <sstream>
#include <map>

class HGCSSEvent {

public:
	HGCSSEvent() :
			event_(0) {

	}
	;

	~HGCSSEvent() {
	}
	;

	inline unsigned eventNumber() const {
		return event_;
	}
	;

	inline void eventNumber(const unsigned aNum) {
		event_ = aNum;
	}
	;

	inline double vtx_x() const {
		return xvtx_;
	}
	;
	inline double steelThick() const {
		return steelThick_;
	}
	;
	inline void vtx_x(const double x) {
		xvtx_ = x;
	}
	;

	inline double vtx_y() const {
		return yvtx_;
	}
	;

	inline int seeds() const {
		return seeds_;
	}
	;

	inline void vtx_y(const double y) {
		yvtx_ = y;
	}
	;

	inline double vtx_z() const {
		return zvtx_;
	}
	;

	inline void vtx_z(const double z) {
		zvtx_ = z;
	}
	;
	inline void steelThick(const double thick) {
		steelThick_ = thick;
	}
	;
	inline void seeds(const int seeds) {
		seeds_ = seeds;
	}
	;
private:

	unsigned event_;
	double xvtx_;
	double yvtx_;
	double zvtx_;
	double steelThick_;
	int seeds_;

ClassDef(HGCSSEvent,1)
	;

};

#endif
