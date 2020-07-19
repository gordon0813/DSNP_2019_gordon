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
        //cout<<"insert\n";
        bool boost=true;
        iterator ini=end();
        //BSTreeNode<T>* _save =new BSTreeNode<T>(in);//change
       
        if(empty()){ini.trace.top()->_lc= new BSTreeNode<T>(in);return ;}
        if(boost){
            insert_help(ini.trace.top()->_lc,in);
            return ;
        }
        /*
        ini.goto_n(in);
        if(ini.trace.top()->_data > in){
            //cout<<"big"<<endl;
            ini.trace.top()->_lc= _save;
            //print();
        }else{
            //cout<<"small"<<endl;
            ini.trace.top()->_rc= _save;
        }
          //print();
          */
        
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
   void pop_front() {erase(begin()); }
   void pop_back() {erase(--(end())); }
   // return false if nothing to erase
   bool erase(iterator pos) {
       cout<<"erase p\n";
       if(empty()){return false; }
        BSTreeNode<T>* now=pos.trace.top();
        pos.trace.pop();
        BSTreeNode<T>* parent=pos.trace.top();
        BSTreeNode<T>** now_in_parent;
        if(parent->_lc ==now)now_in_parent=&(parent->_lc);
        else now_in_parent=&(parent->_rc);
       if(now->_lc !=nullptr && now->_rc !=nullptr){
          delete_twoc(now_in_parent);
       }else{
          delete_onec(now_in_parent);
       }
       print();
        return true; 
        }
   void delete_onec( BSTreeNode<T>** tmp){
       cout<<"onec\n";
       BSTreeNode<T>* now=*tmp;
       BSTreeNode<T>* toadd;
       if(now->_lc !=nullptr)toadd=now->_lc;
       else if(now->_rc !=nullptr)toadd=now->_rc;
       else toadd=nullptr;
       add_a_node(tmp,toadd);
       return;
   }
   void delete_twoc(BSTreeNode<T>** now_in_parent){
       cout<<"twoc\n";
       BSTreeNode<T>* tmp=*(now_in_parent);
       BSTreeNode<T>** todelete;
       tmp=tmp->_rc;
       if(tmp->_lc==nullptr) todelete=&tmp;
       else todelete=find_lm(tmp);
       (*now_in_parent)->_data=(*todelete)->_data;
       delete_onec(todelete);

   }
   void add_a_node(BSTreeNode<T>** pos ,BSTreeNode<T>* toadd){
       cout<<"add\n";
        delete *pos;
        *pos=toadd;
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

   void clear() { }  // delete all nodes except for the dummy node

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
    BSTreeNode<T>** find_lm(BSTreeNode<T>* tmp_root){
        BSTreeNode<T>** re;
        while(tmp_root->_lc!=nullptr)re=&(tmp_root->_lc);
        return re;
    }
   BSTreeNode<T>*  _root;


};

#endif // BST_H
