/*********************************************************************
 * 
 * KasumiString.cxx
 * 
 * Kasumi - a management tool for a private dictionary of anthy
 * 
 * Copyright (C) 2004-2006 Takashi Nakamoto
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 * 
*********************************************************************/

#include "KasumiString.hxx"
#include <cstdlib>
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
  string::size_type i;

  //  if(isKeyValLine()){
  //    return false;
  //  }

  i = find(" ",0);
  if(i == npos){
    return false;
  }

  i = find(" ",i+1);
  if(i == npos){
    return false;
  }

  return true;
}

bool KasumiString::isKeyValLine(){
  string::size_type i;

  i = find("=",0);

  if(i == npos){
    return false;
  }
  
  return true;
}

string KasumiString::getSound(){
  string::size_type i;

  if(isEntryLine()){
    i = find(" ",0);
    return substr(0,i);
  }

  return NULL;
}

unsigned int KasumiString::getFrequency(){
  string::size_type i,j;
  unsigned int ret=0;
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
    // allow spelling to contain space
    // thanks to pinkmonky http://d.hatena.ne.jp/pinkmonkey/20050411
    i = find(" ",0);
    i = find(" ",i+1);
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
