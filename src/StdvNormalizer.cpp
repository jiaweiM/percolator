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
#include <iostream>
#ifdef WIN32
#include <float.h>
#define isfinite _finite
#endif
#include <math.h>
#include <vector>
#include <string>

using namespace std;

#include "Normalizer.h"
#include "StdvNormalizer.h"
#include "Globals.h"


StdvNormalizer::StdvNormalizer() {}

StdvNormalizer::~StdvNormalizer() {}

void StdvNormalizer::unnormalizeweight(const std::vector<double>& in,
	std::vector<double>& out) {

	double sum = 0;
	unsigned int i = 0;
	for (; i < numFeatures; i++) {
		out[i] = in[i] / div[i];
		sum += sub[i] * in[i] / div[i];
	}
	out[i] = in[i] - sum;
}

void StdvNormalizer::normalizeweight(const std::vector<double>& in,
	std::vector<double>& out) {

	double sum = 0;
	size_t i = 0;
	for (; i < numFeatures; i++) {
		out[i] = in[i] * div[i];
		sum += sub[i] * in[i];
	}
	out[i] = in[i] + sum;
}

// 设置数据集，计算每种 feature 的均值，保存在 sub 中
// 计算每种 feature 的标准差，保存在 div 中，
// @param featuresV 所有 features
// @param rtFeaturesV rt 相关的 features，对非 docCal 为 空
// @param numFeatures feature 数目
// @param numRetentionFeatures rt feature 数目，不予考虑为0
void StdvNormalizer::setSet(std::vector<double*>& featuresV, std::vector<double*>& rtFeaturesV,
	size_t nf, size_t nrf) {

	numFeatures = nf;
	numRetentionFeatures = nrf;

	sub.resize(nf + nrf, 0.0);
	div.resize(nf + nrf, 0.0);

	double n = 0.0; // feature 总数
	double* features;
	size_t ix;
	vector<double*>::iterator it = featuresV.begin();
	for (; it != featuresV.end(); ++it) {
		features = *it;
		n++;
		for (ix = 0; ix < numFeatures; ++ix) {
			sub[ix] += features[ix]; // sub 现在是每列 feature 值的加和
		}
	}
	for (it = rtFeaturesV.begin(); it != rtFeaturesV.end(); ++it) {
		features = *it;
		for (ix = numFeatures; ix < numFeatures + numRetentionFeatures; ++ix) {
			sub[ix] += features[ix - numFeatures];
		}
	}
	if (VERB > 2) {
		cerr.precision(2);
		cerr << "Normalization factors" << endl << "Avg ";
	}
	for (ix = 0; ix < numFeatures + numRetentionFeatures; ++ix) {
		if (n > 0.0) {
			sub[ix] /= n; // 现在 sub 是每列 feature 值的均值
		}
		if (VERB > 2) {
			cerr << "\t" << sub[ix];
		}
	}
	for (it = featuresV.begin(); it != featuresV.end(); ++it) {
		features = *it;
		for (ix = 0; ix < numFeatures; ++ix) {
			if (!isfinite(features[ix])) {
				cerr << "Reached strange feature with val=" << features[ix]
					<< " at col=" << ix << endl;
			}
			double d = features[ix] - sub[ix];
			div[ix] += d * d; // div 现在是方差和
		}
	}
	for (it = rtFeaturesV.begin(); it != rtFeaturesV.end(); ++it) {
		features = *it;
		for (ix = numFeatures; ix < numFeatures + numRetentionFeatures; ++ix) {
			if (!isfinite(features[ix - numFeatures])) {
				cerr << "Reached strange feature with val=" << features[ix
					- numFeatures] << " at col=" << ix << endl;
			}
			double d = features[ix - numFeatures] - sub[ix];
			div[ix] += d * d;
		}
	}
	if (VERB > 2) {
		cerr << endl << "Stdv";
	}
	for (ix = 0; ix < numFeatures + numRetentionFeatures; ++ix) {
		if (div[ix] <= 0 || n == 0) {
			div[ix] = 1.0;
		}
		else {
			div[ix] = sqrt(div[ix] / n); // 现在是标准差
		}
		if (VERB > 2) {
			cerr << "\t" << div[ix];
		}
	}
	if (VERB > 2) {
		cerr << endl;
	}
}


void StdvNormalizer::updateSet(vector<double*>& featuresV, size_t offset,
	size_t numFeatures) {
	double n = 0.0;
	double* features;
	size_t ix;
	vector<double*>::iterator it = featuresV.begin();
	for (; it != featuresV.end(); ++it) {
		features = *it;
		n++;
		for (ix = 0; ix < numFeatures; ++ix) {
			sub[offset + ix] += features[ix];
		}
	}
	if (VERB > 2) {
		cerr.precision(2);
		cerr << "Normalization factors" << endl << "Avg ";
	}
	for (ix = 0; ix < numFeatures; ++ix) {
		if (n > 0.0) {
			sub[offset + ix] /= n;
		}
		if (VERB > 2) {
			cerr << "\t" << sub[offset + ix];
		}
	}
	for (it = featuresV.begin(); it != featuresV.end(); ++it) {
		features = *it;
		for (ix = 0; ix < numFeatures; ++ix) {
			if (!isfinite(features[ix])) {
				cerr << "Reached strange feature with val=" << features[ix]
					<< " at col=" << ix << endl;
			}
			double d = features[ix] - sub[offset + ix];
			div[offset + ix] += d * d;
		}
	}

	if (VERB > 2) {
		cerr << endl << "Stdv";
	}
	for (ix = 0; ix < numFeatures; ++ix) {
		if (div[offset + ix] <= 0 || n == 0) {
			div[offset + ix] = 1.0;
		}
		else {
			div[offset + ix] = sqrt(div[offset + ix] / n);
		}
		if (VERB > 2) {
			cerr << "\t" << div[offset + ix];
		}
	}
	if (VERB > 2) {
		cerr << endl;
	}
}
