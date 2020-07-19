/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  size_t fail=0;
   GateList fecstart;
  vector<size_t>simlist;
  size_t totalsimnum=0;
  size_t simnum=0;
  size_t newfecnum=0;
  assert(c_AllFec.size()==0);
  for(int i=0;i<c_DFS_List.size();i++){
    if(c_DFS_List[i]->getType()==AIG_GATE ){
      fecstart.push_back(c_DFS_List[i]);
    }
  }
  fecstart.push_back(c_gate_map[0]);
  this->c_AllFec.push_back(new FecGroup(fecstart));
  //start sim
  for(size_t k=0;k<100;k++){
    simlist=createRandList();
    for(size_t i=0;i<c_in_List.size();i++){
      c_in_List[i]->setsim(simlist[i]);
    }
    simAll();
    simresultToFile(simlist,(k+1)*64);
    newfecnum=fecAll();
    if(newfecnum==0){
      fail++;
      if(fail>3 ||k==99){
        cout<<k*64<<" patterns simulated.\n";
        break;
        }
    }else{
      fail=0;
    }
  }
  sortall();
  connectFECToGate();
}
vector<size_t>
CirMgr::createRandList(){
  vector<size_t> re;
  for(size_t i=0;i<c_in_gate_num;i++){
    size_t r30 = RAND_MAX*rand()+rand();
    size_t s30 = RAND_MAX*rand()+rand();
    size_t t4  = rand() & 0xf;
    re.push_back((r30 << 34) + (s30 << 4) + t4);
  }
  return re;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  bool firsttime=true;
  vector<size_t> simlist;
  GateList fecstart;
  size_t totalsimnum=0;
  size_t simnum=0;
  size_t newfecnum=0;
  bool finish;
  if(c_AllFec.size()!=0)firsttime=false;
  while(readtosize_t(patternFile ,simlist, simnum)){
    if(firsttime){
      for(int i=0;i<c_DFS_List.size();i++){
        if(c_DFS_List[i]->getType()==AIG_GATE ){
          fecstart.push_back(c_DFS_List[i]);
        }
      }
      fecstart.push_back(c_gate_map[0]);
      this->c_AllFec.push_back(new FecGroup(fecstart));
      firsttime=false;
    }
    totalsimnum+=simnum;
    #ifdef debug_sim
    for(size_t i=0;i<simlist.size();i++){
      cout<<bitset<64>(simlist[i])<<endl;
    }
    cout<<"total sim:"<<simnum<<endl;
    #endif
    
    //set sim from file
    for(size_t i=0;i<c_in_List.size();i++){
      c_in_List[i]->setsim(simlist[i]);
    }
    
    simAll();
    simresultToFile(simlist,simnum);
    newfecnum=fecAll();
  #ifdef debug_fec
  cout<<"num of totalfec group: "<<c_AllFec.size()<<endl;
  #endif
  connectFECToGate();
  simlist.clear();
  #ifdef debug 
  cout<<"Total #FEC Group = "<<c_AllFec.size()<<endl;
  #endif
  }
  sortall();
  #ifdef debug 
  cout<<totalsimnum<<" patterns simulated.\n";
  #endif
  
}
void 
CirMgr::simAll(){
  for(size_t i=0;i<c_DFS_List.size();i++){
    c_DFS_List[i]->simulate();
    #ifdef debug_sim
    cout<<"sim gate : "<<c_DFS_List[i]->getid()<<" sim result :";
    cout<<bitset<64>(c_DFS_List[i]->getsim())<<endl;
    #endif
  }
}
bool
CirMgr::readtosize_t(ifstream& file,vector<size_t>& tosim,size_t& simnum){
  if(file.eof())return false;
  assert(tosim.size()==0);
  string line;
  size_t i=0;
  size_t s1=1;
  
  for(size_t k=0;k<c_in_List.size();k++)tosim.push_back(0);
  
  while(true){
    file>>line;
    if(line.length()!=tosim.size()){
      if(!file.eof())cerr<<"error length miss match :"<<line<<endl;
      return false;
    }
    if(file.eof()){
      simnum=i;
      break;
    }
    for(size_t j=0;j<tosim.size();j++){
      if(line[j]=='1'){
        tosim[j]|=s1<<i;
        #ifdef debug_sim
        cout<<"1";
        #endif
      }
      else if(line[j]=='0'){
        #ifdef debug_sim
        cout<<"0";
        #endif
      }
      else {cerr<<"error input non 0/1 input: "<<line<<endl;return false;}
    }
    #ifdef debug_sim
    cout<<endl;
    for(size_t i=0;i<tosim.size();i++){
    cout<<bitset<64>(tosim[i])<<endl;
    }
    #endif
    i++;
    if(i==64){
      simnum=i;
      break;
    }
  }
  return true;
}
size_t
CirMgr::fecAll(){
  size_t  newfec=0;
  vector<FecGroup*> newallfec;
  for(size_t i=0;i<c_AllFec.size();i++){
    newfec+=(c_AllFec[i]->spliteInto(newallfec));
    if(newfec>0)newfec--;
    #ifdef debug_fec
    cout<<"num of new fec group: "<<newfec<<endl;
    #endif
  }
  for(size_t i=0;i<c_AllFec.size();i++){
    delete c_AllFec[i];
  }
  c_AllFec=newallfec;
  return newfec;
}
void 
CirMgr::connectFECToGate(){
  for(size_t i=0;i<c_AllFec.size();i++){
    #ifdef debug_fec
    cout<<"connect :group "<<i<<endl;
    #endif
    c_AllFec[i]->connect();
  }
}
void
CirMgr::simresultToFile(const vector<size_t>& in ,size_t l){
  size_t stop= (l!=64)? ((size_t)1)<<l :0;
 
  #ifdef debug_sim_file
  ostream& output= (_simLog!=nullptr) ? *_simLog : cout;
  cout<<"this is debug  sim write num= "<<l<<" "<<stop<<endl;
  #endif
  #ifndef debug_sim_file
  ostream& output =*_simLog;
  if(_simLog==nullptr)return;
  #endif
  vector<size_t> out;
  
  for(size_t i=0;i<c_out_List.size();i++){
    out.push_back(c_out_List[i]->getsim());
  }
  for(size_t i=1 ;(i!=stop ) ;i<<=1){
    for(size_t j:in){
      output<<(bool)(i&j);
    }
    output<<" ";
    for(size_t j:out){
      output<<(bool)(i&j);
    }
    output<<endl;
    
  }
  #ifdef debug_sim_file
  cout<<"this is debug  sim  write end"<<endl;
  #endif
}
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
size_t
FecGroup::spliteInto(vector<FecGroup*>  & gl ){
  size_t splitto=0;
  unordered_map<size_t, FecGroup*> fecmap;
  FecGroup* tmpgr;
  for(size_t i=0;i<group.size();i++){
    size_t hashkey=group[i]->getsim();
    if(hashkey&1)hashkey=~hashkey;
    unordered_map<size_t, FecGroup*>::const_iterator iter=fecmap.find(hashkey);
    if(iter!=fecmap.end()){

      tmpgr=iter->second;
        #ifdef debug_fec
        cout<<"add to exist fec id= "<<group[i]->getid()<<endl;
        #endif
      tmpgr->addGate(group[i]);
    }else{
        #ifdef debug_fec
        cout<<"new fec id= "<<group[i]->getid()<<endl;
        #endif
      tmpgr=new FecGroup();
      tmpgr->addGate(group[i]);
      fecmap.insert(pair<size_t,FecGroup*>(hashkey,tmpgr));
    }
  }
  for(unordered_map<size_t, FecGroup*>::const_iterator iter =fecmap.begin();iter!=fecmap.end();++iter){
    if(iter->second->size()>1){
      gl.push_back(iter->second);
      splitto++;
    }
    else delete iter->second;
  }
  return splitto;
}
void
CirMgr::sortall(){
  for(FecGroup* fg:c_AllFec){
      fg->sorts();
   }
   
   sort(c_AllFec.begin(),c_AllFec.end(),[]( const FecGroup* lh, const FecGroup* rh ){
      return lh->group[0]->getid() < rh->group[0]->getid();
   });
}
void
FecGroup::connect(){
  for(size_t i=0;i<group.size();i++){
    #ifdef debug_fec
    cout<<"connect id:  "<<group[i]->getid()<<endl;
    #endif
    group[i]->setfec(this);
  }
}
string
FecGroup::toString(size_t sim,size_t id){
  stringstream ss;
  vector<size_t> invid;
  for(CirGate* g:group){
    if(g->getid()==id)continue;
    if(g->getsim()!=sim)invid.push_back(2*(g->getid())+1);
    else invid.push_back(2*(g->getid()));
  }
  sort(invid.begin(),invid.end());
  for(size_t i:invid){
    if(i%2==1)ss<<" !"<<i/2;
    else ss<<" "<<i/2;
  }
  return ss.str();
}
void
FecGroup::sorts(){
  sort(group.begin(),group.end(),[](const CirGate* lh,const CirGate * rh){
    return lh->getid() < rh->getid();
  });

  
}
FecGroup::~FecGroup(){
      for (CirGate* g:group){
        if(g!=nullptr){
          g->unsetfec();
        }
         
      }
   }
