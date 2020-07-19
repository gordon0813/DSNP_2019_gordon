/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  GateList todelete;
  dfsALL(false);
  for(size_t i =1;i<c_gate_map.size();i++){
    if(c_gate_map[i]==nullptr)continue;
    if(c_gate_map[i]->getType()==PI_GATE)continue;
    if(c_gate_map[i]->isMark())continue;
    if(c_gate_map[i]->getType()==AIG_GATE)c_aig_gate_num--;
    #ifdef debug_swp
    cout<<"start remove "<<i<<endl;
    #endif
    c_gate_map[i]->removeSelf();
    todelete.push_back(c_gate_map[i]);
    
    #ifdef debug
    cout<<"Sweeping: "<<c_gate_map[i]->gateType_s()<<"("<<c_gate_map[i]->getid()<<")"<<" removed...\n";
    #endif
    c_gate_map[i]=nullptr;
  }
  for(size_t i=0;i<todelete.size();i++){
    delete todelete[i];
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  GateList todelete;
  size_t id;
  bool inv;
  CirGate* tmp;
  for(size_t i=0;i<c_DFS_List.size();i++){
    if(c_DFS_List[i]->optbecome(id,inv)){
      tmp=c_gate_map[id];
      assert(tmp!=nullptr);
      assert(tmp->getType()!=NEVER_ACESS);
      if(c_DFS_List[i]->getType()==AIG_GATE)c_aig_gate_num--;
      #ifdef debug
      cout<<"Simplifying: "<<id<<" merging ";
      if(inv)cout<<"!";
      cout<<c_DFS_List[i]->getid()<<"...\n";
      #endif
      
      c_DFS_List[i]->replaceSelfBy(c_gate_map[id],inv);
      c_DFS_List[i]->removeSelf();
      todelete.push_back(c_DFS_List[i]);
      
    }
  }
  for(size_t i=0;i<todelete.size();i++){
    c_gate_map[ todelete[i]->getid()]=nullptr;
    delete todelete[i];
  }
  dfsALL(true);

}
CirGate* 
CirMgr::optTo(CirGate* origion){
  if(origion->getType()!=AIG_GATE)return origion;
  //constant 0

}
/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
