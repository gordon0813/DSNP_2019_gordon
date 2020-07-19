/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {
     //cout<<"indelete\n";
     
     for(size_t s=0;s<maxidTotal;s++){
       delete gateArray[s];
     }
     delete[] gateArray;
     
     //cout<<"outdelete\n";
     
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const ;

   // Member functions about circuit construction
   bool readCircuit(const string&);
   bool connectCircuit(CirGate ** g,size_t* lh,size_t * rh,size_t length);
   bool readname(ifstream & in);
   // Member functions about circuit reporting
   void printSummary() const;//down
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
  size_t input_num;
  size_t output_num;
  size_t And_num;
  size_t maxid;
  size_t maxidTotal;
  CirGate** gateArray;
  vector<size_t> idArray;
  vector<size_t> outArray;
  vector<string> gatename;
};

#endif // CIR_MGR_H
