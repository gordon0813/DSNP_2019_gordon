/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
size_t CirGate::now_mark=0;
size_t CirGate::count=0;
void 
CirGate::printDFS(const bool& show){
   //this->printGate();
   if(this->isMark())return;
   if(this->isUndef())return;
   for(size_t s=0;s<fanin.size();s++){
      fanin[s].first->printDFS(show);
   }
   this->setMark();
   if(show){
   cout<<'['<<CirGate::count<<']'<<' ';
   CirGate::count++;
   //cout<<this->mark<<endl;
   this->printGate();
   }
   return;
}
void 
CirGate::outDFS(stringstream& os){
   size_t n1;
   size_t n2;
   if(isMark())return;
   
   for(size_t s=0;s<fanin.size();s++){
      fanin[s].first->outDFS(os);
   }
   setMark();
   if(gateType()!=AIG_GATE)return;
   CirGate::count++;
   n1=2* (this->fanin[0].first->gateNumber);
   os<<2* (this->gateNumber)<<" ";
   if(this->fanin[0].second)n1++;
   os<<n1<<" ";
   n2=2* (this->fanin[1].first->gateNumber);
   if(this->fanin[1].second)n2++;
   os<<n2;
   os<<endl;
   return ;
}

bool
CirGate::connect(CirGate* g ,bool Inverse){
   //this->reportGate();
   if(this->fanin.size()>=max_fanin_num())return false;
   //cout<<"connect\n";
   this->fanin.push_back(pair<CirGate* ,bool>(g,Inverse));
   g->fanout.push_back(pair<CirGate* ,bool>(this,Inverse));
   return true;
}
void
CirGate::reportGate() const
{
   stringstream ss;
   cout<<"=================================================="<<endl;
   ss<<gateType_s()<<"("<<gateNumber<<")";
   if(!name.empty())ss<<"\""<<name<<"\"";
   ss<<", line "<<lineNumber;
   cout<<"= "<<setw(46)<< left <<ss.str()<<" ="<<endl;
   cout<<"=================================================="<<endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   CirGate::now_mark++;
   printFanin(1,level+1);
}
void 
CirGate::printFanin(const int depth,const int level)const
{
   printSelf();
   if(depth==level){
      cout<<endl;
      return;
      }
   else if(max_fanin_num()==0){
      cout<<endl;
      return;
   }
   else if(isMark()){
      cout<<" (*)\n";
      return;
   }
   cout<<endl;
   setMark();
   for(size_t s=0;s<fanin.size();s++){
      for(int i=0;i<depth;i++)cout<<"  ";
      if(fanin[s].second)cout<<"!";
      fanin[s].first->printFanin(depth+1,level);
   }
   return;
}
void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   CirGate::now_mark++;
   printFanout(1,level+1);
}
void 
CirGate::printFanout(const int depth,const int level)const{
   printSelf();
   if(depth==level){
      cout<<endl;
      return;
      }
   else if(gateType()==PO_GATE){
      cout<<endl;
      return;
   }
   else if(isMark()){
      cout<<" (*)\n";
      return;
   }
   cout<<endl;
   setMark();
   for(size_t s=0;s<fanout.size();s++){
      for(int i=0;i<depth;i++)cout<<"  ";
      if(fanout[s].second)cout<<"!";
      fanout[s].first->printFanout(depth+1,level);
   }
   return;
}


void 
constGate::printGate()const{
    cout<<"CONST0"<<endl;
}

void
constGate::printSelf()const{cout<<"CONST 0";}

void 
InGate::printGate()const{
    cout<<"PI  ";
    print_id_name();
    cout<<endl;
}
void 
InGate::printSelf()const{
    cout<<"PI "<<gateNumber;
}
bool 
InGate::isNotuse()const{
   if(fanout.size()==0)return true;
   else return false;
}
void 
undefGate::printGate()const{
   cout<<"UNDEF "<<gateNumber<<endl;
}
void 
undefGate::printSelf()const{
   cout<<"UNDEF "<<gateNumber;
}
void 
AndGate::printGate ()const{
   //cout<<"print A1\n";
    string tmpl="";
    string tmpr="";
    string undefl="";
    string undefr="";
    if(fanin[0].second)tmpl="!";
    if(fanin[1].second)tmpr="!";
   // cout<<"print A3\n";
    if(fanin[0].first->isUndef()) undefl="*";
    if(fanin[1].first->isUndef()) undefr="*";
    //cout<<"print A2\n";
    cout<<"AIG "<<gateNumber<<" "<<undefl<<tmpl<<fanin[0].first->getGateNo()
    <<" "<<undefr<<tmpr<<fanin[1].first->getGateNo()<<endl;
   return;
}
void 
AndGate::printSelf ()const{
   cout<<"AIG "<<gateNumber;
}
bool 
AndGate::isNotuse()const{
   if(fanout.size()==0)return true;
   else return false;
}
bool 
AndGate::isfloat()const{
   if(fanin[0].first->isUndef() || fanin[1].first->isUndef())return true;
   else return false;
}
void 
outGate::printGate ()const{
    string tmp=" ";
    if(fanin[0].second)tmp=" !";
    cout<<"PO  "<<gateNumber<<tmp;
    cout<<fanin[0].first->getGateNo();
    if(!name.empty())cout<<" ("<<name<<")";
    cout<<endl;
    return;
}
void 
outGate::printSelf ()const{
   cout<<"PO "<<gateNumber;
}
bool 
outGate::isfloat()const{
   if(fanin[0].first->isUndef())return true;
   else return false;
}
