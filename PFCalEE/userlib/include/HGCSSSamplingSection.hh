#ifndef _hgcsssamplingsection_hh_
#define _hgcsssamplingsection_hh_
#include <iomanip>
#include <vector>
#include "Rtypes.h"
#include <sstream>
#include <map>

class HGCSSSamplingSection {

public:
	HGCSSSamplingSection() :
			volNb_(0), volX0trans_(0), voldEdx_(0), volLambdatrans_(0), measuredE_(
					0), absorberE_(0), totalE_(0), gFrac_(0), eFrac_(0), muFrac_(
					0), neutronFrac_(0), hadFrac_(0), muKin_(0), neutronKin_(0), hadKin_(
					0), avgTime_(0), nSiHits_(0), eleWgtCnt_(0), hadWgtCnt_(0), neutWgtCnt_(
					0), muWgtCnt_(0),neutronCount_(0),hadCount_(0),muCount_(0) {

	}
	;

	~HGCSSSamplingSection() {
	}
	;
	//getters
	inline unsigned volNb() const {
		return volNb_;
	}
	;

	inline double volX0trans() const {
		return volX0trans_;
	}
	;

	inline double voldEdx() const {
		return voldEdx_;
	}
	;

	inline double volLambdatrans() const {
		return volLambdatrans_;
	}
	;
	inline double measuredE() const {
		return measuredE_;
	}
	;
	inline double absorberE() const {
		return absorberE_;
	}
	;
	inline double totalE() const {
		return totalE_;
	}
	;
	inline double gFrac() const {
		return gFrac_;
	}
	;
	inline double eFrac() const {
		return eFrac_;
	}
	;
	inline double muFrac() const {
		return muFrac_;
	}
	;
	inline double muKin() const {
		return muKin_;
	}
	;
	inline double neutronFrac() const {
		return neutronFrac_;
	}
	;
	inline double neutronKin() const {
		return neutronKin_;
	}
	;
	inline double hadFrac() const {
		return hadFrac_;
	}
	;
	inline double hadKin() const {
		return hadKin_;
	}
	;
	inline double avgTime() const {
		return avgTime_;
	}
	;
	inline double eleWgtCnt() {
		return eleWgtCnt_;
	}
	;

	inline double hadWgtCnt() {
		return hadWgtCnt_;
	}
	;
	inline double neutWgtCnt() {
		return neutWgtCnt_;
	}
	;
	inline double muWgtCnt() {
		return muWgtCnt_;
	}
	;
	inline unsigned nSiHits() const {
		return nSiHits_;
	}
	;

	inline unsigned muCount() {
		return muCount_;
	}
	;
	inline unsigned hadCount() {
		return hadCount_;
	}
	;
	inline unsigned neutronCount() const {
		return neutronCount_;
	}
	;

	//setters
	inline void volNb(const unsigned & aVal) {
		volNb_ = aVal;
	}
	;
	inline void volX0trans(const double & aVal) {
		volX0trans_ = aVal;
	}
	;
	inline void voldEdx(const double & aVal) {
		voldEdx_ = aVal;
	}
	;
	inline void volLambdatrans(const double & aVal) {
		volLambdatrans_ = aVal;
	}
	;
	inline void sensDep(const double & aVal) {
		measuredE_ = aVal;
	}
	;
	inline void absorberDep(const double & aVal) {
		absorberE_ = aVal;
	}
	;
	inline void totalDep(const double & aVal) {
		totalE_ = aVal;
	}
	;
	inline void gDepFrac(const double & aVal) {
		gFrac_ = aVal;
	}
	;
	inline void eDepFrac(const double & aVal) {
		eFrac_ = aVal;
	}
	;
	inline void muDepFrac(const double & aVal) {
		muFrac_ = aVal;
	}
	;
	inline void neutronDepFrac(const double & aVal) {
		neutronFrac_ = aVal;
	}
	;
	inline void hadDepFrac(const double & aVal) {
		hadFrac_ = aVal;
	}
	;
	inline void avgTime(const double & aVal) {
		avgTime_ = aVal;
	}
	;
	inline void nSiHits(const unsigned & aVal) {
		nSiHits_ = aVal;
	}
	;
	inline void muKinFlux(const double & aVal) {
		muKin_ = aVal;
	}
	;
	inline void neutronKinFlux(const double & aVal) {
		neutronKin_ = aVal;
	}
	;
	inline void hadKinFlux(const double & aVal) {
		hadKin_ = aVal;
	}
	;

	inline void eleShowerSize(const double & aVal) {
		eleWgtCnt_ = aVal;
	}
	;
	inline void hadronShowerSize(const double & aVal) {
		neutWgtCnt_ = aVal;
	}
	;
	inline void neutronShowerSize(const double & aVal) {
		hadWgtCnt_ = aVal;
	}
	;
	inline void muShowerSize(const double & aVal) {
		muWgtCnt_ = aVal;
	}
	;

	inline void muCount(const double & aVal) {
		muCount_ = aVal;
	}
	;
	inline void hadCount(const double & aVal) {
		hadCount_ = aVal;
	}
	;
	inline void neutronCount(const double & aVal) {
		neutronCount_ = aVal;
	}
	;
private:
	unsigned volNb_;
	double volX0trans_;
	double voldEdx_;
	double volLambdatrans_;
	double measuredE_;
	double absorberE_;
	double totalE_;
	double gFrac_;
	double eFrac_;
	double muFrac_;
	double neutronFrac_;
	double hadFrac_;
	double avgTime_;
	unsigned nSiHits_;

	double neutronKin_;
	double muKin_;
	double hadKin_;
	double eleWgtCnt_;
	double hadWgtCnt_;
	double neutWgtCnt_;
	double muWgtCnt_;

	unsigned muCount_;
	unsigned hadCount_;
	unsigned neutronCount_;

ClassDef(HGCSSSamplingSection,1)
	;

};

typedef std::vector<HGCSSSamplingSection> HGCSSSamplingSectionVec;

#endif

