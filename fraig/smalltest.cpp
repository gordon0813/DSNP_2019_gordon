#include <iostream>
using namespace std;
class A{
    public :
    int a;
};
int main (){
    A *a1=new A();
    a1->a=123;
    cout<<a1<<endl;
    cout<<a1->a<<endl;
    a1=(A*)(((size_t)a1)+1);
    cout<<a1<<endl;
    cout<<a1->a<<endl;
   // a1=(A*)(((size_t)a1)-1);
    a1 =(A*)((size_t)a1 & ~(1UL << 0));
    cout<<a1<<endl;
    cout<<a1->a<<endl;

}