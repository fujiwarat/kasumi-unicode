#include <iostream>
#include <fstream>
#include <map>
#include <getopt.h> /* for getopt_long() */

#include "KasumiConfiguration.hxx"
#include "KasumiException.hxx"
#include "KasumiString.hxx"
#include "intl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiConfiguration::KasumiConfiguration(int argc, char *argv[])
  throw(KasumiConfigurationLoadException){

  loadDefaultProperties();
  loadConfigurationFromArgument(argc, argv);

  // ~/.kasumi must be encoded in EUC-JP
  
  char *home = getenv("HOME");
  if(home == NULL){
    cout << "Cannot find $HOME environment variable." << endl;
    exit(1);
  }

  ConfFileName = string(home) + "/.kasumi";
  try{
    loadConfigurationFile();
  }catch(KasumiConfigurationLoadException e){
    throw e;
  }
}

KasumiConfiguration::~KasumiConfiguration(){
  saveConfiguration();
}

void KasumiConfiguration::loadDefaultProperties(){
  char *home = getenv("HOME");
  if(home == NULL){
    cout << "Cannot find $HOME environment variable." << endl;
    exit(1);
  }
  
  config[string("StartupMode")] = string("MAIN");
  config[string("DictionaryPath")] = string(home) + "/.anthy/private-dic.src.tmp";
  config[string("DefaultFrequency")] = string("500");
  config[string("MaxFrequency")] = string("1000");
  config[string("MinFrequency")] = string("1");
  config[string("QuitShortcutKey")] = string("Ctrl+Q");
  config[string("StoreShortcutKey")] = string("Ctrl+S");
  config[string("NewWordShortcutKey")] = string("Ctrl+N");
  config[string("RemoveShortcutKey")] = string("Ctrl+R");
  config[string("AddShortcutKey")] = string("Ctrl+A");
  config[string("DefaultSpelling")] = string("");
  config[string("DefaultSound")] = string("");
  config[string("DefaultWordClass")] = string(EUCJP_MEISHI);
  config[string("DefaultAddingSpelling")] = string("");
  config[string("DefaultAddingSound")] = string("");
  config[string("DefaultAddingWordClass")] = string(EUCJP_MEISHI);
}

void KasumiConfiguration::loadConfigurationFromArgument(int argc, char *argv[]){
  int option_index = 0;
  static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"add", no_argument, NULL, 'a'},
    {"main", no_argument, NULL, 'm'},
    {0,0,0,0}
  };

  int c;    
  while(1){
    c = getopt_long(argc, argv, "hvam", long_options, &option_index);
    if(c == -1) break; // no more argument

    switch(c){
    case 'h':
      setPropertyValue(string("StartupMode"),string("HELP"));    
      break;
    case 'v':
      setPropertyValue(string("StartupMode"),string("VERSION"));    
      break;
    case 'a':
      setPropertyValue(string("StartupMode"),string("ADD"));
      break;
    case 'm':
      setPropertyValue(string("StartupMode"),string("MAIN"));
      break;
    case '?':
      cout << "Argument error." << endl;
      exit(1);
      break;
    }
  }

  if(optind < argc){
    cout << "non-option ARGV-elements: ";
    while (optind < argc)
      cout << argv[optind++];
    cout << endl;
  }
}


void KasumiConfiguration::loadConfigurationFile()
  throw(KasumiConfigurationLoadException){

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

void KasumiConfiguration::setPropertyValue(const string &name, const string &value){
  map<string,string>::iterator p;
  
  p = config.find(name);

  if(p == config.end()){
    cout << "error: you cannot set " << name << " property." << endl;
    exit(1);
  }
  
  config[name] = value;
}

string KasumiConfiguration::getPropertyValue(const string &name){
  map<string,string>::iterator p;

  p = config.find(name);

  if(p == config.end()){
    cout << "error: " << name << " property has not been set yet." << endl;
    exit(1);
  }
  
  return p->second;
}

int KasumiConfiguration::getPropertyValueByInt(const string &name){
  map<string,string>::iterator p;

  p = config.find(name);

  if(p == config.end()){
    cout << "error: " << name << " property has not been set yet." << endl;
    exit(1);
  }
  
  return str2int(p->second);
}
