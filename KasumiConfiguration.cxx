#include <iostream>
#include <fstream>
#include <map>

#include "KasumiConfiguration.hxx"
#include "KasumiException.hxx"
#include "KasumiString.hxx"
#include "intl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiConfiguration::KasumiConfiguration()
  throw(KasumiConfigurationLoadException){
  char *home = getenv("HOME");
  if(home == NULL){
    cout << "Cannot find $HOME environment variable." << endl;
    exit(1);
  }

  ConfFileName = string(home) + "/.kasumi";
  try{
    loadConfiguration();
  }catch(KasumiConfigurationLoadException e){
    throw e;
  }
}

KasumiConfiguration::~KasumiConfiguration(){
  saveConfiguration();
}

void KasumiConfiguration::loadConfiguration()
  throw(KasumiConfigurationLoadException){

  cout << "OK1" << endl;
  
  int line = 0;
  string Contents = string();
  KasumiString Buffer;

  ifstream ConfFile(ConfFileName.c_str());

  if(!ConfFile.is_open()){
    return; // do not load configuration file
  }

  // analyze Kasumi Configuration file reading each line
  while(getline(ConfFile, Buffer, '\n')){
    line++;

    if(Buffer.isCommentLine()){
      // commented line; nothing to do
    }else if(Buffer.isEmptyLine()){
      // empty line; nothing to do
    }else if(Buffer.isKeyValLine()){
      config[Buffer.getKey()] = Buffer.getVal();
    }else{
      // not classfied line; configuration file is invalid!
      throw KasumiDicExaminationException(
                  "Invalid entry in configuration file (" + ConfFileName + ")",
                  line);
    }
  }
}

void KasumiConfiguration::saveConfiguration()
  throw(KasumiConfigurationSaveException){

}

void KasumiConfiguration::setConfiguration(const string &name, const string &value){
  config[name] = value;
}

string KasumiConfiguration::getConfiguration(const string &name){
  map<string,string>::iterator p;

  p = config.find(name);

  if(p != config.end()){
    return p->second;
  }
  
  return string();
}
