/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>

#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void CirMgr::strash()
{
  GateList todelete;
  unordered_map<size_t, CirGate *> strashmap;
  CirGate *mergeinto;
  CirGate *other;
  strashmap.reserve(c_DFS_List.size() * 2 - 1);

  for (size_t i = 0; i < c_DFS_List.size(); i++)
  {
    other = c_DFS_List[i];
    if (!(other->getType() == AIG_GATE))
      continue;

    size_t hashkey = other->hash();
#ifdef debug_strash
    cout << bitset<64>(hashkey) << endl;
#endif
    unordered_map<size_t, CirGate *>::const_iterator iter = strashmap.find(hashkey);

    if (iter != strashmap.end())
    {
      mergeinto = iter->second;
#ifdef debug
      cout << "Strashing: " << mergeinto->getid() << " merging " << other->getid() <<"..."<< endl;
#endif
      other->replaceSelfBy(mergeinto, false);
      other->removeSelf();
      c_aig_gate_num--;
      todelete.push_back(other);
    }
    else
    {
      strashmap.insert(pair<size_t, CirGate *>(hashkey, other));
    }
  }
  for (size_t i = 0; i < todelete.size(); i++)
  {
    c_gate_map[todelete[i]->getid()] = nullptr;
    delete todelete[i];
  }

  dfsALL(true);
}

void CirMgr::fraig()
{
  initsatsolver();
  FecGroup *constgroup = c_gate_map[0]->getgroup();
  FecGroup *fg;
  for (size_t i = 0; i < c_AllFec.size(); i++)
  {
    
    fg = c_AllFec[i];
    //cout << "size " << c_AllFec.size() << endl;
    #ifdef debug_fraig
    for (CirGate *g : fg->group)
      cout << g->getid() << endl;
    #endif
    if (fg == constgroup)
    {
      solveConstFraig(fg);
    }
    else
    {
      solvetwoAigFraig(fg);
    }
  }
  for (FecGroup *fg : c_AllFec)
  {
    delete fg;
  }
  c_AllFec.clear();
  dfsALL(true);
  delete sats;
  sats=nullptr;
}
void CirMgr::solveConstFraig(FecGroup *fg)
{
  bool result = true;
  size_t todeleteid;
#ifdef debug_fraig
  cout << "in solve const" << endl;
#endif
  for (size_t i = 0; i < fg->group.size(); i++)
  {
#ifdef debug
    cout << "prove is const: " << fg->group[i]->getid() << endl;
#endif
    if (fg->group[i]->getType() == CONST_GATE)
      continue;
    else
    {
      result = prove(fg->group[i], 0);
    }
#ifdef debug
    cout << "prove const id: " << fg->group[i]->getid() << (!result ? " is const " : " not const") << endl;
#endif
    if (!result)
    {
      //cout<<"start merge"<<endl;
      fg->group[i]->replaceSelfBy(c_gate_map[0], fg->group[i]->getsim()!=0);
      fg->group[i]->removeSelf();
      todeleteid = fg->group[i]->getid();
      delete c_gate_map[todeleteid];
      c_gate_map[todeleteid] = nullptr;
      c_aig_gate_num--;
      fg->group[i] = 0;
    }
  }
}
void CirMgr::solvetwoAigFraig(FecGroup *fg)
{
  size_t todeleteid;
  bool result = true;
  CirGate *g1;
  CirGate *g2;
  for (size_t i = 0; i < fg->group.size(); i++)
  {
    g1 = fg->group[i];
    if (g1 == nullptr)
      continue;
    for (size_t j = i + 1; j < fg->group.size(); j++)
    {
      g2 = fg->group[j];
      if (g2 == nullptr)
        continue;
#ifdef debug
      cout << "prove  id1: " << fg->group[i]->getid() << " id2 " << fg->group[j]->getid() << endl;
#endif
      result = prove(g1, g2);
      if (!result)
      {
#ifdef debug
        cout << "merge id:" << fg->group[j]->getid() << " into " << fg->group[i]->getid() << endl;
#endif
        g2->replaceSelfBy(g1, g2->getsim() != g1->getsim());
        g2->removeSelf();
        todeleteid=g2->getid();
        delete g2;
        c_gate_map[ todeleteid]=nullptr;
        c_aig_gate_num--;
        fg->group[j] = 0;
#ifdef debug_fraig
        cout << "merge end" << endl;
#endif
      }
    }
  }
#ifdef debug_fraig
  cout << "proof end" << endl;
#endif

#ifdef debug_fraig
  cout << "clear end" << endl;
#endif
  dfsALL(true);
#ifdef debug_fraig
  cout << "dfs end" << endl;
#endif
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void CirMgr::initsatsolver()
{

  if (sats == nullptr)
  {
    sats = new SatSolver();
    sats->initialize();
#ifdef debug_fraig
    cout << "create solver start\n";
#endif
  }
  c_gate_map[0]->_satvar = sats->newVar();
  
  sats->assertProperty(c_gate_map[0]->_satvar, false);
  //cout<<"define const id "<<c_gate_map[0]->_satvar<<endl;
  for (CirGate *g : c_DFS_List)
  {
    if(g->getType()!=CONST_GATE &&g->getType()!=PO_GATE)g->_satvar = sats->newVar();
    //cout<<"define gate id "<<g->getid()<<" gate sat "<<g->_satvar<<endl;
    if (g->getType() == AIG_GATE)
    {

#ifdef debug_fraig
      cout << "insert: " << g->_satvar << " " << g->faninid(0) << " " << g->fanininv(0) << " " << g->faninid(1) << " " << g->fanininv(1) << endl;
#endif
      sats->addAigCNF(g->_satvar, g->faninid(0), g->fanininv(0), g->faninid(1), g->fanininv(1));
      //cout<<g->_satvar<<" "<< g->faninid(0)<<" "<<g->fanininv(0)<<" "<<g->faninid(1)<<" "<<g->fanininv(1)<<endl;
#ifdef debug_fraig
      cout << "insert finish\n";
#endif
    }
  }
  
#ifdef debug_fraig
  cout << "create solver finish\n";
#endif
}
bool CirMgr::prove(CirGate *g1, CirGate *g2)
{
  assert(g1 != nullptr);
  bool result;
  if (g2 == nullptr)
  {
    size_t newoutc = sats->newVar();
    #ifdef debug_fraig
    cout<<"const id "<<c_gate_map[0]->_satvar;
    cout<<" and const id "<<g1->_satvar<<" "<<(g1->getsim()!=0)<<endl;
    #endif
    sats->addXorCNF(newoutc, g1->_satvar,false, c_gate_map[0]->_satvar,g1->getsim()!=0);
   sats->assumeProperty(newoutc, true);

  }
  else
  {
    size_t newout = sats->newVar();
    sats->addXorCNF(newout, g1->_satvar, false, g2->_satvar, g1->getsim() != g2->getsim());
    sats->assumeProperty(newout, true);
  }
  result=sats->assumpSolve();
  //cout<<"==============result"<<result<<endl;
  sats->assumeRelease();
  return result;
}
void 
CirMgr::test(){
  size_t newout = sats->newVar();
  sats->addXorCNF(newout, 1, false, 7, false );
  sats->assumeProperty(newout, true);
  //cout<<"====================test   ans:    "<<sats->assumpSolve()<<endl;
}