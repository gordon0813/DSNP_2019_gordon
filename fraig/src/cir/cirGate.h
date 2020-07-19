/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class WrapGate;
class FecGroup;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------

class CirGate
{
public:
  static size_t now_mark;
   CirGate(){}
   CirGate(size_t lineN,size_t gateN):lin_no(lineN) ,id(gateN){mark=0;sim=0;}
   virtual ~CirGate() {}
   Var _satvar;
   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return 0; }
   virtual bool isAig() const { return typ==AIG_GATE; }
   const size_t getid()const{return this->id;}
   void setName(string s){this->name=s;}
   void debugprint()const;
   GateType getType()const{return this->typ;}
   virtual string gateType_s()const =0;
   void connect(GateList& gl);
   virtual bool isfloat()const=0;
   virtual bool isNotuse()const=0;
   bool isundef(){return typ==UNDEF_GATE;}
   size_t getsim()const{return sim;}
   size_t faninid(size_t i);
   bool fanininv(size_t i);
   FecGroup* getgroup(){return fec;}
   virtual void towrite(ostream& outfile){}
   string getname(){return name;}

   //dfs
  void dfs(bool save,GateList* dfsList);
  bool isMark()const{return mark==CirGate::now_mark;}
  void setMark()const{  mark=CirGate::now_mark; }
   // Printing functions
   virtual void printGate() const {}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void printALLcontain()const;
   void printSelf()const;
   
   //print recursive
   void printFanin (const int depth,const int level)const;
   void printFanout  (const int depth,const int level)const;
   //removefunc
   void removeSelf();
   //optfunction
   virtual bool optbecome(size_t& id,bool& inv)const{return false;}
   virtual bool replaceSelfBy(CirGate* g,bool inv);
   virtual bool replacefanin(CirGate *ori,CirGate*become,bool inv);
   virtual bool addfanout(CirGate *g,bool inv);
   //strash
   virtual size_t hash(){return id;}
   //sim
   virtual void setsim(size_t s){cerr<<"should not set sim of not pi gate\n";}
   virtual void simulate(){cerr<<"simulate error : gate type error\n";}
   void setfec(FecGroup* fg);
   void unsetfec(){fec=nullptr;}
  
   
private:
protected:
  mutable size_t mark;
  size_t id;
  size_t lin_no;
  size_t sim;
  
  string name;
  FecGroup* fec;
  GateInvList fanin;
  GateInvList fanout;
  GateType typ;
friend WrapGate;
};

class constGate :public CirGate{
  public:
  constGate(size_t s,size_t n):CirGate(s,n){typ=CONST_GATE;}
  virtual ~constGate() {}
  virtual string gateType_s()const{return "CONST";}
  virtual void printGate()const{cout<<"CONST0"<<endl;}
  virtual bool isfloat()const{return false;}
  virtual bool isNotuse()const{return false;}
  virtual void simulate(){sim=0;}
};
class undefGate :public CirGate{
  public:
  undefGate(size_t s,size_t n):CirGate(s,n){typ=UNDEF_GATE;}
  virtual ~undefGate() {}
  virtual string gateType_s()const{return "UNDEF";}
  virtual void printGate()const{cout<<"UNDEF "<<id<<endl;}
  virtual bool isfloat()const{return false;}
  virtual bool isNotuse()const{return false;}
};
class InGate :public CirGate{
  public:
  InGate(size_t s,size_t n):CirGate(s,n){typ=PI_GATE;}
  virtual ~InGate() {}
  virtual string gateType_s()const{return "PI";}
  virtual void printGate()const{ cout<<"PI  ";printALLcontain();}
  virtual bool isfloat()const{return false;}
  virtual bool isNotuse()const;
  virtual void setsim(size_t s){sim=s;}
  virtual void simulate(){}
  virtual void towrite(ostream& outfile){outfile<<2*id<<endl;}
};
class OutGate :public CirGate{
  public:
  OutGate(size_t s,size_t n,size_t inaig);
  virtual ~OutGate() {}
  virtual string gateType_s()const{return "PO";}
  virtual void printGate()const{ cout<<"PO  ";printALLcontain();}
  virtual bool isfloat()const;
  virtual bool isNotuse()const{return false;}
  virtual void simulate();
  virtual void towrite(ostream& outfile);
};
class AigGate :public CirGate{
  public:
  AigGate(size_t s,size_t n,size_t in1,size_t in2);
  virtual ~AigGate() {}
  virtual string gateType_s()const{return "AIG";}
  virtual void printGate()const{ cout<<"AIG ";printALLcontain();}
  virtual bool isfloat()const;
  virtual bool isNotuse()const;
  virtual bool optbecome(size_t& id,bool& inv)const;
  virtual size_t hash();
  virtual void simulate();
  virtual void towrite(ostream& outfile);
};
class WrapGate{
  public:
    WrapGate(CirGate* g,bool i);
    WrapGate(size_t n);
    bool isinv()const{return inv;}
    void setinv(bool i){inv=i;}
    CirGate* getGate()const{return gate;}
    void setGate(CirGate* g){gate=g;}
    size_t getid()const{return gate->getid();}
    void printdebug() const;
    void disconnectWithfanin();
    void disconnectWithfanout(CirGate* g);
    // equal 1 inv -1 other 0
    int equalorInv(const WrapGate& wg)const;
    int isconst01()const{
      if(gate->getType()==CONST_GATE &&inv==false)return 0;
      else if(gate->getType()==CONST_GATE &&inv==true)return 1;
      else return -1;
    }
    size_t simulate()const;
   
    
    
    
  private:
    CirGate* gate;
    bool inv;
};

#endif // CIR_GATE_H
