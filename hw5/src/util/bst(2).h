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
#include <vector>
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
      const T& operator * () const { return trace.back()->data; }
      T& operator * () { return trace.back()->_data ; }
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

      bool operator != (const iterator& i) const { return  (trace.back()!=i.trace.back()); }
      bool operator == (const iterator& i) const { return (trace.back()==i.trace.back()); }//ok
    private:
    bool goto_n(const T& d){
        //cout<<"goto_n\n";
        go_left();
        while(true){
            if(trace.back()->_data==d){
                return true;
            }else if(trace.back()->_data>d){
                if(!go_left())return false;
            }else{
                if(!go_right())return false;
            }
        }//ok
    }
    bool go_right(){
        //cout<<"go_rigth\n";
        // cout<<trace.back()<<endl;
        if (trace.back()->_rc != nullptr){
            trace.push(trace.back()->_rc);
            return true;
        }else return false;
    }
    bool go_left(){
        // cout<<"go_left\n";
         //cout<<trace.back()<<endl;
        if(trace.back()->_lc!=nullptr){
            trace.push(trace.back()->_lc);
            return true;
        }else{return false;}
    }
    void go_back_right(){
        //cout<<"go_bach_r\n";
        BSTreeNode<T>* tmp=trace.back();
        trace.pop_back();
       while(trace.back()->_rc==tmp){
           tmp=trace.back();
           trace.pop_back();
       }
    }
    void go_back_left(){
      // while(trace.back()->_lc==(trace.pop_back()));
       BSTreeNode<T>* tmp=trace.back();
        trace.pop_back();
       while(trace.back()->_lc==tmp){
           tmp=trace.back();
           trace.pop_back();
           
           if(trace.empty())break;
       }
    }
    vector<BSTreeNode<T>*> trace(10);

 };
    void insert(T in){
        //cout<<"insert\n";
       
       
        //BSTreeNode<T>* _save =new BSTreeNode<T>(in);//change
       
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
   void pop_back_front() {erase(begin()); }
   void pop_back_back() {erase(--(end())); }

   // return false if nothing to erase
   bool erase(iterator pos) {
       if(empty()){return false; }
       if(pos.trace.back()->_lc !=nullptr && pos.trace.back()->_rc !=nullptr){
            delete_twoc(pos); 
       }else{
           delete_onec(pos);//have rigth child
       }
        return true; 
        }
   void delete_onec(iterator& pos){
       BSTreeNode<T>* tmp =pos.trace.back();
       BSTreeNode<T>* toadd;
       if(tmp->_lc !=nullptr)toadd=tmp->_lc;
       else if(tmp->_rc !=nullptr)toadd=tmp->_rc;
       else toadd=nullptr;
       pos.trace.pop_back();
       add_a_node(pos,toadd,tmp);
       return;
   }
   void delete_twoc(iterator& pos){
       BSTreeNode<T>* tmp =pos.trace.back();
       ++pos;
       tmp->_data=*pos;
       delete_onec(pos);

   }
   void add_a_node(iterator& pos,BSTreeNode<T>* toadd,BSTreeNode<T>* todelete){
        if(pos.trace.back()->_lc==todelete)pos.trace.back()->_lc=toadd;
       else pos.trace.back()->_rc=toadd;
       delete todelete;
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
   BSTreeNode<T>*  _root;


};

#endif // BST_H
