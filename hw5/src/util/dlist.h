/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {this->_node=_node->_next; return *(this); }
      iterator operator ++ (int) {iterator t=iterator(*this); this->_node=_node->_next; return t; }
      iterator& operator -- () {this->_node=_node->_prev; return *(this); }
      iterator operator -- (int) { iterator t=iterator(*this); this->_node=_node->_prev; return t; }

      iterator& operator = (const iterator& i) {this->_node=i._node; return *(this); }

      bool operator != (const iterator& i) const { return (this->_node!=i._node); }
      bool operator == (const iterator& i) const { return (this->_node==i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const { return (_head->_next==_head); }
   size_t size() const {  
      DListNode<T>* tmp=_head;
      size_t count=0;
      while (tmp->_next!=_head){
         count++;
         tmp=tmp->_next;
      }
      return count; 
   }

   void push_back(const T& x) { 
      DListNode<T>* tmp=new DListNode<T>(x,_head->_prev,_head);
      _head->_prev->_next=tmp;
      _head->_prev=tmp;
   }
   void pop_front() {
      if(empty()) return;
      _head->_next=_head->_next->_next;
      delete _head->_next->_prev;
      _head->_next->_prev=_head;
   }
   void pop_back() { 
       if(empty()) return;
       _head->_prev=_head->_prev->_prev;
       delete _head->_prev->_next;
       _head->_prev->_next=_head;
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if(empty()){return false; }
      DListNode<T>* tmp=pos._node;
      tmp->_prev->_next=tmp->_next;
      tmp->_next->_prev=tmp->_prev;
      delete tmp;
      return true;
   }
   bool erase(const T& x) { 
      iterator it=find(x);
      if(it==end())return false; 
      erase(it);
      return true;
   }

   iterator find(const T& x) { 
      iterator it=begin();
         while(it!=end()){
            if(*it==x)return it;
            ++it;
         }
      return end(); 
      }

   void clear() { 
      if(empty())return;
      DListNode<T>* tmp=_head->_next->_next;
      while(tmp!=_head){
         delete tmp->_prev;
         tmp=tmp->_next;
      }
      delete _head->_prev;
      _head->_next=_head;
      _head->_prev=_head;
   }  // delete all nodes except for the dummy node

   void sort() const {
         bool isSorted=false;
         iterator itf;
         iterator its;
         T tmp;
         while(isSorted==false){
            isSorted=true;
            itf=begin();
            its=begin();
            ++itf;
            while (itf!=end()){
               if(*itf<*its){
                  //cout<<"sort: "<<*itf << *its <<endl;
                  tmp=*itf;
                  *itf=*its;
                  *its=tmp;
                  isSorted=false; 
               }
               ++itf;
               ++its;
            }
         }
    }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
