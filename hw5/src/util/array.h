/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node+=1; return (*this); }
      iterator operator ++ (int) {iterator t=iterator(*this);   _node+=1;   return t; }
      iterator& operator -- () { _node-=1; return (*this); }
      iterator operator -- (int) { iterator t=iterator(*this);   _node-=1 ;  return t; }

      iterator operator + (int i) const { iterator t=iterator(*this);   t._node+=i;  return t;}
      iterator& operator += (int i) {_node+=i;    return (*this); }
      //ok

      iterator& operator = (const iterator& i) {this->_node = i._node; return (*this); }

      bool operator != (const iterator& i) const { return this->_node!= i._node;}
      bool operator == (const iterator& i) const { return this->_node== i._node;}
    //ok
   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(this->_data); }//ok
   iterator end() const { return iterator(this->_data+_size); }//ok
   bool empty() const { return _size==0; }//ok
   size_t size() const { return _size; }//ok

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      //cout<<"push_back: "<<x<<endl;
      if(_capacity==_size){
        // cout<<"push new arr\n";
        
         if(_capacity==0){
            _capacity=1;
         }else{
            _capacity=_capacity*2;
         }
         T* newarr=new T[_capacity];
         for(size_t i=0;i<_size;i++){
            newarr[i]=*(_data+i);
         }
         newarr[_size]=x;
         _size++;
         delete[] _data;
         _data=newarr;
      }else{
         *(_data+_size)=x;
         _size++;
      }//ok

   }
   void pop_front() {
       if(_size!=0){
           *_data=*(_data+_size-1);
           _size--;
       }
    }
   void pop_back() {
       if(_size!=0)_size--;
    }//ok

   bool erase(iterator pos) { 
      //cout<<"erase pos"<<pos._node<<" _data"<<_data<<" sub "<<(pos._node-_data)<<"size t "<<sizeof(T)<<endl;
      if(_size==0)return false;
      //size_t idx=(pos._node-_data);
      *pos=_data[_size-1];
      _size--;
      return true; 
    }
   bool erase(const T& x) { 
       //cout<<"erase  : "<<x<<endl;
       iterator fi=find(x);
       if(fi==end())return false;
       else erase(fi);
       return true; 
       }

   iterator find(const T& x) { 
       //cout<<"find : "<<x<<endl;
       for (size_t i=0;i<_size;i++){
           //cout<<"find::now : "<<_data[i]<<endl;
           if(_data[i]==x){
            //cout<<"find::get : "<<_data[i]<<endl;
               return iterator(_data+i);
           }
       }
       //cout<<"find::fail"<<endl;
       return end(); 
    }

   void clear() { 
       _size=0;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
