/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
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
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
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
CirMgr::~CirMgr(){
   for(FecGroup* fg:c_AllFec){
      if(fg!=nullptr)delete fg;
   }
   for(CirGate* g:c_gate_map){
      if(g!=nullptr){
         delete g;
      }
   }
}
bool
CirMgr::readCircuit(const string& fileName)
{
   string s;
   ifstream inputFile(fileName);
   if(!inputFile)return false;
   inputFile>>s>>c_lastid_aig>>c_in_gate_num>>c_out_gate_num>>c_out_gate_num>>c_aig_gate_num;
   c_firstOutId=c_lastid_aig+1;
   
   #ifdef debug_mgr_read
   cout<<"header"<<endl;
   cout<<"s: "<<s<<" lastid_aig: "<<c_lastid_aig<<" in_gate_num: "<<c_in_gate_num<<" out_gate_num: "<<c_out_gate_num<<" aig_gate_num: "<<c_aig_gate_num<<endl;
   #endif
   initGateList(c_lastid_aig+c_out_gate_num);
   readInGate(inputFile);
   readOutGate(inputFile);
   readAigGate(inputFile);
   readname(inputFile);
   connectALL();
   dfsALL(true);
   return true;
}
void
CirMgr::initGateList(size_t resize){
   c_gate_map=GateList(1);
   for(size_t s=0;s<resize;s++){
      c_gate_map.push_back(nullptr);
   }
   c_gate_map[0]=new constGate(0,0);
}
bool
CirMgr::readInGate( ifstream & in){
   size_t inid;
   size_t lineno=2;
   CirGate* tmpG;
   for(size_t i=0;i<this->c_in_gate_num;i++){
      in>>inid;
      inid/=2;
      tmpG=new InGate(lineno,inid);
      this->c_in_List.push_back(tmpG);
      #ifdef debug_mgr_read
      cout<<"in read in\n";
      assert(c_gate_map[inid]==nullptr);
      #endif
      this->c_gate_map[inid]=tmpG;
      lineno++;
      #ifdef debug_mgr_read
      tmpG->debugprint();
      #endif
   }
   return true;
}
bool
CirMgr::readOutGate( ifstream & in){
   size_t outfanin;
   size_t outid=c_lastid_aig+1;
   size_t lineno=c_in_gate_num+2;
   CirGate* tmpG;
   for(size_t i=0;i<this->c_out_gate_num;i++){
      
      in>>outfanin;
      tmpG=new OutGate(lineno,outid,outfanin);
      this->c_out_List.push_back(tmpG);
      #ifdef debug_mgr_read
      cout<<"in read Out\n";
      assert(c_gate_map[outid]==nullptr);
      #endif
      this->c_gate_map[outid]=tmpG;
      lineno++;
      outid++;
      #ifdef debug_mgr_read
      tmpG->debugprint();
      #endif
   }
}
bool
CirMgr::readAigGate( ifstream & in){
   size_t Aigfanin1;
   size_t Aigfanin2;
   size_t Aigid;
   size_t lineno=c_in_gate_num+c_out_gate_num+2;
   CirGate* tmpG;
   for(size_t i=0;i<this->c_aig_gate_num;i++){
     
      in>>Aigid>>Aigfanin1>>Aigfanin2;
      Aigid/=2;
      tmpG=new AigGate(lineno,Aigid,Aigfanin1,Aigfanin2);
      #ifdef debug_mgr_read
      cout<<"in read aig\n";
      assert(c_gate_map[Aigid]==nullptr);
      #endif
      
      this->c_gate_map[Aigid]=tmpG;
      lineno++;
      Aigid++;
      #ifdef debug_mgr_read
      tmpG->debugprint();
      #endif
   }
}
bool 
CirMgr::readname(ifstream & in){
   string head;
   string name;
   int id;
   while(true){
      in>>head;
      if(in.eof())return true;
      if(head=="c")return true;
      if(head[0]=='i'){
         myStr2Int(head.substr(1),id);
         in>>name;
         c_in_List[id]->setName(name);
        
      }
      else if(head[0]=='o'){
         myStr2Int(head.substr(1),id);
         in>>name;
         c_out_List[id]->setName(name);
      }
      else{
         return false;
      }
   }
}
void 
CirMgr::connectALL(){
   for(int i=0;i<this->c_gate_map.size();i++){
      #ifdef debug_connect
      cout<<"start connect  id: "<<i<<endl;
      #endif
      if(c_gate_map[i]!=nullptr&&c_gate_map[i]->getType()!=UNDEF_GATE){
         c_gate_map[i]->connect(this->c_gate_map);
      }
   }
}
void
CirMgr::dfsALL(bool createList){
   if(createList){
      this->c_DFS_List=GateList();
   }
   CirGate::now_mark++;
   
   for(int i=c_firstOutId;i<this->c_gate_map.size();i++){
      if(c_gate_map[i]==nullptr)continue;
      c_gate_map[i]->dfs(createList,&c_DFS_List);
   }
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
   #ifdef debug_mgr_read
   
   #endif

   cout<<endl;
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  PI  "<<setw(10)<<right<<c_in_gate_num<<endl;
   cout<<"  PO  "<<setw(10)<<right<<c_out_gate_num<<endl;
   cout<<"  AIG "<<setw(10)<<right<<c_aig_gate_num<<endl;
   cout<<"------------------"<<endl;
   cout<<"  Total"<<setw(9)<<right<<c_in_gate_num+c_out_gate_num+c_aig_gate_num<<endl;
}

void
CirMgr::printNetlist() const
{

   cout << endl;
   for (unsigned i = 0, n = c_DFS_List.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      c_DFS_List[i]->printGate();
   }

}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i=0;i<this->c_in_gate_num;i++){
      cout<<" "<<c_in_List[i]->getid();
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i=0;i<this->c_out_gate_num;i++){
      cout<<" "<<c_out_List[i]->getid();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   bool firstTime=true;
   
   for(size_t s=0;s<c_gate_map.size();s++){
      if(c_gate_map[s]==nullptr)continue;
      if(c_gate_map[s]->isfloat()){
         if(firstTime){
            cout<<"Gates with floating fanin(s):";
            firstTime=false;
         }
         cout<<" "<<s;
      }
   }
   if(!firstTime)cout<<endl;
   firstTime=true;
   for (size_t s=0;s<c_gate_map.size();s++){
      if(c_gate_map[s]==nullptr)continue;
      if(c_gate_map[s]->isNotuse()){
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
CirMgr::printFECPairs() const
{
   size_t basesim;
   for(size_t i=0;i<c_AllFec.size();i++){
      basesim=c_AllFec[i]->group[0]->getsim();
      cout<<"["<<i<<"]";
      for(CirGate* g :c_AllFec[i]->group){
         cout<<((g->getsim()==basesim)?" ":" !")<< (g->getid());
      }
      cout<<endl;
   }
   
}

void
CirMgr::writeAag(ostream& outfile) const

{
 
   outfile<<"aag "<<c_lastid_aig<<" "<<c_in_List.size()<<" "<<0<<" "<<c_out_List.size()<<" "<<c_aig_gate_num<<endl;
   for(CirGate* g:c_in_List){
      g->towrite(outfile);
   }
   for(CirGate* g:c_out_List){
      g->towrite(outfile);
   }
   for(CirGate* g:c_DFS_List){
      if(g->getType()==AIG_GATE){
         g->towrite(outfile);
      }
   }
   for(size_t i=0;i<c_in_List.size();i++){
      string name=c_in_List[i]->getname();
      if( ! name.empty()){
         outfile<<"i"<<i<<" "<<name<<endl;
      }
   }
   for(size_t i=0;i<c_out_List.size();i++){
      string name=c_out_List[i]->getname();
      if( ! name.empty()){
         outfile<<"o"<<i<<" "<<name<<endl;
      }
   }
   outfile<<"c"<<endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   
   GateList tmp;
   size_t maxid;
   size_t aignum;
   size_t innum=0;
   CirGate::now_mark++;
   g->dfs(true,&tmp);
   maxaigandNum( tmp,maxid,aignum);
   for(CirGate* g:c_in_List){
      if(g->isMark())innum++;
   }
   outfile<<"aag "<<maxid<<" "<<innum<<" "<<0<<" "<<1<<" "<<aignum<<endl;
   for(CirGate* g:c_in_List){
      if(g->isMark())g->towrite(outfile);
   }
   outfile<<2*g->getid()<<endl;
   for(CirGate* g:tmp){
      if(g->getType()==AIG_GATE){
         g->towrite(outfile);
      }
   }
   size_t j=0;
   for(size_t i=0;i<c_in_List.size();i++){
      if(!c_in_List[i]->isMark())continue;
      string name=c_in_List[i]->getname();
      if( ! name.empty()){
         outfile<<"i"<<j<<" "<<name<<endl;
      }
      j++;
   }
   outfile<<"o0 "<< g->getid()<<"\nc"<<endl;
}

 void 
 CirMgr::maxaigandNum(const GateList& gl,size_t &maxaig,size_t &num)const{
   maxaig=0;
   num=0;
   for(CirGate* g:gl){
      if(g->isAig() ){
         num++;
         if(g->getid()>maxaig)maxaig=g->getid();
      }
   }
}