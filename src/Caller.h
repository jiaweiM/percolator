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

#ifndef CALLER_H_
#define CALLER_H_
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "Globals.h"
#include "MyException.h"
#include "Option.h"
#include "SetHandler.h"
#include "DataSet.h"
#include "Scores.h"
#include "SanityCheck.h"
#include "Normalizer.h"
#include "ProteinProbEstimator.h"
#include "FidoInterface.h"
#include "PickedProteinInterface.h"
#include "XMLInterface.h"
#include "CrossValidation.h"

/*
* Main class that starts and controls the calculations.
*
* In addition to the calculation Caller also handles command line
* initialization, parsing input parameters, handling output of the
* calculation results.
*
*/
class Caller {
public:
	enum SetHandlerType {
		NORMAL = 1, SHUFFLED = -1, SHUFFLED_TEST = 2, SHUFFLED_THRESHOLD = 3
	};

	Caller();
	virtual ~Caller();

	static string greeter();
	string extendedGreeter(time_t& startTime);
	bool parseOptions(int argc, char** argv);
	int run();

protected:
	Normalizer* pNorm_;
	SanityCheck* pCheck_;
	ProteinProbEstimator* protEstimator_;

	// file input parameters
	// read input file in tab format
	bool tabInput_;
	// 从标准输入读取数据
	bool readStdIn_;
	std::string inputFN_;
	// Skip validation of input file against xml schema.
	bool xmlSchemaValidation_;

	// file output parameters
	// pin-tab 格式输出特征到指定文件
	std::string tabOutputFN_;
	// Path to xml-output (pout) file.
	std::string xmlOutputFN_;
	//  Output final weights to given file
	std::string weightOutputFN_;
	// Output tab delimited results of PSMs to a file instead of stdout, -m
	std::string psmResultFN_;
	// Output tab delimited results of peptides to a file instead of stdout (will be ignored if used with -U option), -r
	std::string peptideResultFN_;
	// Output tab delimited results of proteins to a file instead of stdout (Only valid if option -A or -f is active)
	std::string proteinResultFN_;
	// Output tab delimited results for decoy PSMs into a file, -M
	std::string decoyPsmResultFN_;
	// Output tab delimited results for decoy peptides into a file (will be ignored if used with -U option),-B
	std::string decoyPeptideResultFN_;
	// Output tab delimited results for decoy proteins into a file (Only valid if option -A or -f is active)
	std::string decoyProteinResultFN_;
	// Include decoys (PSMs, peptides and/or proteins) in the xml-output. Only available if -X is set.
	bool xmlPrintDecoys_;
	bool xmlPrintExpMass_;

	// report level parameters
	bool reportUniquePeptides_;
	// 用 target-decoy 竞争替代 mix-max计算 q-values 和 PEPs
	// 如果输入 PSM 是正反库分开检索，Percolator 使用 SVM scores 删除低打分的 scan+expMass 组合
	// 如果Percolator 检测到输入来自合并计算，会自动启动该选项。该选项和 mix-max 不兼容，不能同时使用。
	bool targetDecoyCompetition_;
	// 使用 mix-max 方法设置 q-values 和 PEPs。
	// 用在正反库分开检索的情况，为默认设置。
	bool useMixMax_;
	// 指定正反库检索方式：
	// auto: 由 Percolator自动检测，默认值
	// concatenated: 正反库合并同时检索
	// separate: 正反库分开检索
	std::string inputSearchType_;

	// SVM / cross validation parameters
	// 定义 training 数据集的 positive 样本时所用的FDR阈值。如果为0，设置为 testFDR。Default = 0.01.
	double selectionFdr_;
	// 评价最佳CV结果和输出最终结果所用的FDR阈值. Default = 0.01.
	double testFdr_;
	// 第一次迭代所用的FDR阈值，在原始特征分离target和decoy不好使很有用，随后的迭代使用trainFDR
	double initialSelectionFdr_;
	// 最大迭代次数. Default = 10.
	unsigned int numIterations_;
	// Only train an SVM on a subset of <x> PSMs, and use the resulting score vector to evaluate the other PSMs. Recommended when analyzing huge numbers (>1 million) of PSMs. When set to 0, all PSMs are used for training as normal. Default = 0.
	unsigned int maxPSMs_;
	// Number of nested cross validation bins within each cross validation bin. This should reduce overfitting of the hyperparameters. Default = 1.
	unsigned int nestedXvalBins_;
	// positive 错误分类的惩罚值，即C值。如果不指定，CV会计算合适的值
	double selectedCpos_;
	// negative 错误分类的惩罚值。如果不指定，CV会计算合适的值
	double selectedCneg_;
	// 每次迭代都在测试数据集上查看性能,default=false
	bool reportEachIteration_;
	// 通过减少内部交叉验证加快执行速度,default=false
	bool quickValidation_;
	// 对每个谱图，强制只保留打分最高的PSM，如果用户只提供每个谱图一个PSM，该选项无效。
	bool trainBestPositive_;

	// reporting parameters
	std::string call_;

	void calculatePSMProb(Scores& allScores, bool uniquePeptideRun,
		time_t& procStart, clock_t& procStartClock, double& diff);

	void calculateProteinProbabilities(Scores& allScores);

	void checkIsWritable(const std::string& filePath);

#ifdef CRUX
	virtual void processPsmScores(Scores& allScores) {}
	virtual void processPeptideScores(Scores& allScores) {}
	virtual void processProteinScores(ProteinProbEstimator* protEstimator) {}
#endif

};

#endif /*CALLER_H_*/
