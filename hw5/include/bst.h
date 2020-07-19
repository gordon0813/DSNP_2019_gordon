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
  
   BSTreeNode(const T& d, BSTreeNode<T>* l = nullptr, BSTreeNode<T>* r = nullptr):
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
        iterator(const iterator& it){
            trace=it.trace;
        }
     // iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return trace.top()->data; }
      T& operator * () { return trace.top()->_data ; }
      iterator& operator ++ () {
          //cout<<"++()\n";
          if(go_right()){
              while(go_left());
          }else{
              go_back_right();
          }
          return *(this);
    }//ok
      iterator operator ++ (int) {
         // cout<<"()++\n";
          iterator tmp(*this);
          operator++();
          return tmp; }
      iterator& operator -- () {
          if(go_left()){
              while(go_right());
          }else{
              go_back_left();
          }
          return *(this);
          
          }
      iterator operator -- (int) {
           iterator tmp(*this);
           operator--();
          return tmp; 
          }

      iterator& operator = (const iterator& i) {return *(this); }

      bool operator != (const iterator& i) const { return  (trace.top()!=i.trace.top()); }
      bool operator == (const iterator& i) const { return (trace.top()==i.trace.top()); }//ok
    private:
    bool goto_n(const T& d){
        //cout<<"goto_n\n";
        go_left();
        while(true){
            if(trace.top()->_data==d){
                return true;
            }else if(trace.top()->_data>d){
                if(!go_left())return false;
            }else{
                if(!go_right())return false;
            }
        }//ok
    }
    bool go_right(){
        //cout<<"go_rigth\n";
        // cout<<trace.top()<<endl;
        if (trace.top()->_rc != nullptr){
            trace.push(trace.top()->_rc);
            return true;
        }else return false;
    }
    bool go_left(){
        // cout<<"go_left\n";
         //cout<<trace.top()<<endl;
        if(trace.top()->_lc!=nullptr){
            trace.push(trace.top()->_lc);
            return true;
        }else{return false;}
    }
    void go_back_right(){
        //cout<<"go_bach_r\n";
        BSTreeNode<T>* tmp=trace.top();
        trace.pop();
       while(trace.top()->_rc==tmp){
           tmp=trace.top();
           trace.pop();
       }
    }
    void go_back_left(){
      // while(trace.top()->_lc==(trace.pop()));
       BSTreeNode<T>* tmp=trace.top();
        trace.pop();
       while(trace.top()->_lc==tmp){
           tmp=trace.top();
           trace.pop();
           if(trace.empty())break;
       }
    }
    stack<BSTreeNode<T>*> trace;

 };
    void insert(T in){
        if(empty()){_root->_lc= new BSTreeNode<T>(in);return ;}
        insert_help(_root->_lc,in);
        return ;
    }//ok

   iterator begin() const {
       iterator be=end();
       while(be.go_left());//cout<<"times\n";
       return be; 
    }//ok
   iterator end() const {return iterator(_root); }//ok
   bool empty() const { return _root->_lc==nullptr; }
   size_t size() const {  return count_sub_size(_root->_lc); }
   size_t count_sub_size (BSTreeNode<T>* n )const{
       if(n==nullptr)return 0;
       else return count_sub_size(n->_lc)+count_sub_size(n->_rc)+1;
   } 
   //void push_back(const T& x) { }
   void pop_front() {
       BSTreeNode<T>* parent=_root;
       if(parent->_lc==nullptr)return;
       while(parent->_lc->_lc!=nullptr)parent=parent->_lc;
        add_a_node(parent,parent->_lc,delete_one_node(parent->_lc)) ;
       }
   void pop_back() {
       BSTreeNode<T>* parent=_root;
       if(parent->_lc==nullptr)return;
       parent=_root->_lc;
       if(parent->_rc==nullptr){
           add_a_node(_root,parent,delete_one_node(parent)) ;
           return ;
       }
       while(parent->_rc->_rc!=nullptr)parent=parent->_rc;
        add_a_node(parent,parent->_rc,delete_one_node(parent->_rc)) ;
    }

   // return false if nothing to erase
   bool erase(iterator pos) {
       if(empty()){return false; }
       if(pos.trace.top()->_lc !=nullptr && pos.trace.top()->_rc !=nullptr){
            delete_twoc(pos); 
       }else{
           delete_onec(pos);//have rigth child
       }
        return true; 
        }
   void delete_onec(iterator& pos){
       BSTreeNode<T>* tmp =pos.trace.top();
       pos.trace.pop();
       BSTreeNode<T> *parent=pos.trace.top();
       BSTreeNode<T>* toadd ;
       toadd =delete_one_node(tmp);
       add_a_node(parent,tmp,toadd);
       return;
   }
   BSTreeNode<T>* delete_one_node(BSTreeNode<T>* n){
       BSTreeNode<T>* toadd;
       if(n->_lc !=nullptr)toadd=n->_lc;
       else if(n->_rc !=nullptr)toadd=n->_rc;
       else toadd=nullptr;
       delete n;
       return toadd;
   }
   void delete_twoc(iterator& pos){
       BSTreeNode<T>* tmp =pos.trace.top();
       ++pos;
       tmp->_data=*pos;
       delete_onec(pos);

   }
   void add_a_node(BSTreeNode<T>* parent,BSTreeNode<T>* tmp,BSTreeNode<T>* toadd){
        if(parent->_lc==tmp)parent->_lc=toadd;
       else parent->_rc=toadd;
   }
   bool erase(const T& x) { 
       iterator it =find(x);
       if(it!=end()){
           erase(it);
           return true;
       }
       return false; 
       }
   void  print()const {
       cout<<"print:"<<endl;
        help_print(_root,0);
    return ;
   }//ok
   void help_print(BSTreeNode<T> *n,int i)const{
       for(int j=0;j<i;j++)cout<<"      ";
       if(n==nullptr){cout<<"["<<i<<"]"<<endl;return;}
       cout<<"["<<i<<"] "<<n<< " "<<n->_data<<endl;
       help_print(n->_lc,i+1);
       help_print(n->_rc,i+1);
   }//ok
   iterator find(const T& x) { 
       iterator it=end();
       it.goto_n(x);
       if(*it==x)return it;
        else return end();
    }

   void clear() { 
       delete_help(_root->_lc);
       _root->_lc=nullptr;
   }  // delete all nodes except for the dummy node

    void delete_help(BSTreeNode<T>* n){
        if(n==nullptr) return;
        delete_help(n->_lc);
        delete_help(n->_rc);
        delete n;
    }
   void sort() const { }
   private:
        void insert_help(BSTreeNode<T>* n,T in){
        BSTreeNode<T>* tmp=n;
        while(true){
            if(tmp->_data>in){
                if(tmp->_lc==nullptr){
                    tmp->_lc=new BSTreeNode<T>(in);
                    return;
                }else tmp=tmp->_lc;
            }else{
                if(tmp->_rc==nullptr){
                    tmp->_rc=new BSTreeNode<T>(in);
                    return;
                }else tmp=tmp->_rc;
            }
        }
        }
       BSTreeNode<T>* find_help(T in){
            BSTreeNode<T>* tmp=_root->_lc;
        while(true){
            if(tmp->_data>in){
                if(tmp->_lc==nullptr){
                    return nullptr;
                }else tmp=tmp->_lc;
            }else if(tmp->_data < in){
                if(tmp->_rc==nullptr){
                    return nullptr;
                }else tmp=tmp->_rc;
            }else return tmp;
       }
    }
   BSTreeNode<T>*  _root;


};

#endif // BST_H
