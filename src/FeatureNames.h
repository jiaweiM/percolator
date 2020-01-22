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
#ifndef FEATURENAMES_H_
#define FEATURENAMES_H_

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cctype>
#include <iterator>

using namespace std;

class FeatureNames {
public:
	FeatureNames();

	virtual ~FeatureNames();

	std::string getFeatureNames(bool skipDOC = false);

	inline std::string getFeatureName(unsigned int index) { return featureNames.at(index); }

	// @return feature 数目，如果包含 calcDOC，为其它feature数+4，否则为其它feature数
	static inline size_t getNumFeatures() {
		return numFeatures;
	}

	// 设置 feature 数目，如果包含 calcDOC，为其它feature数+4，否则为其它feature数
	static inline void setNumFeatures(size_t nf) {
		if (!numFeatures) {
			numFeatures = nf;
		}
	}

	// 重置 feature 数为0
	static inline void resetNumFeatures() {
		numFeatures = 0;
	}

	// 初始化 features，即考虑是否加入 calcDOc 所需的 features
	// @param calcDOC 是否计算 DOC
	void initFeatures(bool calcDOC);

	// 添加一个新的 feature 名称
	// @featureName feature 名称
	void insertFeature(const string& featureName) {

		if (std::find(featureNames.begin(), featureNames.end(), featureName) == featureNames.end())
			featureNames.push_back(featureName);
	}

	int getFeatureNumber(const string& featureName);

	int getMinCharge() {
		return minCharge;
	}
	int getMaxCharge() {
		return maxCharge;
	}
	int getChargeFeatNum() {
		assert(1 == 0);
		return chargeFeatNum;
	}
	int getEnzFeatNum() {
		assert(1 == 0);
		return enzFeatNum;
	}
	int getNumSPFeatNum() {
		assert(1 == 0);
		return numSPFeatNum;
	}
	int getPtmFeatNum() {
		assert(1 == 0);
		return ptmFeatNum;
	}
	int getIntraSetFeatNum() {
		assert(1 == 0);
		return intraSetFeatNum;
	}
	int getQuadraticFeatNum() {
		assert(1 == 0);
		return quadraticFeatNum;
	}
	int getDocFeatNum() {
		return docFeatNum;
	}
	void setDocFeatNum(int fn) {
		docFeatNum = fn;
	}
protected:
	// 保存所有 feature 名称，不包括 id,label和可选字段
	vector<string> featureNames;
	static size_t numFeatures;
	int minCharge, maxCharge;
	int chargeFeatNum, enzFeatNum, numSPFeatNum, ptmFeatNum,
		intraSetFeatNum, quadraticFeatNum, docFeatNum;
};

#endif /*FEATURENAMES_H_*/
