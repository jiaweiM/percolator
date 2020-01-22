/*******************************************************************************
 Copyright 2006-2012 Lukas Käll <lukas.kall@scilifelab.se>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 *******************************************************************************/
#ifndef PSMDESCRIPTION_H_
#define PSMDESCRIPTION_H_
#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "Enzyme.h"

 /*
 * PSMDescription
 *
 * Here are some useful abbreviations:
 * PSM - Peptide Spectrum Match
 *
 */
class PSMDescription {

public:
	PSMDescription();
	PSMDescription(const std::string& peptide);

	virtual ~PSMDescription();
	static void deletePtr(PSMDescription* psm);
	virtual void deleteRetentionFeatures() {}

	void clear() { proteinIds.clear(); }
	double* getFeatures() { return features; }

	// TODO: move these static functions somewhere else
	static std::string removePTMs(const std::string& peptideSeq);
	static std::string removeFlanks(const std::string& peptideSeq) {
		return peptideSeq.substr(2, peptideSeq.size() - 4);
	}
	static bool ptrLess(PSMDescription* one, PSMDescription* other) {
		return *one < *other;
	}
	static bool ptrEqual(PSMDescription* one, PSMDescription* other) {
		return *one == *other;
	}

	std::string getPeptideSequence() { return peptide.substr(2, peptide.size() - 4); }

	std::string& getFullPeptideSequence() { return peptide; }

	std::string getFlankN() { return peptide.substr(0, 1); }

	std::string getFlankC() { return peptide.substr(peptide.size() - 1, peptide.size()); }

	bool isNotEnzymatic();

	friend std::ostream& operator<<(std::ostream& out, PSMDescription& psm);
	void printProteins(std::ostream& out);

	bool operator<(const PSMDescription& other) const {
		return (peptide < other.peptide) ||
			(peptide == other.peptide && getRetentionTime() < other.getRetentionTime());
	}

	bool operator==(const PSMDescription& other) const {
		return (peptide == other.peptide);
	}

	// set the id of the PSM
	virtual inline void setId(const std::string& id) { id_ = id; }
	virtual inline std::string& getId() { return id_; }

	// Virtual functions for PSMDescriptionDOC
	virtual std::string& getFullPeptide() { return peptide; }
	virtual PSMDescription* getAParent() { return this; }
	virtual void checkFragmentPeptides(
		std::vector<PSMDescription*>::reverse_iterator other,
		std::vector<PSMDescription*>::reverse_iterator theEnd) {}

	// 设置 rt features
	virtual void setRetentionFeatures(double* retentionFeatures) {}

	// RT features is null if not docCal
	virtual double* getRetentionFeatures() { return NULL; }

	virtual void setParentFragment(PSMDescription*) {}

	virtual PSMDescription* getParentFragment() { return NULL; }

	// 设置 PSM 保留时间
	virtual inline void setRetentionTime(const double retentionTime) {}
	virtual inline double getRetentionTime() const { return 0.0; }

	virtual inline void setUnnormalizedRetentionTime(const double retentionTime) {}
	virtual inline double getUnnormalizedRetentionTime() const {
		std::cerr << "Warning: no retention time available" << std::endl;
		return 0.0;
	}
	// set mass diff in da
	virtual inline void setMassDiff(const double dm) {}
	virtual inline double getMassDiff() const {
		std::cerr << "Warning: no mass difference available" << std::endl;
		return 0.0;
	}

	// 设置肽段等电点
	virtual inline void setIsoElectricPoint(const double pI) {}

	// 肽段等电点，如果不计算 docCalc，为0.0
	virtual inline double getIsoElectricPoint() const {
		std::cerr << "Warning: no iso electric point available" << std::endl;
		return 0.0;
	}

	virtual inline void setPredictedRetentionTime(const double predictedTime) {}

	virtual inline double getPredictedRetentionTime() const {
		std::cerr << "Warning: no retention time available" << std::endl;
		return 0.0;
	}

	// PSM 的 features，不包括 docFeatures
	double* features; // owned by a FeatureMemoryPool instance, no need to delete
	// PSM 母离子质量
	double expMass;
	// PSM 肽段质量
	double calcMass;
	// PSM 的 scan number，如果 scannr 不存在，用行号代替
	unsigned int scan;
	std::string id_;
	// 肽段序列
	std::string peptide;
	std::vector<std::string> proteinIds;
};

inline std::ostream& operator<<(std::ostream& out, PSMDescription& psm) {

	out << "Peptide: " << psm.peptide << endl;
	out << "Spectrum scan number: " << psm.scan << endl;
	out << endl;
	return out;
}

#endif /*PSMDESCRIPTION_H_*/
