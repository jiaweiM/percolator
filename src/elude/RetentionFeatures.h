/*******************************************************************************
 Copyright 2006-2010 Lukas Käll <lukas.kall@cbr.su.se>

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
/*
 * @ Created by Luminita Moruz
 * Sep, 2010
 */
/*
 * The file stores the declaration of the class RetentionFeatures
 * This class includes information about different retention features available
 * and provides methods to calculate the retention features for a set of peptides
 */

#ifndef ELUDE_RETENTIONFEATURES_H_
#define ELUDE_RETENTIONFEATURES_H_

#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <set>

using namespace std;

/* Th features are organized in groups. We will allow to switch off
 * and on only entire groups and not individual features */
enum FeatureGroup {
  SVR_INDEX = 0,
  KYTE_DOO_LITTLE_INDEX = 1,
  POLAR_FEATURES = 2,
  HYDROPHOBIC_FEATURES = 3,
  BULKINESS = 4,
  LENGTH = 5,
  PTM_FEATURES = 6,
  AA_FEATURES = 7
};
/* number of feature groups */
#define NUM_FEATURE_GROUPS 8

/* forward declaration of PSMDescription */
class PSMDescription;

class RetentionFeatures {
 public:
   RetentionFeatures();
   ~RetentionFeatures();

   /* computes the retention features for a set of peptides; return 0 if success */
   int ComputeRetentionFeatures(vector<PSMDescription> &psms);
   /* computes the retention features for one psm */
   int ComputeRetentionFeatures(PSMDescription &psm);
   /* compute the features for a retention index; returns a pointer to the feature table */
   double* ComputePolarFeatures(const string &peptide, double *features);
   /* compute the features related to hydrophobic aa; returns a pointer to the feature table */
   double* ComputeHydrophobicFeatures(const string &peptide, double *features);
   /* compute bulkiness features; returns a pointer to the feature table */
   double* ComputeBulkinessFeatures(const string &peptide, double *features);
   /* compute length features; returns a pointer to the feature table */
   double* ComputeLengthFeatures(const string &peptide, double *features);
   /* compute ptm features; returns a pointer to the feature table */
   double* ComputePtmFeatures(const string &peptide, double *features);
   /* compute aa features; returns a pointer to the feature table */
   double* ComputeAAFeatures(const string &peptide, double *features);

   double* ComputeIndexFeatures(const string &peptide, const map<string, double> &index, double *features);
   /* compute the features related to polar aa; returns a pointer to the feature table */

   /* get the value in the index for aa */
   static double GetIndexValue(const string &aa, const map<string, double> &index);
   /* get the unmodified version of an amino acid */
   static char GetUnmodifiedAA(const string &aa);
   /* get the amino acids in a peptide (including the modified ones) */
   static vector<string> GetAminoAcids(const string &peptide);
   /* compute the sum of hydrophobicities of the amino acids in a peptide */
   static double IndexSum(const string &peptide, const map<string, double> &index);
   /* compute the average hydrophobicity of the aa in the peptide */
   static double IndexAvg(const string &peptide, const map<string, double> &index);
   /* calculate the sum of hydrophobicities of neighbours of R(Argenine) and K (Lysine) */
   static double IndexNearestNeigbourPos(const string &peptide, const map<string, double> &index);
   /* calculate the sum of hydrophobicities of neighbours of D(Aspartic Acid) and E (Glutamic acid) */
   static double IndexNearestNeigbourNeg(const string &peptide, const map<string, double> &index);
   /* calculate the hydrophobicity of the N-terminus */
   static double IndexN(const string &peptide, const map<string, double> &index);
   /* calculate the hydrophobicity of the C-terminus */
   static double IndexC(const string &peptide, const map<string, double> &index);
   /* product between hydrophobicity of n- and c- terminus */
   static double IndexNC(const string &peptide, const map<string, double> &index);
   /* the most hydrophobic window */
   static double IndexMaxPartialSum(const string &peptide, const map<string, double> &index, const int &win);
   /* the least hydrophobic window */
   static double IndexMinPartialSum(const string &peptide, const map<string, double> &index, const int &win);
   /* calculate the average hydrophobicity of an index */
   static double AvgHydrophobicityIndex(const map<string, double> &index);
   /* calculate the most hydrophobic sides for alpha helices */
   static double IndexMaxHydrophobicSideHelix(const string &peptide,  const map<string, double> &index);
   /* calculate the least hydrophobic sides for alpha helices */
   static double IndexMinHydrophobicSideHelix(const string &peptide,  const map<string, double> &index);
   /* calculate the maximum value of the hydrophobic moment */
   static double IndexMaxHydrophobicMoment(const string &peptide, const map<string, double> &index, const double &angle_degrees, const int &win);
   /* calculate the minimum value of the hydrophobic moment */
   static double IndexMinHydrophobicMoment(const string &peptide, const map<string, double> &index, const double &angle_degrees, const int &win);
   /* Compute the length of a peptide */
   static double PeptideLength(const string &peptide);
   /* Calculate the sum of squared differences in hydrophobicities between neighbours */
   static double IndexSumSquaredDiff(const string &peptide, const map<string, double> &index);

   /* calculate the number of a certain type of aa The set gives the list of such amino acids */
   static double NumberTypeAA(const string &peptide, const set<string> &amino_acid_type);
   /* calculate the number of a consecutibe aa of a certain type. The set gives the type of these such amino acids */
   static double NumberConsecTypeAA(const string &peptide, const set<string> &amino_acid_type);
   /* adds a feature giving the number of each of the symbols in the alphabet found in the peptide */
   static double* FillAAFeatures(const string &peptide, double *retention_features);

   /* accessors and mutators */
   static const map<string, double>& k_kyte_doolittle() { return kKyteDoolittle; }
   static const map<string, double>& k_bulkiness() { return kBulkiness; }
   inline bitset<NUM_FEATURE_GROUPS> active_feature_groups() const { return active_feature_groups_; }
   inline void set_active_feature_groups(const bitset<NUM_FEATURE_GROUPS> active_groups) { active_feature_groups_ = active_groups; }
   inline map<string, double> svr_index() const { return svr_index_; }
   inline void set_svr_index(const map<string, double> index) { svr_index_ = index; }
   inline vector<string> amino_acids_alphabet() const { return amino_acids_alphabet_; }
   inline void set_amino_acids_alphabet(const vector<string> alphabet) { amino_acids_alphabet_ = alphabet; }

 private:
   /* name of each feature group */
   static const string kGroupNames[NUM_FEATURE_GROUPS];
   /* number of features in each group */
   static const int kFeatureGroupNums[NUM_FEATURE_GROUPS];
   /* kyte and doolittle retention index */
   static const map<string, double> kKyteDoolittle;
   /* bulkiness as defined by Zimmerman et al */
   static const map<string, double> kBulkiness;
   /* every bit set corresponds to an active group of features */
   bitset<NUM_FEATURE_GROUPS> active_feature_groups_;
   /* SVR trained index */
   map<string, double> svr_index_;
   /* amino acids alphabet (can include post translationally modified peptides) */
   vector<string> amino_acids_alphabet_;
};

#endif /* ELUDE_RETENTIONFEATURES_H_ */
