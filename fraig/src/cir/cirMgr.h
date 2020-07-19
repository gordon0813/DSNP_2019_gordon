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
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset> //toremove
using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;
class FecGroup{
   public:
   FecGroup(){}
   FecGroup(const GateList& g){
      group=g;
   }
   ~FecGroup();
   size_t spliteInto(vector<FecGroup*>  & gl );
   void addGate(CirGate* g){group.push_back(g);}
   string toString(size_t sim,size_t id);
   size_t size(){return group.size();}
   void sorts();
   void connect();
   private:
   GateList group;
   friend CirMgr;
};
class CirMgr
{
public:
   CirMgr() {c_AllFec=vector<FecGroup*>();sats=nullptr;}
   ~CirMgr() ;

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if(gid>=this->c_gate_map.size()){
         return 0;
      }
       return c_gate_map[gid]; 
      }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void connectALL();
   void dfsALL(bool createList);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
  

private:
   size_t fecAll();
   void initsatsolver();
   void initGateList(size_t resize);
   bool readInGate(ifstream & in);
   bool readOutGate(ifstream & in);
   bool readAigGate(ifstream & in);
   bool readname(ifstream & in);
   bool readtosize_t(ifstream& file,vector<size_t>& tosim,size_t& simnum);
   void simAll();
   void connectFECToGate();
    void simresultToFile(const vector<size_t>& in,size_t l);
    void solveConstFraig(FecGroup* fg);
    void solvetwoAigFraig(FecGroup* fg);
    bool prove(CirGate* g1,CirGate* g2);
    void reportResult(const SatSolver& solver, bool result);
    void sortall();
    void maxaigandNum (const GateList& gl,size_t &maxaig,size_t &num)const;
    void  test();
    vector<size_t> createRandList();
   CirGate* optTo(CirGate* origion);
   ofstream           *_simLog;
   size_t c_lastid_aig;
   size_t c_in_gate_num;
   size_t c_out_gate_num;
   size_t c_aig_gate_num;
   size_t c_firstOutId;
   GateList c_in_List;
   GateList c_out_List;
   GateList c_gate_map;
   GateList c_DFS_List;
   vector<FecGroup*> c_AllFec;
   SatSolver* sats;   

   
};

#endif // CIR_MGR_H
#ifndef debug_mgr_read
#define debug_mgr_read
#undef debug_mgr_read
#endif
#ifndef debug_connect
#define debug_connect
#endif
#undef debug_connect
#ifndef debug_dfs
#define debug_dfs
#endif
#undef debug_dfs
#ifndef debug_swp
#define debug_swp
#endif
#undef debug_swp
#ifndef debug_opt
#define debug_opt
#endif
#undef debug_opt
#ifndef debug
#define debug
#endif
//#undef debug
#ifndef debug_strash
#define debug_strash
#endif
#undef debug_strash
#ifndef debug_sim
#define debug_sim
#endif
#undef debug_sim
#ifndef debug_fec
#define debug_fec
#endif
#undef debug_fec
#ifndef debug_sim_file
#define debug_sim_file
#endif
#undef debug_sim_file
#ifndef debug_fraig
#define debug_fraig
#endif
#undef debug_fraig