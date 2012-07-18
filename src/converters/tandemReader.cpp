/* Some Notes about the code
 * 
 * The defaultNameSpace bool
 * It is used to check if the tandem xml file has a default namespace declared named http://www.thegpm.org/TANDEM/2011.12.01.1
 * The bool is set to true if it has. Normal tandem files doesn't have it declared this namespace declared and the code wont work properly if it has
 * beacuse of that the xsd file doesn't have the targetnamespace declared:
 * targetNamespace="http://www.thegpm.org/TANDEM/2011.12.01.1"
 * 
 */

#include "tandemReader.h"

static const XMLCh groupStr[] = { chLatin_g, chLatin_r, chLatin_o, chLatin_u, chLatin_p, chNull};
static const XMLCh groupTypeStr[] = { chLatin_t, chLatin_y, chLatin_p, chLatin_e, chNull};
static const XMLCh groupModelStr[] = { chLatin_m, chLatin_o, chLatin_d, chLatin_e, chLatin_l, chNull};
string schemaDefinition = TANDEM_SCHEMA_LOCATION + string("tandem2011.12.01.1.xsd");
string scheme_namespace; //Will be set in checkValidity based on the bool defaultNameSpace
string schema_major = "";
string schema_minor = "";

tandemReader::tandemReader(ParseOptions po):Reader(po)
{
  x_score=false;
  y_score=false;
  z_score=false;
  a_score=false;
  b_score=false;
  c_score=false;
  firstPSM=true;
}

tandemReader::~tandemReader()
{

}

//A function to split strings
std::vector<std::string> &tandemReader::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
      if(item.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_#@*?")!=std::string::npos){
	elems.push_back(item); 
      }
    }
    return elems;
}

std::vector<std::string>tandemReader::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

//NOTE Function to add namespace to a dom documents all elements except elements that has the namespace www.bioml.com/gaml/
/*
void add_namespace (xercesc_3_1::DOMDocument* doc,xercesc::DOMElement* e,const XMLCh* ns)
{
  DOMElement* ne;
  XMLCh* GAML_XMLCH=XMLString::transcode("www.bioml.com/gaml/");
  
  if(XMLString::equals(e->getNamespaceURI(),GAML_XMLCH))//Check if gaml namespace
  {
    ne=e;
  }else
  {
    ne =static_cast<DOMElement*> (doc->renameNode (e, ns, e->getLocalName ()));
  }
  DOMNodeList* childList=ne->getChildNodes();
 
  for(int iter=0; iter<childList->getLength();iter++)
  {
    //std::cerr << "Lvl: " << lvl << " For iter" << std::endl;
    if (childList->item(iter)->getNodeType () == DOMNode::ELEMENT_NODE)
    {
      add_namespace (doc, static_cast<DOMElement*> (childList->item(iter)), ns);
    }
  }
  XMLString::release(&GAML_XMLCH); 
}

void add_namespace (xercesc_3_1::DOMDocument* doc,xercesc::DOMElement* e,const std::string& ns)
{
  add_namespace (doc, e, xsd::cxx::xml::string (ns).c_str ());
}
*/

bool tandemReader::checkValidity(const std::string file)
{
  bool isvalid;
  std::ifstream fileIn(file.c_str(), std::ios::in);
  if (!fileIn) 
  {
    std::cerr << "Could not open file " << file << std::endl;
    exit(-1);
  }
  std::string line,line_xml,line_bioml;
  if (!getline(fileIn, line)) 
  {
    std::cerr << "Could not read file " << file << std::endl;
    exit(-1);
  }
  
  if (line.find("<?xml") != std::string::npos)
  {
    getline(fileIn, line_xml);
    getline(fileIn, line_bioml);
    
    if (line_xml.find("tandem") == std::string::npos||line_bioml.find("<bioml") == std::string::npos)
    {
      std::cerr << "XML file not generated by X!tandem: " << file << std::endl;
      exit(-1);
    }else
    {
      isvalid=true;
    }
    if(line_bioml.find("xmlns=\"http://www.thegpm.org/TANDEM/2011.12.01.1\"")!=std::string::npos){
      defaultNameSpace=true;
      scheme_namespace=TANDEM_NAMESPACE;
    }else
    {
      defaultNameSpace=false;
      scheme_namespace="";
    }
    defaultNameSpaceVect.push_back(defaultNameSpace); //Needed to check that all have or dont have default namespace
  } 
  else
  {
    isvalid = false;
  }

  fileIn.close();
  return isvalid;
}

bool tandemReader::checkIsMeta(std::string file)
{
  bool ismeta;
  std::string line;
  std::ifstream fileIn(file.c_str(), std::ios::in);
  getline(fileIn, line);
  
  if (line.find("<?xml") != std::string::npos)
  {
    ismeta= false;
  } 
  else
  {
    ismeta = true;
  }

  fileIn.close();
  return ismeta;
}


void  tandemReader::addFeatureDescriptions(bool doEnzyme,const std::string& aaAlphabet) //TODO Needs some changes
{
  push_backFeatureDescription("hyperscore");
  push_backFeatureDescription("nextscore");
  push_backFeatureDescription("ProteinExpectedValue");
  push_backFeatureDescription("DomainExpectedValue");
  
  push_backFeatureDescription("ProteinSumIon");
  
  if(b_score && y_score)
  {
    push_backFeatureDescription("IonRatio");
  }
  
  push_backFeatureDescription("Hyperscore constant a0");
  push_backFeatureDescription("Hyperscore constant a1");

  push_backFeatureDescription("Mass");
  //Mass difference
  push_backFeatureDescription("dM");
  push_backFeatureDescription("absdM");
  
  push_backFeatureDescription("Missed cleaveges");
  
  push_backFeatureDescription("PepLen");
  
  for (int charge = minCharge; charge <= maxCharge; ++charge) 
  {
    std::ostringstream cname;
    cname << "Charge" << charge;
    push_backFeatureDescription(cname.str().c_str());

  }
  if (doEnzyme) 
  {
    push_backFeatureDescription("enzN");
    push_backFeatureDescription("enzC");
    push_backFeatureDescription("enzInt");
  }
  
  
  if (po.calcPTMs) 
  {
    push_backFeatureDescription("ptm");
  }
  
  if (po.pngasef) 
  {
    push_backFeatureDescription("PNGaseF");
  }
  
  if (po.calcAAFrequencies)
  {
    for (std::string::const_iterator it = aaAlphabet.begin(); it != aaAlphabet.end(); it++)
    {
      std::string temp = boost::lexical_cast<std::string>(*it)+"-Freq";
      push_backFeatureDescription(temp.c_str());
    }
  }
}

void tandemReader::getMaxMinCharge(const std::string fn, bool isDecoy){
  
  int nTot = 0, charge = 0;
  
  namespace xml = xsd::cxx::xml;
 
  ifstream ifs;
  ifs.exceptions(ifstream::badbit|ifstream::failbit);
    
  ifs.open(fn.c_str());
  if (!ifs)
  {
    std::cerr << "Could not open file " << fn << std::endl;
    exit(-1);
  }
  parser p;
  
  try
  {
    //Sending defaultNameSpace as the bool for validation since if its not fixed the namespace has to be added later and then we cant validate the schema and xml file.
    xml_schema::dom::auto_ptr< xercesc::DOMDocument> doc (p.start (ifs, fn.c_str(),true, schemaDefinition,schema_major, schema_minor, scheme_namespace,!defaultNameSpace));
    assert(doc.get());
    
    for (doc = p.next(); doc.get() != 0; doc = p.next ())
    {  
      //Check that the tag name is group and that its not the inputput parameters
      if(XMLString::equals(groupStr,doc->getDocumentElement()->getTagName()) && XMLString::equals(groupModelStr,doc->getDocumentElement()->getAttribute(groupTypeStr)))  
      {
	tandem_ns::group groupObj(*doc->getDocumentElement()); //Parse to the codesynthesis object model
	  
	if(groupObj.z().present()) //We are sure we are not in parameters group so z(the charge) has to be present.
	{
	  stringstream chargeStream (stringstream::in | stringstream::out);
	  chargeStream << groupObj.z();
	  chargeStream >> charge;
	  if (minCharge > charge) minCharge = charge;
	  if (maxCharge < charge) maxCharge = charge;
	  nTot++;
	}
	else
	{
	  cerr << "Missing charge(attribute z in group element) for one or more groups in: " << fn << endl;
	  exit(1);
	}
	if(firstPSM)
	{
	  BOOST_FOREACH(const tandem_ns::protein &protObj, groupObj.protein()) //Protein
	  {
	    //Check what type of scores/ions are present
	    tandem_ns::protein::peptide_type peptideObj=protObj.peptide();
	    tandem_ns::peptide::domain_type domainObj=peptideObj.domain();
	    
	    //x,y,z
	    if(domainObj.x_score().present())
	    {
	      x_score=true;
	    }
	    if(domainObj.x_ions().present())
	    {
	      x_score=true;
	    }
	    if(domainObj.y_score().present())
	    {
	      y_score=true;
	    }
	    if(domainObj.y_ions().present())
	    {
	      y_score=true;
	    }
	    if(domainObj.z_score().present())
	    {
	      z_score=true;
	    }
	    if(domainObj.z_ions().present())
	    {
	      z_score=true;
	    }
	    
	    //a,b,c
	    if(domainObj.a_score().present())
	    {
	      a_score=true;
	    }
	    if(domainObj.a_ions().present())
	    {
	      a_score=true;
	    }
	    if(domainObj.b_score().present())
	    {
	      b_score=true;
	    }
	    if(domainObj.b_ions().present())
	    {
	      b_score=true;
	    }
	    if(domainObj.c_score().present())
	    {
	      c_score=true;
	    }
	    if(domainObj.c_ions().present())
	    {
	      c_score=true;
	    }
	  }//End of boost
	  firstPSM=false;
	}
      }
    }
  }catch (const xml_schema::exception& e)
  {
    cerr << "Problem reading the xml file: " << fn << endl;
    cerr << e << endl;
    exit(1);
  }
  
  if(nTot<=0)
  {
    std::cerr << "The file " << fn << " does not contain any records" << std::endl;
    exit(1);
  }
  
  ifs.close();
  return;
}


void tandemReader::createPSM(const tandem_ns::protein &protObj,bool isDecoy,boost::shared_ptr<FragSpectrumScanDatabase> database,spectraMapType &spectraMap,std::string fn){

  typedef map<std::string,double> protMapType;
  typedef map<std::string,std::string> protMapStringType;
  
  std::ostringstream id;
  std::string fileId, protId, proteinName;
  int rank, spectraId;
  std::vector< std::string > proteinNames;
  protMapType protMap;
  protMapStringType protMapString;
  
  std::auto_ptr< percolatorInNs::features >  features_p( new percolatorInNs::features ());
  percolatorInNs::features::feature_sequence & f_seq =  features_p->feature();
  std::map<char,int> ptmMap = po.ptmScheme;
  
  tandem_ns::protein::peptide_type peptideObj=protObj.peptide();
  tandem_ns::peptide::domain_type domainObj=peptideObj.domain();
	      
  fileId = fn;
  size_t spos = fileId.rfind('/');
  if (spos != std::string::npos) fileId.erase(0, spos + 1);
  spos = fileId.find('.');
  if (spos != std::string::npos) fileId.erase(spos);
  
  
  //Information about the protein that the spectra matched
  protMap["protExpect"]=protObj.expect();	//the log10 value of the expectation value for the protein
  protMap["protSumI"]=protObj.sumI();	//the sum of all of the fragment ions that identify this protein
  protId=protObj.id();	//the identifier for this particular identification (spectrum #).(id #)
  int pos=protId.find('.');
  
  if (pos!=string::npos)
  {
    spectraId=boost::lexical_cast<int>(protId.substr(0,pos));
    rank=boost::lexical_cast<int>(protId.substr(pos+1));
  }
	    
  //Describes the region of the protein’s sequence that was identified.
  protMap["domainExpect"]=domainObj.expect();		//the expectation value for the peptide identification
  protMap["calculatedMass"]=domainObj.mh();		//the calculated peptide mass + a proton
  protMap["massDiff"]=domainObj.delta();		//the spectrum mh minus the calculated mh
  protMap["hyperScore"]=domainObj.hyperscore();	//Tandem’s score for the identification
  protMap["nextScore"]=domainObj.nextscore();
  
  //x score and ions
  if(domainObj.x_score().present() && domainObj.x_ions().present())
  {
    if(!x_score)
    {
      std::cerr << "x score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
    protMap["xScore"]=domainObj.x_score().get();
    protMap["xIons"]=domainObj.x_ions().get();
  }
  else
  {
    if(x_score)
    {
      std::cerr << "x score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
  }
  
  //y score and ions
  if(domainObj.y_score().present() && domainObj.y_ions().present())
  {
    if(!y_score)
    {
      std::cerr << "y score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
    protMap["yScore"]=domainObj.y_score().get();
    protMap["yIons"]=domainObj.y_ions().get();
  }
  else
  {
    if(y_score)
    {
      std::cerr << "y score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
  }
  
  //z score and ions
  if(domainObj.z_score().present() && domainObj.z_ions().present())
  {
    if(!z_score)
    {
      std::cerr << "z score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
    protMap["zScore"]=domainObj.z_score().get();
    protMap["zIons"]=domainObj.z_ions().get();
  }
  else
  {
    if(z_score)
    {
      std::cerr << "z score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
  }
  
  //a score and ions
  if(domainObj.a_score().present() && domainObj.a_ions().present())
  {
    if(!a_score)
    {
      std::cerr << "a score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
    protMap["aScore"]=domainObj.a_score().get();
    protMap["aIons"]=domainObj.a_ions().get();
  }
  else
  {
    if(a_score)
    {
      std::cerr << "a score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
  }
  
  //b score and ions
  if(domainObj.b_score().present() && domainObj.b_ions().present())
  {
    if(!b_score)
    {
      std::cerr << "b score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
    protMap["bScore"]=domainObj.b_score().get();
    protMap["bIons"]=domainObj.b_ions().get();
  }
  else
  {
    if(b_score)
    {
      std::cerr << "b score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
  }
  
  //c score and ions
  if(domainObj.c_score().present() && domainObj.c_ions().present())
  {
    if(!c_score)
    {
      std::cerr << "c score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
    protMap["cScore"]=domainObj.c_score().get();
    protMap["cIons"]=domainObj.c_ions().get();
  }
  else
  {
    if(c_score)
    {
      std::cerr << "c score/ions is only reported in some domain elements, it should be reported in all or none." << endl;
      exit(-1);
    } 
  }
  
  protMapString["domainPre"]=domainObj.pre();		//the four residues preceding the domain
  protMapString["domainPost"]=domainObj.post();	//the four residues following the domain
  protMapString["peptide"]=domainObj.seq();		//the sequence of the domain
  protMap["missedCleavages"]=domainObj.missed_cleavages();//the number of potential cleavage sites in this peptide sequence.
  
  //Get absMassDiff
  if(protMap["massDiff"]>0)
  {
    protMap["absMassDiff"]=protMap["massDiff"];
  }
  else
  {
    protMap["absMassDiff"]=-protMap["massDiff"];
  }
  
  //Information about mass modifications in the peptide
  BOOST_FOREACH(const tandem_ns::aa &aaObj, domainObj.aa())
  {
    aaObj.modified();
    aaObj.type();
    aaObj.at();
  }
  
  //Create id
  id.str("");
  id << fileId << '_' << spectraId << '_' << spectraMap["charge"] << '_' << rank;
  std::string psmId=id.str();
  
  //Get rid of unprinatables in proteinID and make list of proteinIDs
  protMapString["proteinName"]=getRidOfUnprintables(protObj.label());
  
  //Adjust isDecoy if combined file
  if(po.iscombined)
  {
    isDecoy = protMapString["proteinName"].find(po.reversedFeaturePattern, 0) != std::string::npos;
  }

  //Make new strings without flank and make strings with the flanks FIXME Not sure if this is the correct flanks
  protMapString["peptideNoFlank"]=protMapString["peptide"].substr(0,protMapString["peptide"].length() - 1); //TODO check this might be -2 or something
  protMapString["flankN"]=protMapString["domainPre"].at(protMapString["domainPre"].size()-1); //Last of pre
  protMapString["flankC"]=protMapString["peptide"].at(protMapString["peptide"].length()-1); //Last of peptide
  protMapString["peptideWithFlank"]=protMapString["flankN"]+"."+protMapString["peptideNoFlank"]+"."+protMapString["flankC"];
  
  std::string peptideS=protMapString["peptideNoFlank"]; //TMP string used to get all modifications to the psm object
  std::string peptideNoFlankNoMod=protMapString["peptideNoFlank"];
  
  //Check length of peptide, if its to short it cant contain both flanks and peptide
  if(protMapString["peptideNoFlank"].size()<po.peptidelength )
  {
    std::cerr << "The peptide: " << protMapString["peptideWithFlank"] << " is shorter than the specified minium length. File: " << fn << std::endl;
    exit(-1);
  }
  
  
  //Remove modifications
  for(unsigned int ix=0;ix<peptideNoFlankNoMod.size();++ix)
  {
    if (aaAlphabet.find(peptideNoFlankNoMod[ix])==string::npos && ambiguousAA.find(peptideNoFlankNoMod[ix])==string::npos && modifiedAA.find(peptideNoFlankNoMod[ix])==string::npos)
    {
      if (ptmMap.count(peptideNoFlankNoMod[ix])==0) 
      {
	cerr << "Peptide sequence " << protMapString["peptideWithFlank"] << " contains modification " << peptideNoFlankNoMod[ix] << " that is not specified by a \"-p\" argument" << endl;
	exit(-1);
      }
      peptideNoFlankNoMod.erase(ix,1);
    }  
  }
  std::auto_ptr< percolatorInNs::peptideType >  peptide_p( new percolatorInNs::peptideType( peptideNoFlankNoMod   ) );
  
  //Register the ptms
  for(unsigned int ix=0;ix<peptideS.size();++ix) 
  {
    if (aaAlphabet.find(peptideS[ix])==string::npos) 
    {
      int accession = ptmMap[peptideS[ix]];
      std::auto_ptr< percolatorInNs::uniMod > um_p (new percolatorInNs::uniMod(accession));
      std::auto_ptr< percolatorInNs::modificationType >  mod_p( new percolatorInNs::modificationType(um_p,ix));
      peptide_p->modification().push_back(mod_p);      
      peptideS.erase(ix,1);      
    }  
  }
  
  //Push back the main scores
  f_seq.push_back(protMap["hyperScore"]);
  f_seq.push_back(protMap["nextScore"]);

  //Expect
  f_seq.push_back(protMap["protExpect"]);
  f_seq.push_back(protMap["domainExpect"]);
  
  //Ion related features
  f_seq.push_back(protMap["protSumI"]);
  
  //Calculate Ion ratio NOTE Not complety sure this is right and it would be nice to use the other variables to.
  if(b_score && y_score)
  {
    f_seq.push_back(float(protMap["bIons"]+protMap["yIons"])/float((protMapString["peptideWithFlank"].length()-2)*2));//-2 is cause of the . in the sequence
  }
  
  //Hyperscore constants
  f_seq.push_back(spectraMap["a0"]);
  f_seq.push_back(spectraMap["a1"]);
  
  //Pusback massrelated
  f_seq.push_back(spectraMap["parenIonMass"]);
  f_seq.push_back(protMap["massDiff"]);
  f_seq.push_back(protMap["absMassDiff"]);
  
  //Missed cleavages
  f_seq.push_back(protMap["missedCleavages"]);
  
  //Length of peptide
  f_seq.push_back(peptideLength(protMapString["peptideWithFlank"]));
  
  //Charge
  for (int c = minCharge; c <= maxCharge; c++) 
  {
    f_seq.push_back( spectraMap["charge"] == c ? 1.0 : 0.0);
  }
  
  //Enzyme
  if (Enzyme::getEnzymeType() != Enzyme::NO_ENZYME) 
  {
    f_seq.push_back( Enzyme::isEnzymatic(protMapString["peptideWithFlank"].at(0),protMapString["peptideWithFlank"].at(2)) ? 1.0 : 0.0);
    f_seq.push_back(Enzyme::isEnzymatic(protMapString["peptideWithFlank"].at(protMapString["peptideWithFlank"].size() - 3),protMapString["peptideWithFlank"].at(protMapString["peptideWithFlank"].size() - 1)) ? 1.0 : 0.0);
    f_seq.push_back( (double)Enzyme::countEnzymatic(protMapString["peptideNoFlank"]) );
  }
  
  if (po.calcPTMs) 
  {
    f_seq.push_back(cntPTMs(protMapString["peptideWithFlank"]));
  }
  if (po.pngasef) 
  {
    f_seq.push_back(isPngasef(protMapString["peptideWithFlank"],isDecoy));
  }
  if (po.calcAAFrequencies) {
    computeAAFrequencies(protMapString["peptideWithFlank"], f_seq);
  }
  
  //TODO x!tandems new stuff
  
  //Save the psm
  percolatorInNs::peptideSpectrumMatch* tmp_psm = new percolatorInNs::peptideSpectrumMatch (
	features_p,  peptide_p,psmId, isDecoy, spectraMap["parenIonMass"], protMap["calculatedMass"], spectraMap["charge"]);
  std::auto_ptr< percolatorInNs::peptideSpectrumMatch >  psm_p(tmp_psm);
  std::auto_ptr< percolatorInNs::occurence >  oc_p( new percolatorInNs::occurence ("1",protMapString["flankN"], protMapString["flankC"]));
  psm_p->occurence().push_back(oc_p);
  
  database->savePsm(spectraId, psm_p);
    
  protMap.clear();
}

void tandemReader::read(const std::string fn, bool isDecoy,boost::shared_ptr<FragSpectrumScanDatabase> database)
{
  std::string line, tmp, prot;
  std::istringstream lineParse;
  std::ifstream tandemIn;
  int spectraId;
  
  namespace xml = xsd::cxx::xml;
  
  ifstream ifs;
  ifs.exceptions(ifstream::badbit|ifstream::failbit);
  ifs.open(fn.c_str());
  parser p;
  
  //Check that all files have defaultNameSpace declared or not
  for(int i=0; i<defaultNameSpaceVect.size(); i++)
  {
    if(defaultNameSpaceVect.at(i)!=defaultNameSpace)
    {
      cerr << "Some files have a default namespace others have not, check that the X!Tandem version used to make the files is the same." << std::endl;
      exit(1);
    }   
  }
  
    //Sending defaultNameSpace as the bool for validation since if its not fixed the namespace has to be added later and then we cant validate the schema and xml file.
    xml_schema::dom::auto_ptr< xercesc::DOMDocument> doc (p.start (ifs, fn.c_str(),true, schemaDefinition,schema_major, schema_minor, scheme_namespace,!defaultNameSpace));
    assert(doc.get());
    
    //std::cerr << "Doc: " << XMLString::transcode(doc->getDocumentElement()->getTagName()) << std::endl;
    
    //tandem_ns::bioml biomlObj=biomlObj(*doc->getDocumentElement()); NOTE the root of the element, doesn't have any useful attributes
    
    for (doc = p.next(); doc.get() != 0; doc = p.next ()) //Loops over the grouo elements which are the spectra and the last 3 are the input parameters
    {
      //NOTE cant acess mixed content using codesynthesis, need to keep dom assoication. See the manual for tree parser and : 
      //http://www.codesynthesis.com/pipermail/xsd-users/2008-October/002005.html
      //Not implementet here
	
	//Check that the tag name is group and that its not the inputput parameters
	if(XMLString::equals(groupStr,doc->getDocumentElement()->getTagName()) && XMLString::equals(groupModelStr,doc->getDocumentElement()->getAttribute(groupTypeStr))) 
	{
	  int nHits=0;
	  bool a0Found=false;
	  bool a1Found=false;
	  tandem_ns::group groupObj(*doc->getDocumentElement()); //Parse it the codesynthesis object model.
	  
	  if(!spectraMap.empty())
	  {
	    spectraMap.clear();
	  }
	  
	  //Information about the spectra and the highest scoring match. 
	  //NOTE The attributes related to the highest scoring match is not parsed from the group element, all matches are parsed in createPSM function from the protein element and its children
	  if(groupObj.mh().present() && groupObj.z().present() && groupObj.sumI().present() && groupObj.maxI().present() && groupObj.fI().present()&& groupObj.id().present())
	  {
	    spectraMap["parenIonMass"]=groupObj.mh().get(); 	//the parent ion mass (plus a proton) from the spectrum
	    spectraMap["charge"]=groupObj.z().get(); 	//the parent ion charge from the spectrum
	    spectraMap["sumI"]=groupObj.sumI().get();	//the log10 value of the sum of all of the fragment ion intensities
	    spectraMap["maxI"]=groupObj.maxI().get();	//the maximum fragment ion intensity
	    spectraMap["fI"]=groupObj.fI().get();	//a multiplier to convert the normalized spectrum contained in this group back to the original intensity values NOTE Currently not used
	  }
	  else
	  {
	    cerr << "An required attribute is not present in the group/spectra element in file: " << fn << endl;
	    exit(1);
	  }
	  
	  BOOST_FOREACH(const tandem_ns::group1 &groupGAMLObj, groupObj.group1()) //Getting the group element surrounding the GAML namespace
	  {
	  
	    BOOST_FOREACH(const gaml_tandem_ns::trace &traceGAMLObj, groupGAMLObj.trace()) //GAML:trace
	    {
	      
	      BOOST_FOREACH(const gaml_tandem_ns::attribute &attributeTraceGAMLObj, traceGAMLObj.attribute()) //GAML:attribute
	      {
		gaml_tandem_ns::attribute::type_type typeAttr=attributeTraceGAMLObj.type();
		//a0 and a1 are two constans used to calculate the hyperscore expectation function. See:
		//Fenyö D, Beavis RC
		//A method for assessing the statistical significance of mass spectrometry-based protein identifications using general scoring schemes
		if(typeAttr=="a0")
		{
		  if(a0Found)
		  {
		    cerr << "Found more than one a0 attribute in a groups GAML part. File: " << fn << endl;
		    exit(1);
		  }
		  a0Found=true;
		  spectraMap["a0"]=attributeTraceGAMLObj;
		  
		} else if(typeAttr=="a1")
		{
		  if(a1Found)
		  {
		    cerr << "Found more than one a1 attribute in a groups GAML part. File: " << fn << endl;
		    exit(1);
		  }
		  a1Found=true;
		  spectraMap["a1"]=attributeTraceGAMLObj;
		}
	      }
	    }
	  }//End of GAML part
	  
	  //PSMS here
	  BOOST_FOREACH(const tandem_ns::protein &protObj, groupObj.protein()) //Protein
	  {
	    if(nHits<po.hitsPerSpectrum)
	    {
	      createPSM(protObj,isDecoy,database,spectraMap,fn);
	      nHits++;
	    }
	  }
	  spectraMap.clear();
	}//End of if group and not parameters
    }//End of for p.next
    ifs.close();
}

