#include "KasumiString.hxx"

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
  int i;

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
  int i;

  i = find("=",0);

  if(i == npos){
    return false;
  }
  
  return true;
}

string KasumiString::getSound(){
  int i;

  if(isEntryLine()){
    i = find(" ",0);
    return substr(0,i);
  }

  return NULL;
}

int KasumiString::getFrequency(){
  int i,j,ret;
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
    while(c_str()[i-1] == ' '){
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
    while(c_str()[i+1] == ' '){
      i++;
    }

    return substr(i+1);
  }

  return NULL;
}
