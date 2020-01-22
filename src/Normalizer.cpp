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
#include <assert.h>
#include <iostream>
#include <vector>
#include <set>
#include <string>
using namespace std;
#include "Normalizer.h"
#include "StdvNormalizer.h"
#include "UniNormalizer.h"
#include "Globals.h"

int Normalizer::subclass_type = STDV;
Normalizer* Normalizer::theNormalizer = NULL;

Normalizer::Normalizer() {}

Normalizer::~Normalizer() {}

// 正则化数据集
// @param featuresV feature 数据
// @param rtFeaturesV RT 相关的 feature 数据
void Normalizer::normalizeSet(vector<double*>& featuresV, vector<double*>& rtFeaturesV) {

	normalizeSet(featuresV, 0, numFeatures);
	normalizeSet(rtFeaturesV, numFeatures, numRetentionFeatures);
}

// 正则化数据集
// @param featuresV feature 数据集
// @param offset 索引 offset，为RT设计，因为它们一般放在最后
// @param numFeatures 待正则化的 feature 数
void Normalizer::normalizeSet(vector<double*>& featuresV, size_t offset, size_t numFeatures) {

	double* features;
	vector<double*>::iterator it = featuresV.begin();
	for (; it != featuresV.end(); ++it) {
		features = *it;
		normalize(features, features, offset, numFeatures);
	}
}

// normalize feature
// @param in 输入
// @param out 输出
// @param offset index offset
// @param numFeatures number of features
void Normalizer::normalize(const double* in, double* out,
	size_t offset, size_t numFeatures) {

	for (unsigned int ix = 0; ix < numFeatures; ++ix) {
		out[ix] = (in[ix] - sub[offset + ix]) / div[offset + ix];
	}
}

// 获得 Normalizer 实例
// Percolator 提供了两种正则化方法，一般使用标准偏差方法，即使每个特征的均值为0，方差为1。
Normalizer* Normalizer::getNormalizer() {
	if (theNormalizer == NULL) {
		if (subclass_type == UNI) {
			theNormalizer = new UniNormalizer();
		}
		else {
			theNormalizer = new StdvNormalizer();
		}
	}
	return theNormalizer;
}

// 设置正则化方法
// @param type 类型，0 -> UNI, 1 -> STDV
void Normalizer::setType(int type) {
	assert(type == UNI || type == STDV);
	subclass_type = type;
}
