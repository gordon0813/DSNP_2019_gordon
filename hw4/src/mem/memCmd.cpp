/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else {
      mtest.reset();
   }

   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   unsigned int s=options.size();
   int num=0;
   int num2=0;
   if(!myStrNCmp("-Array",options[0],2) && s==3){
      //todo
      if(!myStr2Int(options[1],num) || num<=0)return  CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      if(!myStr2Int(options[2],num2) || num2<=0)return  CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      try {
               mtest.newArrs(num2,num);//array
               return CMD_EXEC_DONE;
            }catch (bad_alloc err){
               //cerr<<"bad_alloc"<<endl;
               //return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
               return CMD_EXEC_ERROR;
         }
       //return CMD_EXEC_DONE;
   }
   if(!myStr2Int(options[0],num)  || num<=0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   if(s==1){
       mtest.newObjs(num);  //object
      return CMD_EXEC_DONE;
   }
   if(s==3){
      if(!myStrNCmp("-Array",options[1],2) ){
         if(!myStr2Int(options[2],num2) || num2<=0){
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }else{
            try {
               mtest.newArrs(num,num2);//array
               return CMD_EXEC_DONE;
            }catch (bad_alloc err){
               //cerr<<"bad_alloc"<<endl;
               //return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
               return CMD_EXEC_ERROR;
            }
         }
      }else{
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      }
   }else if(s==2){
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   }else if(s>=4){
      return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
   }
   
   // Use try-catch to catch the bad_alloc exception
   //cerr<<"something wrong"<<endl;
   return CMD_EXEC_ERROR;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
 
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   unsigned int s=options.size();
   if(s==1)return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);//error
   if(s>3)return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
   int num=0;
   if(!myStrNCmp("-Index",options[0],2)){          //        -index
      if(myStr2Int(options[1],num)){
         if(s==3){// -i -a
            if(!myStrNCmp("-Array",options[2],2)){
               if((size_t)num>=mtest.getArrListSize()){
                  cerr<<"Size of Array list ("<<mtest.getArrListSize()<<") is <= "<<num<<endl;
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
               }
               mtest.deleteArr(num); 
               return CMD_EXEC_DONE;
            }
            else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }else{//  -i     obj
            if((size_t)num>=mtest.getObjListSize()){
               cerr<<"Size of object list ("<<mtest.getObjListSize()<<") is <= "<<num<<endl;
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
            }
             mtest.deleteObj(num); 
             return CMD_EXEC_DONE; 
         }
      }else return  CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
   }else if(!myStrNCmp("-Random",options[0],2)){        // -rand
      if(myStr2Int(options[1],num)){
         if(s==3){                        // -r -a
            if(!myStrNCmp("-Array",options[2],2)){
               if(mtest.getArrListSize()==0){
                  cerr<<"Size of array list is 0!!"<<endl;
                  return CMD_EXEC_ERROR;
               }
               for (int i=0;i<num;i++){
                   mtest.deleteArr(rnGen(mtest.getArrListSize()));
               }
               return CMD_EXEC_DONE;
               }
            else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }else{                        //-r  obj
           if(mtest.getObjListSize()==0){
                  cerr<<"Size of object list is 0!!"<<endl;
                  return CMD_EXEC_ERROR;
               }
            for (int i=0;i<num;i++){
               mtest.deleteObj(rnGen(mtest.getObjListSize()));
            }
            return CMD_EXEC_DONE;
         }
      }else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
   }else if(!myStrNCmp("-Array",options[0],2)){                    //-array
      if(s<3)return CmdExec::errorOption(CMD_OPT_MISSING, "");
      else if(s>3)return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
      else{
         if(!myStrNCmp("-Index",options[1],2)){  //-a -i
            if(myStr2Int(options[2],num)){
               //
               if((size_t)num>=mtest.getArrListSize()){
                  cerr<<"Size of Array list ("<<mtest.getArrListSize()<<") is <= "<<num<<endl;
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
               }
               mtest.deleteArr(num); 
               return CMD_EXEC_DONE;

            }else return  CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }else if(!myStrNCmp("-Random",options[1],2)){  // -r   -i
            if(myStr2Int(options[2],num)){
               //
               if(mtest.getArrListSize()==0){
                  cerr<<"Size of array list is 0!!"<<endl;
                  return CMD_EXEC_ERROR;
               }
               for (int i=0;i<num;i++){
                   mtest.deleteArr(rnGen(mtest.getArrListSize()));
               }
               return CMD_EXEC_DONE;
            }else return  CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      }
      
   }else{
      //error
      return  CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   }
   //cerr<<"something wrong"<<endl;
   return CMD_EXEC_ERROR;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


