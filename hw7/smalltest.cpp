#include <iostream>
using namespace std;
class A{
    public:
    A(int i){
        ii=i;
    }
    int ii;
};

int main(void){
    A a1(2);
    A & a2 =a1;
    
    cout<<&a1<<" "<<&a2<<endl;


}