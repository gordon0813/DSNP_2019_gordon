/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>

#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
size_t CirGate::now_mark=0;
/**************************************/
/*   class CirGate member functions   */
/**************************************/
size_t 
CirGate::faninid(size_t i){return fanin[i].getGate()->_satvar;}
bool 
CirGate::fanininv(size_t i){return fanin[i].isinv();}
void
CirGate::connect(GateList& gl){
   for (int i=0;i<this->fanin.size();i++){
      
      size_t id=(size_t)fanin[i].getGate();
      #ifdef debug_connect
      cout<<"connect fanin "<<i<<" is id "<<id<<endl;
      #endif
      if(gl[id]==nullptr){
         gl[id]= new undefGate(0,id);
      }
      this->fanin[i].setGate(gl[id]);
      gl[id]->fanout.push_back(WrapGate(this,fanin[i].isinv()));
      #ifdef debug_connect
      cout<<"connect fanin pointer "<<fanin[i].getGate()<<endl;
      #endif
   }
}
void
CirGate::dfs(bool save,GateList* dfsList){
   #ifdef debug_dfs
   cout<<"dfs to id: "<<this->id<<endl;
   #endif
   if(this->isMark())return;
   if(this->getType()==UNDEF_GATE){setMark();return;}
   #ifdef debug_dfs
   cout<<"have "<<fanin.size()<<" fanin"<<endl;
   #endif
   for(size_t s=0;s<fanin.size();s++){
      fanin[s].getGate()->dfs(save,dfsList);
   }
   this->setMark();
   #ifdef debug_dfs
   cout<<"push "<<this->id<<endl;
   #endif
   if(save){
      dfsList->push_back(this);
   }
   return;
}
void
CirGate::reportGate() const
{
   size_t s1=1;
   stringstream ss;
   cout<<"================================================================================"<<endl;
   ss<<gateType_s()<<"("<<id<<")";
   if(!name.empty())ss<<"\""<<name<<"\"";
   ss<<", line "<<lin_no;
   cout<<"= "<<ss.str()<<endl;
   cout<<"= FECs:";
   if(fec!=nullptr)cout<<fec->toString(sim,id);
   cout<<endl;
   cout<<"= Value: ";
   for(size_t i=s1<<63,j=0; i>0; i>>=1,j++){
      if(j!=0 && j%8==0)cout<<"_";
      if(sim&i)cout<<"1";
      else cout<<"0";
   }
   cout<<endl;
   cout<<"================================================================================"<<endl;
}
void
CirGate::printSelf()const{
   cout<<gateType_s()<<" "<<id;
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
   printSelf();//
   if(depth==level){
      cout<<endl;
      return;
      }
   else if(this->fanin.size()==0||this->getType()==UNDEF_GATE){//
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
      if(fanin[s].isinv())cout<<"!";
      fanin[s].getGate()->printFanin(depth+1,level);
   }
   return;
}
void
CirGate::reportFanout(int level) const
{
   CirGate::now_mark++;
   printFanout(1,level+1);
   assert (level >= 0);
}
void 
CirGate::printFanout(const int depth,const int level)const
{
   printSelf();//
   if(depth==level){
      cout<<endl;
      return;
      }
   else if(this->fanout.size()==0||this->getType()==UNDEF_GATE){//
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
      if(fanout[s].isinv())cout<<"!";
      fanout[s].getGate()->printFanout(depth+1,level);
   }
   return;
}
void
CirGate::printALLcontain()const{
   stringstream ss;
   ss<<id;
   for(int i=0;i<fanin.size();i++){
      ss<<" ";
      if(fanin[i].getGate()->getType()==UNDEF_GATE)ss<<"*";
      if(fanin[i].isinv())ss<<"!";
      ss<<fanin[i].getGate()->id;
   }
   if(!name.empty())ss<<" ("<<name<<")";
   cout<<ss.str()<<endl;
}
void 
CirGate::debugprint()const{
     cout<<"id: "<<id<<" line: "<<lin_no<<" mark: "<<mark<<" sim: "<<sim<<endl;
    for(size_t i=0;i<fanin.size();i++){
      fanin[i].printdebug();
   }
}
void
CirGate::removeSelf(){
   for(size_t i=0;i<fanin.size();i++){
      fanin[i].disconnectWithfanout(this);
   }
   this->typ=NEVER_ACESS;
   #ifdef debug_swp
   cout<<"remove id:"<<id<<endl;
   #endif
   
}
bool 
CirGate::addfanout(CirGate* g,bool inv){
   fanout.push_back(WrapGate(g,inv));
}
void
CirGate::setfec(FecGroup* fg){fec=fg;}
bool 
CirGate::replacefanin(CirGate *ori,CirGate*become,bool inv){
   for(size_t i=0;i<fanin.size();i++){
      if(fanin[i].getGate()==ori){
         fanin[i].setGate(become);
         fanin[i].setinv(fanin[i].isinv()^inv);
         #ifdef debug_opt
         cout<<"gate id :"<<id<<" replace fanin to "<<fanin[i].getid()<<" inv:"<<fanin[i].isinv()<<endl;
         #endif
         become->addfanout(this,fanin[i].isinv());
         return true;
      }
   }
   cerr<<"error: replace fanin :"<<ori->getid()<<" fail"<<endl;
   assert(false);
}
bool
CirGate::replaceSelfBy(CirGate* g,bool inv){
   #ifdef debug_swp
   cout<<"replace id: "<<id<<" to "<<g->getid()<<" with inv "<<inv<<endl;
   #endif
   for(size_t i=0;i<fanout.size();i++){
      fanout[i].getGate()->replacefanin(this,g,inv);
   }
}

void 
OutGate::towrite(ostream& outfile){
   outfile<<(fanin[0].isinv()?2*fanin[0].getid()+1:2*fanin[0].getid())<<endl;
}
bool 
InGate::isNotuse()const{
   if(fanout.size()==0)return true;
   else return false;
}
OutGate::OutGate(size_t s,size_t n,size_t inaig):CirGate(s,n){
    typ=PO_GATE; 
    fanin.push_back(WrapGate(inaig));
    }

bool 
OutGate::isfloat()const{
   if(fanin[0].getGate()->isundef())return true;
   else return false;
}
void 
OutGate::simulate(){
   sim=fanin[0].simulate();
}
AigGate::AigGate(size_t s,size_t n,size_t n1,size_t n2):CirGate(s,n){
   typ=AIG_GATE;
   
   fanin.push_back(WrapGate(n1));
   fanin.push_back(WrapGate(n2));
}
bool 
AigGate::isNotuse()const{
   if(fanout.size()==0)return true;
   else return false;
}
bool 
AigGate::isfloat()const{
   if(fanin[0].getGate()->isundef() || fanin[1].getGate()->isundef())return true;
   else return false;
}
bool
AigGate::optbecome(size_t& id,bool& inv)const{
   int const01l=fanin[0].isconst01();
   int const01r=fanin[1].isconst01();
   int eqorinv=fanin[0].equalorInv(fanin[1]);
   if(const01l==0||const01r==0){
      id=0; 
      inv=false; 
      return true;
   }
   if(const01l==1){
      id=fanin[1].getid();
      inv=fanin[1].isinv();
      return true;
   }
   else if(const01r==1){
      id=fanin[0].getid();
      inv=fanin[0].isinv();
      return true;
   }
   if(eqorinv==1){
      id=fanin[0].getid();
      inv=fanin[0].isinv();
      return true;
   }else if(eqorinv==-1){
      id=0;
      inv=false;
      return true;
   }
   return false;
   
}
size_t
AigGate::hash(){
   size_t gidl=fanin[0].getid();
   size_t gidr=fanin[1].getid();
   size_t invl=fanin[0].isinv()?1:0;
   size_t invr=fanin[1].isinv()?1:0;
   size_t tmp;
   if(gidl>gidr){
      tmp=gidl;
      gidl=gidr;
      gidr=tmp;
      tmp=invl;
      invl=invr;
      invr=tmp;
   }
   return (gidl<<31|gidr|invl<<63|invr<<62);
}
void 
AigGate::simulate(){
   sim=fanin[0].simulate()&fanin[1].simulate();
}
void 
AigGate::towrite(ostream& outfile){
    outfile<<2*id;
    outfile<<" "<<(fanin[0].isinv()?2*fanin[0].getid()+1:2*fanin[0].getid());
    outfile<<" "<<(fanin[1].isinv()?2*fanin[1].getid()+1:2*fanin[1].getid());
    outfile<<endl;
}
void
WrapGate::printdebug()const{
      cout<<"address: "<<(size_t)gate<<" inverse: "<<inv<<endl;
}
WrapGate::WrapGate(size_t n){
      if(n%2){inv=true;}
      else {inv=false;}
      gate=(CirGate*)(n/2);
}
WrapGate::WrapGate(CirGate* g,bool i):gate(g),inv(i){assert(g!=nullptr);}
void
WrapGate::disconnectWithfanin(){
   for(size_t i=0;i<gate->fanin.size();i++){
      if(gate->fanin[i].getid()==gate->getid()){
         gate->fanin.erase( gate->fanin.begin()+i);
      }
   }
}
void
WrapGate::disconnectWithfanout(CirGate* g){
   if(gate->getType()==NEVER_ACESS)return;
   for(size_t i=0;i<gate->fanout.size();i++){
      if(gate->fanout[i].getid()==g->getid()){
         gate->fanout.erase( gate->fanout.begin()+i);
         return;
      }
   }
   cerr<<"fatal error disconnect fail cant remove id:"<<g->getid()<<endl;
   assert(false);
}
int 
WrapGate::equalorInv(const WrapGate& wg)const{
   if(gate==wg.getGate()&&inv==wg.isinv())return 1;
   else if(gate==wg.getGate()&&inv==!wg.isinv())return -1;
   else return 0;
}
 size_t 
 WrapGate::simulate()const{
      if(inv)return ~(gate->sim);
      else return gate->sim;
    }

