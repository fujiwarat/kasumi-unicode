#include "KasumiString.hxx"
#include <iostream>
#include <sstream>

using namespace std;

bool KasumiString::isCommentLine(){
  // comment line is commented out by "#"
  if(find("#",0) == 0){
    return true;
  }

  return false;
}

bool KasumiString::isEmptyLine(){
  if(length() == 0){
    return true;
  }
  return false;

}

bool KasumiString::isEntryLine(){
  size_type i;

  if(isKeyValLine()){
    return false;
  }

  i = find(" ",0);
  if(i == npos){
    return false;
  }

  i = find(" ",i+1);
  if(i == npos){
    return false;
  }

  i = find(" ",i+1);
  if(i == npos){
    return true;
  }

  return false;
}

bool KasumiString::isKeyValLine(){
  size_type i;

  i = find("=",0);

  if(i == npos){
    return false;
  }
  
  return true;
}

string KasumiString::getSound(){
  unsigned int i;

  if(isEntryLine()){
    i = find(" ",0);
    return substr(0,i);
  }

  return NULL;
}

unsigned int KasumiString::getFrequency(){
  unsigned int i,j,ret = 0;
  string sub;

  if(isEntryLine()){
    i = find(" ",0);
    j = find(" ",i+1);
    
    sub = substr(i+1,j-i-1);
    
    ret = atoi(sub.c_str());
  }  

  return ret;
}

string KasumiString::getSpelling(){
  int i;
  
  if(isEntryLine()){
    i = rfind(" ",npos);
    return substr(i+1);
  }

  return NULL;
}

string KasumiString::getKey(){
  int i;

  if(isKeyValLine()){
    i = find("=",0);
    while(c_str()[i-1] == ' ' || c_str()[i-1] == '\t'){
      i--;
    }
    
    return substr(0,i);
  }
  
  return NULL;
}

string KasumiString::getVal(){
  int i;

  if(isKeyValLine()){
    i = find("=",0);
    while(c_str()[i+1] == ' ' || c_str()[i-1] == '\t'){
      i++;
    }

    return substr(i+1);
  }

  return NULL;
}

int str2int(const string &str){
  if(isInt(str)){
    stringstream ss(str);
    int i;
    ss >> i;
    return i;
  }
  
  return 0;
}

string int2str(int i){
   stringstream ss;
   ss << i;
   return ss.str();
}

bool isInt(const string &str){
  unsigned int i;
  
  if(str == "-" || str == ""){
    return false;
  }

  if(str == "0"){
    return true;
  }

  if(str.c_str()[0] == '-'){
    i=1;
  }else{
    i=0;
  }

  if(str.c_str()[i] >= '1' && str.c_str()[i] <= '9'){
    for(i++;i<str.length();i++){
      if(str.c_str()[i] < '0' || str.c_str()[i] > '9'){
        return false;
      }
    }

    return true;
  }

  return false;
}
