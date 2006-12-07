#include<iostream>
#include<fstream>
#include <map>
#include <set>
#include <vector>
#include <string>
using namespace std;
#include "DataSet.h"
#include "Normalizer.h"
#include "SetHandler.h"
#include "Scores.h"
#include "Globals.h"
#include "IntraSetRelation.h"

SetHandler::SetHandler() {
//	charge=c;
//	norm=Normalizer::getNew();
    n_examples=0;
    labels = NULL;
    c_vec = NULL;
    intra=NULL;
}

SetHandler::~SetHandler()
{
//	if (norm)
//      delete norm;
//	norm=NULL;
    if (labels)
      delete [] labels;
    labels=NULL;
    if (c_vec)
      delete [] c_vec;
    c_vec=NULL;
    if (intra) 
       delete intra;
    intra=NULL;
    for(unsigned int ix=0;ix<subsets.size();ix++) {
      if (subsets[ix]!=NULL) 
        delete subsets[ix];
      subsets[ix]=NULL;
    } 
}
    

void SetHandler::readFile(const string & fn, const int label) {
  intra = new IntraSetRelation();
  readFile(fn,label,subsets,intra);
  setSet();
}

void SetHandler::readFile(const string& fn, const string& wc, const bool match) {
  intra = new IntraSetRelation();
  readFile(fn,match?-1:1,subsets,intra,wc,match);
  setSet();
}

void SetHandler::readFile(const string fn, const int label, vector<DataSet *> & sets, IntraSetRelation *intra, const string & wild, const bool match, bool calc) {
  ifstream fileIn(fn.c_str(),ios::in);
  if (!fileIn) {
    cerr << "Could not open file " << fn << endl;
    exit(-1);
  }
  string line;
  if (!getline(fileIn,line)) {
    cerr << "Could not read file " << fn << endl;
    exit(-1);
  }
  fileIn.close();
  if (line.size()>1 && line[0] == 'H' && line[1]=='\t') {
    if (line.find("SQTGenerator")==string::npos) {
      cerr << "SQT file not generated by SEQUEST: " << fn << endl;
      exit(-1);  
    }
    DataSet * pSet = new DataSet();
    pSet->setLabel(label);
    pSet->read_sqt(fn,intra,wild,match);
    sets.push_back(pSet);
  } else {
    // we hopefully found a meta file
    string line2;
    ifstream meta(fn.data(),ios::in);
    while(getline(meta,line2)) {
      if (line2.size()>0 && line2[0] != '#')
//        cout << "0:" << line2[0] << " 1:" << line2[1] << " e:" << line2[line2.size()-1] << endl;
        readFile(line2,label,sets,intra,wild,match,false);
    }
    meta.close();
  }
  if (calc) {
    vector<DataSet *>::iterator it;
    for (it=sets.begin();it!=sets.end();it++) {
      ((DataSet *)(*it))->computeIntraSetFeatures();
    }
  }  
}

void SetHandler::modifyFile(const string& fn, double *w, Scores& sc , const string& greet) {
  if (!fn.empty()) {
    modifyFile(fn,subsets,w,sc,greet);
  }
}

void SetHandler::modifyFile(const string& fn, vector<DataSet *> & sets, double *w, Scores& sc , const string& greet) {
  string line;
  ifstream fileIn(fn.c_str(),ios::in);
  if (sets.size()>1 && (!fileIn)) {
    cerr << "More than one input file, and file " << fn << 
            " that should contain listing to a set of files is not readable" << endl;
    exit(-1);
  }
  if (sets.size()>1 && getline(fileIn,line) && line.size()>1 && 
        line[0] == 'H' && line[1]=='\t') {
    cerr << "More than one input file, and file " << fn << 
            " do not contain listing to a set of files" << endl;
    exit(-1);
  }
  if (!!fileIn)
    fileIn.close();
  
  if (sets.size()==1 ) {
    ((DataSet *)sets[0])->modify_sqt(fn,w,&sc,greet);
    return;
  }
  unsigned int ix=0;
  fileIn.open(fn.c_str(),ios::in);
  while(getline(fileIn,line)) {
    if(line.size()>0 && line[0]!='#') {
      ((DataSet *)sets[ix++])->modify_sqt(line,w,&sc,greet);
    }    
  }
  fileIn.close();
}

void SetHandler::print(Scores &test) {
  vector<pair<double,string> > outList(0);
  for (unsigned int setPos=0;setPos<subsets.size();setPos++) {
    subsets[setPos]->print(test,outList);
  }    
  sort(outList.begin(),outList.end());
  reverse(outList.begin(),outList.end());  
  vector<pair<double,string> >::const_iterator it = outList.begin();
  for(;it!=outList.end();it++) {
    cout << it->second << endl;
  }
}


void SetHandler::generateTrainingSet(const double fdr,const double cpos, const double cneg,const Scores & sc) {
  double tp=0,fp=0;
  unsigned int ix=0;
  examples.clear();
  bool underCutOff = true;
  vector<ScoreHolder>::const_iterator it;
  for(it=sc.begin();it!=sc.end();it++) {
    if (it->label==-1) fp++; else tp++;
    if (underCutOff && fdr<(fp/(tp+fp)))
      underCutOff=false;
    if (it->label==-1 || underCutOff) {
      examples.push_back(it->featVec);
      labels[ix]=it->label;
      c_vec[ix++]=(it->label!=-1?cpos:cneg);
    }
  }
}


const double * SetHandler::getNext(int& setPos,int& ixPos) const {
  double * features = subsets[setPos]->getNext(ixPos);
  if (features) return features;
  if (++setPos>=((signed int)subsets.size()))
    return NULL;
  ixPos=-1;
  return subsets[setPos]->getNext(ixPos);
}

const double * SetHandler::getFeatures(const int setPos,const int ixPos) const {
  return subsets[setPos]->getFeatures(ixPos);
}

int const SetHandler::getLabel(int setPos) {
  assert(setPos>=0 && setPos<(signed int)subsets.size());
  return subsets[setPos]->getLabel();
}

void SetHandler::setSet(){
  n_examples=0;
  int i=0,j=-1;
  while(getNext(i,j)) {
    n_examples++;
  }
  if(!labels) labels= new double[n_examples];
  if(!c_vec) c_vec = new double[n_examples];
  if (VERB>3) {
    cerr << "Set up a SetHandler with " << subsets.size() << " DataSet:s and " << n_examples << " examples" << endl;
    if (VERB>4) {
      for (unsigned int i=0;i<subsets.size();i++) {
        cerr << "First 10 lines of " << i+1 << " set with " << subsets[i]->getLabel() << " label" << endl;
        subsets[i]->print_10features();
      }
    }
  }
}



void SetHandler::readGist(const string & dataFN, const string & labelFN, const int setLabel) {
  intra = NULL;
  if (VERB>1) cerr << "Reading gist input from datafile " << dataFN << " and labels from " << labelFN << endl; 
  ifstream labelStream(labelFN.c_str(),ios::out);
  if (!labelStream) {
    cerr << "Can not open file " << labelFN << endl;
    exit(-1);
  }
  vector<unsigned int> ixs;
  ixs.clear();
  string tmp,line;
  int label;
  unsigned int ix=0;
  getline(labelStream,tmp); // Id row
  while(true) {
    labelStream >> tmp >> label;
    if (!labelStream) break;
    if (label==setLabel) {ixs.push_back(ix);}
    ++ix;
  }
  labelStream.close();
  ifstream dataStream(dataFN.c_str(),ios::out);
  if (!dataStream) {
    cerr << "Can not open file " << dataFN << endl;
    exit(-1);
  }
  dataStream >> tmp;
  dataStream.get();        // removed enumrator and tab 
  getline(dataStream,line);
  DataSet::setFeatureNames(line);
  DataSet * theSet = new DataSet();
  theSet->setLabel(setLabel>0?1:-1);
  theSet->readGistData(dataStream,ixs);
  dataStream.close();
  subsets.push_back(theSet);
  setSet();

}    

void SetHandler::gistWrite(const string & fileNameTrunk,const SetHandler& norm,const SetHandler& shuff, const SetHandler& shuff2) {
  string dataFN = fileNameTrunk + ".data";
  string labelFN = fileNameTrunk + ".label";
  ofstream dataStream(dataFN.data(),ios::out);
  ofstream labelStream(labelFN.data(),ios::out);
  labelStream << "SpecId\tLabel" << endl; 
  dataStream << "SpecId\t" << DataSet::getFeatureNames() << endl;
  string str;
  for (int setPos=0;setPos< (signed int)norm.subsets.size();setPos++) {
    int ixPos=-1;
    while (norm.subsets[setPos]->getGistDataRow(ixPos,str)) {
      dataStream << str;
      labelStream << str.substr(0,str.find('\t')+1) << (norm.subsets[setPos]->getLabel()==-1?-1:+1) << endl; 
    }
  }    
  for (int setPos=0;setPos< (signed int)shuff.subsets.size();setPos++) {
    int ixPos=-1;
    while (shuff.subsets[setPos]->getGistDataRow(ixPos,str)) {
      dataStream << str;
      labelStream << str.substr(0,str.find('\t')+1) << (shuff.subsets[setPos]->getLabel()==-1?-1:+1) << endl; 
    }
  }    
  for (int setPos=0;setPos< (signed int)shuff2.subsets.size();setPos++) {
    int ixPos=-1;
    while (shuff2.subsets[setPos]->getGistDataRow(ixPos,str)) {
      dataStream << str;
      labelStream << str.substr(0,str.find('\t')+1) << (shuff2.subsets[setPos]->getLabel()==-1?-2:+1) << endl; 
    }
  }    
  dataStream.close();
  labelStream.close();
  
}
