/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   j.first_time=false;
   char temp[300];
   char tostring[300];
   int tosL=0;
   string str;
   do{
      is.getline(temp,300);
      if(temp[0]=='{')continue;
      else if(temp[0]=='}')break;
      else{
         tosL = 0;
         for (unsigned int i=0;temp[i]!=char(0);i++){
           if(temp[i]!=' ' && temp[i]!='\t'){
              tostring[tosL]=temp[i];
              tosL++;
           }
         }
         tostring[tosL]=char(0);
         str=string(tostring);
        
         size_t ids=str.find_first_of(':');
         if(ids!=string::npos){
            string s1=str.substr(0,ids);
            j._obj.push_back(DBJsonElem(s1.substr(1,s1.length()-2),stoi(str.substr(ids+1,str.length()))));
            //cout<<j._obj[j._obj.size()-1]<<endl;//mustremove
         }
        //cout<<"read 1 line"<<endl;//mustremove
         
      }
   }while(!is.eof());
   //cout<<"read file: objL:"<<j._obj.size()<<endl;
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   int size=j._obj.size();
   //cout<<"in operator in dbjson cpp"<<endl;//mustremove
   os<<"{"<<endl;
   for(int i=0;i<size-1;i++){
      os<<"  "<<j._obj[i]<<","<<endl;
   }
   if(size!=0)os<<"  "<<j._obj[size-1]<<endl;
   os<<"}"<<endl;
   os<<"Total JSON elements: "<<size<<endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
const DBJsonElem* 
DBJson::find(const string& str){
    for (unsigned int i=0;i<_obj.size();i++){
        if(_obj[i].key().compare(str)==0){
          // cout<<"{ "<<dbjson[i]<<" }"<<endl;
           return &_obj[i];
        }
        
    }
    return nullptr;
      
};
void
DBJson::reset()
{
   
   _obj.clear();
   //cout<<"in reset size:"<<_obj.size()<<endl;//mustremove
   // TODO
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   if(this->find(elm.key())!=nullptr){
      //cout<<"add : false"<<endl;//mustremove
      return false;
   }else{
      //cout<<"add : true "<<elm<<endl;//mustremove
      this->_obj.push_back(elm);
      return true;
   }
   
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   float re=0;
   unsigned int size=_obj.size();
   if(size==0){ return NAN;}
   for (unsigned int i=0;i<size;i++){
      re+=_obj[i].value();
   }
   return re/size;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   for(size_t i=0;i<_obj.size();i++){
      if(_obj[i].value()>maxN){
         idx=i;
         maxN=_obj[i].value();
      }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   for(size_t i=0;i<_obj.size();i++){
      if(_obj[i].value()<minN){
         idx=i;
         minN=_obj[i].value();
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   size_t my_size=_obj.size();
   for (size_t i=0;i<my_size;i++){
      s+=_obj[i].value();
   }
   return s;
}
