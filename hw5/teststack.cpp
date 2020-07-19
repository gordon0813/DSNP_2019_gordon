#include<iostream>
#include<string>
#include<stack>
using namespace std;
int main (){
    stack<string *> s;
    string s4=string("dsds");
    string s5=string("dsds");
    s.push(&s4);
    s.push(&s5);
    string s1="asd";
    string s2="";
    string s3=string();
    cout<<(s2<s1)<<" "<<(s3>s2)<<" "<<*s.top()<<endl;

}