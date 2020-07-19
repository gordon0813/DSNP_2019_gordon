/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include<utility>
#include <iostream>
#include "cirDef.h"

using namespace std;



//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   static size_t now_mark;
   static size_t count;
   CirGate(size_t lineN,size_t gateN):lineNumber(lineN) ,gateNumber(gateN){mark=0;}
   virtual ~CirGate() {}
   

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return 0; }
   bool connect(CirGate* ,bool);
   virtual size_t max_fanin_num() const =0;
   void setMark()const{   mark=CirGate::now_mark; }
   bool isMark()const{return mark==CirGate::now_mark;}
   size_t getGateNo(){return gateNumber;}
   virtual bool isUndef() const =0;
   virtual GateType gateType()const =0;
   virtual bool isfloat()const=0;
   virtual bool isNotuse()const=0;
   virtual string gateType_s()const =0;
   void setname(string n){this->name=n;}
   // Printing functions
   virtual void printGate() const = 0;
   virtual void printSelf()const =0;
   void print_id_name()const{
     cout<<gateNumber;
      if(!name.empty()){
        cout<<" ("<<name<<")";
      }
     }
   void coutAgate(pair<CirGate*,bool>)const;
   void printFanin (const int depth,const int level)const;
   void printFanout  (const int depth,const int level)const;
  // void printnum(){cout<<gateNumber<<" ";}
   void printDFS(const bool&);
   void outDFS(stringstream& );
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
  

  
protected:
vector <pair<CirGate* , bool>> fanin;
  vector <pair<CirGate* , bool>> fanout;
  string name ;
  size_t lineNumber;
  size_t gateNumber;
  mutable size_t mark;
private:

};


class constGate :public CirGate{
  public:
  constGate(size_t s,size_t n):CirGate(s,n){}
  virtual ~constGate() {}
  virtual size_t max_fanin_num()const{return 0;}
  virtual void printGate()const;
  virtual bool isUndef()const{return false;}
  virtual bool isfloat()const{return false;}
  virtual bool isNotuse()const{return false;}
  virtual void printSelf()const;
  virtual GateType gateType()const{return CONST_GATE;}
  virtual string gateType_s()const{return "CONST";}
};
class undefGate :public CirGate{
  public:
  undefGate(size_t s,size_t n):CirGate(s,n){}
  virtual ~undefGate() {}
  virtual size_t max_fanin_num()const{return 0;}
  virtual void printGate()const;
  virtual bool isUndef()const{return true;}
  virtual bool isfloat()const{return false;}
  virtual bool isNotuse()const{return false;}
  virtual void printSelf()const;
  virtual GateType gateType()const{return UNDEF_GATE;}
  virtual string gateType_s()const{return "UNDEF";}
};
// input gate
class InGate :public CirGate{
  public:
  InGate(size_t s,size_t n):CirGate(s,n){}
  virtual ~InGate() {}
  virtual size_t max_fanin_num()const{return 0;}
  virtual void printGate()const;
  virtual bool isUndef()const{return false;}
  virtual bool isfloat()const{return false;}
  virtual bool isNotuse()const;
  virtual void printSelf()const;
  virtual GateType gateType()const{return PI_GATE;}
  virtual string gateType_s()const{return "PI";}
};
//output gate
class outGate :public CirGate{
  public:
  outGate(size_t s,size_t n):CirGate(s,n){}
  virtual ~outGate() {}
  virtual size_t max_fanin_num()const{return 1;}
  virtual void printGate ()const;
   virtual bool isUndef()const{return false;}
   virtual bool isfloat()const;
   virtual bool isNotuse()const{return false;}
   virtual void printSelf()const;
   virtual GateType gateType()const{return PO_GATE;}
   virtual string gateType_s()const{return "PO";}
};
//
class AndGate :public CirGate{
  public:
  AndGate(size_t s,size_t n):CirGate(s,n){}
  virtual ~AndGate() {}
  virtual size_t max_fanin_num()const{return 2;}
  virtual void printGate ()const;
   virtual bool isUndef()const{return false;}
   virtual bool isfloat()const;
   virtual bool isNotuse()const;
   virtual void printSelf()const;
   virtual GateType gateType()const{return AIG_GATE;}
   virtual string gateType_s()const{return "AIG";}
};

#endif // CIR_GATE_H
