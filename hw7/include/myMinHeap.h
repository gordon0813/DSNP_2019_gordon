/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      //cout<<"insert: "<<d<<endl;
      size_t cid;
      size_t pid;
      _data.push_back(d);
      cid=_data.size()-1;
      while(swapup(cid));
      return;
    }
   void delMin() {
      size_t nowid=0;
      _data[0]=_data.back();
      _data.pop_back();
      while(swapdown(nowid));
    }
   void delData(size_t i) {
      size_t nowid=i; 
      _data[nowid]=_data.back();
      _data.pop_back();
      while(swapdown(nowid));
      nowid=i;
      while(swapup(nowid));
   }
   void assign(size_t l){
      size_t nowid=0;
      _data[0]+=l;
      while(swapdown(nowid));
   }

private:
   size_t getparent(size_t child){
      return (child-1)/2;
   }
   void swap(Data & p,Data & c){
      Data tmp=p;
      p=c;
      c=tmp;
   }
   bool getminchild_id(const size_t pid,size_t& cid){
      size_t rc =(pid+1)*2;
      size_t lc = rc-1;
      
      if(lc>=_data.size() ){
         return false;
      }
      else if(lc ==_data.size()-1){
         cid=lc;
         return true;
      }
      else if(_data[rc]<_data[lc]){
         cid=rc;
         return true;
      }
      else {
         cid=lc;
         return true;
      }
   }
   bool swapup(size_t& nowid){
      //cout<<"swap_up "<<nowid<<endl;
      size_t pid;
      if(nowid==0)return false;
      pid=getparent(nowid);
      if(_data[nowid]<_data[pid]){
         swap(_data[pid],_data[nowid]);
         nowid=pid;
         return true;
      }else{
         return false;
      }
   }
   bool swapdown(size_t& nowid){
      //cout<<"swapdown: "<<nowid<<endl;
      size_t cid;
      if(getminchild_id(nowid,cid)){
         if(_data[cid]<_data[nowid]){
            swap(_data[nowid],_data[cid]);
            nowid=cid;
            return true;
         }else{
            return false;
         }
      }else{
         return false;
      }   
   }
   // DO NOT add or change data members

   vector<Data>  _data;
};

#endif // MY_MIN_HEAP_H
