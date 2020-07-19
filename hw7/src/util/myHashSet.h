/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator():hashset(nullptr),nowbucketnum(0),nowNuminBucket(0){}
      iterator(const HashSet<Data>* hs,size_t b_id,size_t id): 
      hashset((HashSet<Data>*)hs),nowbucketnum(b_id),nowNuminBucket(id){}

      const Data& operator * () const { 
         return hashset->_buckets[nowbucketnum][nowNuminBucket];
       }
      iterator& operator ++ () { 
         //cout<<"++"<<" on: "<<hashset<<endl;
         if(hashset->_buckets[nowbucketnum].size()>nowNuminBucket+1)nowNuminBucket++;
         else{
            do{
               //cout<<"num_b: "<<nowbucketnum<<endl;
               nowbucketnum++;
            }while(hashset->_buckets[nowbucketnum].size()==0 && nowbucketnum!=hashset->_numBuckets);
            nowNuminBucket=0;
         }
         return (*this); 
      }
      bool operator != (const iterator& i) const {
          if(hashset==i.hashset && nowbucketnum ==i.nowbucketnum && nowNuminBucket==i.nowNuminBucket) return false;
          else return true;
          }
   private:
      HashSet<Data>* hashset;
      size_t nowbucketnum;
      size_t nowNuminBucket;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      iterator it(this,0,0);
      //cout<<"begin"<<endl;
      if(this->_buckets[0].size()!=0){
          //cout<<"begin1"<<endl;
         return it;
      }else{
          //cout<<"begin2"<<endl;
         ++it;
         return it;
      }
   }
   // Pass the end
   iterator end() const { return iterator(this,this->_numBuckets,0); }
   // return true if no valid data
   bool empty() const { 
      for (size_t s=0 ;s< this->_numBuckets;s++){
         if(_buckets[s].size())return 0;
      }
   }
   // number of valid data
   size_t size() const { 
   size_t s = 0; 
      for (size_t i=0 ;i< this->_numBuckets;i++){
         s+=_buckets[s].size();
      } 
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      size_t id;
      size_t b_id;
      if(getid(d,b_id,id))return true;
      else return false; 
    }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      size_t id;
      size_t b_id;
      if(getid(d,b_id,id)){
         d=_buckets[b_id][id];
         return true;
      }
      else return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      size_t id;
      size_t b_id;
      if(getid(d,b_id,id)){
         _buckets[b_id][id]=d;
         return true;
      }else{
         _buckets[b_id].push_back(d);
         return false;
      }
    }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      
      size_t id;
      size_t b_id;
      if(getid(d,b_id,id)){
         return false;
      }else{
         _buckets[b_id].push_back(d);
         //cout<<"insert : "<<d<<" on: "<<b_id<<"size: "<<this->_buckets[b_id].size()<<endl;
         return true;
      }
       
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      size_t id;
      size_t b_id;
      if(getid(d,b_id,id)){
         //_buckets[b_id].erase(_buckets[b_id].begin()+id);
         _buckets[b_id][id]=_buckets[b_id][_buckets[b_id].size()-1];
         _buckets[b_id].pop_back();
         return true;
      }else{
         
         //cout<<"insert : "<<d<<" on: "<<b_id<<"size: "<<this->_buckets[b_id].size()<<endl;
         return false;
      }
    }

private:
   bool getid  (const Data & d,size_t & b_id , size_t & id )const{
      b_id= bucketNum( d );
      vector<Data> & bucket=this->_buckets[b_id];
      for (size_t s =0;s<bucket.size();s++){
         if(bucket[s]==d){
            id=s;
            return true;
         }
      } 
      id=0;
      return false;
   }
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
