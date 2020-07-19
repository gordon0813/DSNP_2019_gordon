#include <iostream>
using namespace std;
class A{
    
    int a;
    int b;
    int c;
    int d;
    int e;
    public:
    A(){
    }
    ~A() {}
    A(int a1,int a2){
        a=a1;
        b=a2;
    }

};
int main (){
    A* a1=new A(1,2);
    A* a2=new A(3,4);
    A* a3=0;
    A* a4 = new A[20];
   
    cout<<*((size_t*)(void*)a4-1)<<endl;
/*
    
    
    *(size_t*)(void*)a1=(size_t)a2;
   // cout<<sizeof((size_t*)(void*)a1)<<endl;

    cout<<(  (A*)  *(size_t*)(void*)a1  )->a<<endl;
    cout<<(  (A*)  *(size_t*)(void*)a1  )<<" : "<<a2<<endl;
    *(size_t*)(void*)a1=(size_t)a3;
    cout<< ((A*)  *(size_t*)(void*)a1 ==nullptr)<<endl;
    delete a1;
    cout<<"finish"<<endl;
    */

}