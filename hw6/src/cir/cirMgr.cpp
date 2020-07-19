/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{  
   //cout<<"start read\n";
   string s;
   
   
   size_t noneed;
   size_t gateN;
   size_t length;
   size_t n1;
   size_t n2;
   size_t n3;
   
   ifstream inputFile(fileName);
   if(!inputFile)return false;
   inputFile>>s>>maxid>>input_num>>noneed>>output_num>>And_num;
   //cout<<s<<" "<<input_num<<" "<<output_num<<" "<<And_num<<endl;
   maxidTotal=maxid+output_num;
   length=input_num+output_num+And_num+1;

   gateArray=new CirGate* [maxidTotal+1];
   size_t* tmplh =new size_t [maxidTotal+1];
   size_t* tmprh =new size_t [maxidTotal+1];
   //cout<<"read aag\n";
   
   for (size_t s=0;s<maxidTotal+1;s++){
      gateArray[s]=nullptr;
      tmplh[s]=0;
      tmprh[s]=0;
   }
   tmplh[0]=0;
   tmprh[0]=0;
   //cout<<"const\n";
   gateArray[0]=new constGate(0,0);
   //cout<<"input\n";
   
   for (size_t i=1; i<input_num+1 ;i++){
       inputFile>>n1;
       n1=n1/2;
       idArray.push_back(n1);
       gateArray[n1]=new InGate(i+1,n1);
       //cout<<n1<<endl;
      // gateArray[n1]->reportGate();
       tmprh[i]=(size_t)0;
      
   }
   //cout<<"output\n";
   for (size_t i=1; i<output_num+1 ;i++){
       inputFile>>n1;
       outArray.push_back(n1);
       gateArray[maxid+i]=new outGate(i+input_num+1,maxid+i);
       tmplh[maxid+i]=n1;
       //cout<<tmplh[maxid+i]<<endl;
   }
   //cout<<"and\n";
   for (size_t i=input_num+output_num+1; i<input_num+output_num+And_num+1 ;i++){
       inputFile>>gateN;
       gateN=gateN/2;
       inputFile>>tmplh[gateN];
       inputFile>>tmprh[gateN];
       gateArray[gateN]=new AndGate(i+1,gateN);
       //cout<<noneed<<" "<<tmplh[gateN]<<" "<<tmprh[gateN]<<endl;
   }
   //cout<<"undef\n";
   for (size_t i=0;i<maxidTotal;i++){
      if(gateArray[i]==nullptr){
         gateArray[i]=new undefGate(0,i);
        // cout<<i<<endl;
      }
   }
   /*
   for(size_t i=0; i<maxidTotal+1 ;i++){
      gateArray[i]->reportGate();
      //cout<<tmplh[i]<<" "<<tmprh[i]<<endl;
   }*/
   //cout<<"start connect\n";
   connectCircuit(gateArray,tmplh,tmprh,maxidTotal+1);
   //cout<<"start readname\n";
   readname(inputFile);
   delete[] tmplh;
   delete[] tmprh;

   /*
   cout<<" print"<<length<<" \n";
   for(size_t i=0; i<maxidTotal+1 ;i++){
      cout<<i*2<<" "<<tmplh[i]<<" "<<tmprh[i]<<" ";
       //gateArray[i]->printnum();
      gateArray[i]->printGate();
   }
   cout<<" finish"<<endl;
   */
   return true;
}
bool 
CirMgr::connectCircuit(CirGate ** g,size_t* lh,size_t * rh,size_t length){
   bool inv;
   for (size_t s=0;s<length;s++){
      inv=(lh[s]%2==1);
      if(gateArray[s]->connect(gateArray[lh[s]/2],inv)){
         inv=(rh[s]%2==1);
         gateArray[s]->connect(gateArray[rh[s]/2],inv);
      }
   }
   return true;

}
bool 
CirMgr::readname(ifstream& in){
   string head;
   string name;
   int id;
   while(true){
      in>>head;
      if(in.eof())return true;
     // cout<<"head: "<<head<<endl;
      if(head=="c")return true;
      if(head[0]=='i'){
         myStr2Int(head.substr(1),id);
         id=idArray[id];
         in>>name;
         gateArray[id]->setname(name);
        
      }
      else if(head[0]=='o'){
         myStr2Int(head.substr(1),id);
         id=id+maxid+1;
         in>>name;
         gateArray[id]->setname(name);
      }
      else{
         return false;
      }
         
         //gateArray[id]->printGate();
         //cout<<"o : "<<id<<" "<<name<<endl;
      
      gatename.push_back(head+" "+name);
   }
   
}
 CirGate* 
 CirMgr::getGate(unsigned gid) const {
     if(gid>maxidTotal)return 0;
     if(gateArray[gid]->isUndef()) return 0; 
     return gateArray[gid];
   }
/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout<<endl;
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  PI  "<<setw(10)<<right<<input_num<<endl;
   cout<<"  PO  "<<setw(10)<<right<<output_num<<endl;
   cout<<"  AIG "<<setw(10)<<right<<And_num<<endl;
   cout<<"------------------"<<endl;
   cout<<"  Total"<<setw(9)<<right<<input_num+output_num+And_num<<endl;
}

void
CirMgr::printNetlist() const
{
   CirGate::count=0;
   CirGate::now_mark++;
   cout<<endl;
   for (size_t s=maxid+1;s<=maxidTotal;s++){
      //cout<<"mark "<<CirGate::now_mark<<"in\n";
      gateArray[s]->printDFS(true);
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (size_t s=0;s<input_num;s++){
      cout<<" "<<idArray[s];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t s=maxid+1;s<=maxidTotal;s++)cout<<" "<<s;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   bool firstTime=true;
   
   for(size_t s=0;s<=maxidTotal;s++){
      if(gateArray[s]->isfloat()){
         if(firstTime){
            cout<<"Gates with floating fanin(s):";
            firstTime=false;
         }
         cout<<" "<<s;
      }
   }
   if(!firstTime)cout<<endl;
   firstTime=true;
   for (size_t s=0;s<=maxidTotal;s++){
      if(gateArray[s]->isNotuse()){
         if(firstTime){
            cout<<"Gates defined but not used  :";
            firstTime=false;
         }
         cout<<" "<<s;
      }
   }
   if(!firstTime)cout<<endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{
   //to debug
   stringstream ss;
   CirGate::count=0;
   CirGate::now_mark++;
    for (size_t s=maxid+1;s<=maxidTotal;s++){
      //cout<<"mark "<<CirGate::now_mark<<"in\n";
      gateArray[s]->outDFS(ss);
   }
   outfile<<"aag "<<maxid<<" "<<input_num<<" 0 "<<output_num<<" "<<CirGate::count<<endl;
   for (size_t s=0;s<idArray.size();s++){
      outfile<<idArray[s]*2<<endl;
   }
   for (size_t s=0;s<outArray.size();s++){
      outfile<<outArray[s]<<endl;
   }
    
   outfile<<ss.str();
   for (size_t s=0;s<gatename.size();s++){
      outfile<<gatename[s]<<endl;
   }

   outfile<<"c"<<endl;

}
