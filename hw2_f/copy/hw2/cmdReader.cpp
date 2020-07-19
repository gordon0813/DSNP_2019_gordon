/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include<string>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();
   this->_history.push_back("");
   this->_tempCmdStored=true;
   
   while (1) {
      
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;//ctrl h
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : if(moveBufPtr(_readBufPtr-1)){deleteChar();}/* TODO */ break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: this->moveBufPtr(this->_readBufPtr+1);/* TODO */ break;
         case ARROW_LEFT_KEY :this->moveBufPtr(this->_readBufPtr-1);/* TODO */break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : if(_readBufPtr==_readBufEnd){
               insertChar(' ',3);
         }else{
               insertChar(' ',8-((_readBufPtr-_readBuf)/sizeof(char))%8);
         }
         /* TODO */ break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   if(ptr>this->_readBufEnd ||ptr<this->_readBuf){
      mybeep();
      return false;
   }
   else{
      if(ptr<this->_readBufPtr){
         for (unsigned int i=0;i<(_readBufPtr-ptr)/sizeof(char);i++){//
            cout<<'\b';
         }
      }else if(ptr>this->_readBufPtr){
         for(unsigned int i=0;i<(ptr-_readBufPtr)/sizeof(char);i++){
            cout<<*(_readBufPtr+i);
         }
      }
      this->_readBufPtr=ptr;
      return true;
   }  
   // TODO...
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   if(this->_readBufPtr!=this->_readBufEnd){
      int count=0;
      for(char* ptr=this->_readBufPtr;ptr<this->_readBufEnd;ptr++){
         count++;
         *ptr=*(ptr+1);
         cout<<*ptr;
      }
      cout<<char(0);
      _readBufEnd-=1;
      for (int i=0;i<=count;i++){
         cout<<'\b';
      }
      return true;
   }else{
      mybeep();
      return false;
   }

}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
// finish
void
CmdParser::insertChar(char ch, int repeat)
{
   int backTimes;
   for (int i=0;i<repeat;i++){
      backTimes=0;
      //char array part
   //append one space
     this->_readBufEnd++;
   //move string from _readBufPtr to_readBufEnd in char array right
     for(char* ptr=this->_readBufEnd;ptr>=this->_readBufPtr;ptr--){
        *(ptr)=*(ptr-1);
     }
   *(this->_readBufPtr)=ch;
     //screen part
     for(char* ptr=this->_readBufPtr;ptr<this->_readBufEnd;ptr++){
        cout<<*ptr;
        backTimes++;
     }
     for(int i=0;i<backTimes-1;i++){
        cout<<'\b';
     }
   this->_readBufPtr++;
   }
   assert(repeat >= 1);
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   for(unsigned int i=0;i<(_readBufPtr-_readBuf)/sizeof(char);i++){
      cout<<'\b';
   }
   for(char* ptr=_readBuf;ptr<_readBufEnd;ptr++){
      *ptr='\0';
      cout<<char(0);
   }
   for(unsigned int i=0;i<(_readBufEnd-_readBuf)/sizeof(char);i++){
      cout<<'\b';
   }
   this->_readBufPtr=this->_readBuf;
   this->_readBufEnd=this->_readBuf;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//no yet 
void
CmdParser::moveToHistory(int index)
{
  // cout<<'\n'<<" i-"<<_historyIdx<<" s-"<<_history.size()<<" tf "<<_tempCmdStored<<'\n';
   
   if (index < 0)
   { //||index>=int(_history.size())
      if (_historyIdx == 0)
      {
         mybeep(); return;
      }else{
         index=0;
      }
      //mybeep(); return;
   }
   else if (index >= int(_history.size()))
   {
      if(_historyIdx==int(_history.size()-1)){
         mybeep(); return;
      }else{
         index=int(_history.size()-1);
      }
   }
   if(this->_tempCmdStored==false){
       this->_tempCmdStored=true;
       this->_history[_history.size()-1]=string(this->_readBuf);
   }
 
   this->_historyIdx=index;
   deleteLine();
   if(index==int(_history.size())-1){
      this->_tempCmdStored=false;
   }

   string& tmp=this->_history[_historyIdx];
   for(unsigned int i=0;i<tmp.size();i++){
      *(this->_readBuf+i)=tmp[i];
   }

   _readBufEnd=_readBuf+tmp.size();
   _readBufPtr=_readBufEnd;
   cout<<tmp;
   // TODO...
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   if(*_readBuf!=0){
      string temps;
      temps=string(this->_readBuf);
      temps.erase(0,temps.find_first_not_of(' '));
      temps.erase(temps.find_last_not_of(' ')+1);
     
      this->_history[_history.size()-1]=temps;
      this->_history.push_back("");
   
      
      for(char* ptr=_readBuf;ptr<_readBufEnd;ptr++){
         *ptr='\0';
      }
      this->_historyIdx=this->_history.size()-1;
      this->_tempCmdStored=false;
   }
   // TODO...
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
