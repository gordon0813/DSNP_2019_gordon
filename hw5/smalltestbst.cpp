/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <stack>
using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
    friend class BSTree<T>;
   friend class BSTree<T>::iterator;
  
   BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
      _data(d), _lc(l), _rc(r) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
    BSTreeNode<T>*  _lc;
   BSTreeNode<T>*  _rc;
};


template <class T>
class BSTree
{
    public:
     BSTree(){
        _root=new BSTreeNode<T>(T());    
     }
   // TODO: design your own class!!
   class iterator {
     friend class BSTree<T>;
      public:
        iterator(BSTreeNode<T>* root= 0){
            trace.push(root);//if(!goto_n(n->_data))cerr<<"not found";
        }
     // iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return trace.top()->data; }
      T& operator * () { return trace.top()->_data ; }
      iterator& operator ++ () {return *(this); }
      iterator operator ++ (int) {return *(this); }
      iterator& operator -- () {return *(this);}
      iterator operator -- (int) {return *(this); }

      iterator& operator = (const iterator& i) {return *(this); }

      bool operator != (const iterator& i) const { return false; }
      bool operator == (const iterator& i) const { return false; }
    private:
    bool goto_n(const T& d){
        while(true){
            if(trace.top()->_data==d){
                return true;
            }else if(trace.top()->_data>d){
                if(!go_left())return false;
            }else{
                if(!go_right())return false;
            }
        }
    }
    bool go_right(){
        if (trace.top()->_rc != nullptr){
            trace.push(trace.top()->_rc);
            return true;
        }else return false;
    }
    bool go_left(){
        if(trace.top()->_lc!=nullptr){
            trace.push(trace.top()->_lc);
            return true;
        }else{return false;}
    }
    void go_back(){
       trace.pop();
       return ;
    }
    
    stack<BSTreeNode<T>*> trace;

 };
    void insert(T in){
        iterator ini=end();
        ini.goto_n(in);
        if()
        ini.trace.top()->
        
    }
   iterator begin() const {
       iterator begin(_root);
       while(begin.go_left());
       return begin; 
    }
   iterator end() const {return iterator(_root); }
   bool empty() const { return false; }
   size_t size() const {  return 0; }

   void push_back(const T& x) { }
   void pop_front() { }
   void pop_back() { }

   // return false if nothing to erase
   bool erase(iterator pos) { return false; }
   bool erase(const T& x) { return false; }
   void  print()const {return ;}
   iterator find(const T& x) { return end(); }

   void clear() { }  // delete all nodes except for the dummy node

   void sort() const { }
   private:
   BSTreeNode<T>& leftmost(BSTreeNode<T>* now){
       BSTreeNode<T>* _find=now;
       while(_find->_lc!=nullptr){
           _find=_find->_lc;
       }
       return _find;
   }
   BSTreeNode<T>& rightmost(BSTreeNode<T>* now){
       BSTreeNode<T>* _find=now;
       while(_find->_rc!=nullptr){
           _find=_find->_rc;
       }
       return _find;
   }
   BSTreeNode<T>*  _root;


};

#endif // BST_H
