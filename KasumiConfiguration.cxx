#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <getopt.h> /* for getopt_long() */

#include "KasumiConfiguration.hxx"
#include "KasumiException.hxx"
#include "KasumiString.hxx"
#include "intl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

// if you want to add a new configuration settings, do the following:
//  1. Set default value: Add a line to loadDefaultProperties() method
//     like;
//       config[string("NewKey")] = string("DefaultValue");
//  2. Add check routine for validating the setting in checkValidity method.
//     If it accepts only an integer value, it is the best to add the new key's
//     name to intValueKeyNames list like;
//       intValueKeyNames.push_back("NewKey");
//     Or if it is a shortcut key setting, add the new key's name to keyName
//     list like:
//       keyName.push_back("NewKey");
//  3. If the setting may be set by command line arguments, add some routines
//     to loadConfigurationFromArgument method

KasumiConfiguration::KasumiConfiguration(int argc, char *argv[])
  throw(KasumiConfigurationLoadException){

  try{  
    loadDefaultProperties();

    // ~/.kasumi must be encoded in EUC-JP
  
    char *home = getenv("HOME");
    if(home == NULL){
      throw KasumiConfigurationLoadException(string("Cannot find $HOME environment variable."));
    }

    ConfFileName = string(home) + "/.kasumi";

    loadConfigurationFile();
  }catch(KasumiConfigurationLoadException e){
    throw e;
  }

  loadConfigurationFromArgument(argc, argv);
  checkValidity();
}

KasumiConfiguration::~KasumiConfiguration(){
  saveConfiguration();
}

void KasumiConfiguration::loadDefaultProperties() 
  throw(KasumiConfigurationLoadException){
  char *home = getenv("HOME");
  if(home == NULL){
    throw KasumiConfigurationLoadException(string("Cannot find $HOME environment variable."));
  }
  
  config[string("StartupMode")] = string("MANAGE");
  config[string("DictionaryPath")] = string(home) + "/.private-dic.src.tmp";
  config[string("DefaultFrequency")] = string("500");
  config[string("MaxFrequency")] = string("1000");
  config[string("MinFrequency")] = string("1");
  config[string("QuitShortcutKey")] = string("Ctrl+Q");
  config[string("StoreShortcutKey")] = string("Ctrl+S");
  config[string("NewWordShortcutKey")] = string("Ctrl+N");
  config[string("RemoveShortcutKey")] = string("Ctrl+R");
  config[string("AddShortcutKey")] = string("Ctrl+A");
  config[string("AddingModeShortcutKey")] = string("Ctrl+J");
  config[string("ManageModeShortcutKey")] = string("Ctrl+M");    
  config[string("DefaultSpelling")] = string("");
  config[string("DefaultSound")] = string("");
  config[string("DefaultWordClass")] = string(EUCJP_MEISHI);
  config[string("DefaultAddingSpelling")] = string("");
  config[string("DefaultAddingSound")] = string("");
  config[string("DefaultAddingWordClass")] = string(EUCJP_MEISHI);
  config[string("DefaultWindowPosX")] = string("-1");
  config[string("DefaultWindowPosY")] = string("-1");
}

void KasumiConfiguration::loadConfigurationFromArgument(int argc, char *argv[])
 throw(KasumiConfigurationLoadException){
  int option_index = 0;
  static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"add", no_argument, NULL, 'a'},
    {"manage", no_argument, NULL, 'm'},
    {"sound", required_argument, NULL, 's'},
    {"spelling", required_argument, NULL, 't'},
    {"wordclass", required_argument, NULL, 'w'},
    {"x", required_argument, NULL, 'x'},
    {"y", required_argument, NULL, 'y'},
    {0,0,0,0}
  };

  int c;    
  while(1){
    c = getopt_long(argc, argv, "hvams:t:w:x:y:", long_options, &option_index);
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
      setPropertyValue(string("StartupMode"),string("MANAGE"));
      break;
    case 's':
      setPropertyValue(string("DefaultAddingSound"),string(optarg));
      break;
    case 't':
      setPropertyValue(string("DefaultAddingSpelling"),string(optarg));
      break;
    case 'w':
      setPropertyValue(string("DefaultAddingWordClass"),string(optarg));
      break;
    case 'x':
      setPropertyValue(string("DefaultWindowPosX"),string(optarg));
      break;
    case 'y':
      setPropertyValue(string("DefaultWindowPosY"),string(optarg));
      break;
    case '?':
      throw KasumiConfigurationLoadException(string("Found an invalid argument"));
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
      throw KasumiConfigurationLoadException(string("line ") + int2str(line) + string(": Invalid entry in configuration file (" + ConfFileName + ")"));
    }
  }
}

void KasumiConfiguration::saveConfiguration()
  throw(KasumiConfigurationSaveException){

}

void KasumiConfiguration::checkValidity()
  throw(KasumiConfigurationLoadException){
  unsigned int i;
  
  if(config[string("StartupMode")] != string("MANAGE") && config[string("StartupMode")] != string("ADD")){
    throw KasumiConfigurationLoadException(string("StartupMode variable must be \"MANAGE\" or \"ADD\""));
  }

  // check conrresponding settings being an integer
  list<string> intValueKeyNames;

  intValueKeyNames.push_back(string("DefaultFrequency"));
  intValueKeyNames.push_back(string("MaxFrequency"));
  intValueKeyNames.push_back(string("MinFrequency"));
  intValueKeyNames.push_back(string("DefaultWindowPosX"));
  intValueKeyNames.push_back(string("DefaultWindowPosY"));

  while(!intValueKeyNames.empty()){
    string keyName = intValueKeyNames.front();
    intValueKeyNames.pop_front();
    
    if(!isInt(config[keyName])){
    throw KasumiConfigurationLoadException(keyName + string(" variable must be an integer"));      
    }
  }

  // check integer value are suitable
  int def = str2int(config[string("DefaultFrequency")]);
  int max = str2int(config[string("MaxFrequency")]);
  int min = str2int(config[string("MinFrequency")]);
  if(min < 1){
    throw KasumiConfigurationLoadException(string("MinFrequency must be greater than 0"));
  }else if(max < min){
    throw KasumiConfigurationLoadException(string("MinFrequency must not be greater than MaxFrequency."));
  }else if(def > max){
    throw KasumiConfigurationLoadException(string("DefaultFrequency must not be greater than MaxFrequency"));
  }else if(def < min){
    throw KasumiConfigurationLoadException(string("DefaultFrequency must not be less than MinFrequency"));
  }

  int x = str2int(config[string("DefaultWindowPosX")]);
  int y = str2int(config[string("DefaultWindowPosY")]);
  if(x < -1){
    throw KasumiConfigurationLoadException(string("DefaultWindowPosX must be -1 or more"));
  }else if(y < -1){
    throw KasumiConfigurationLoadException(string("DefaultWindowPosY must be -1 or more"));
  }

  // check key configurations
  // throws exeption if there is an invalid key or duplication
  map<string,string> registeredKey;
  list<string> keyNames;
  
  keyNames.push_back(string("QuitShortcutKey"));
  keyNames.push_back(string("StoreShortcutKey"));
  keyNames.push_back(string("NewWordShortcutKey"));
  keyNames.push_back(string("RemoveShortcutKey"));
  keyNames.push_back(string("AddShortcutKey"));
  keyNames.push_back(string("AddingModeShortcutKey"));
  keyNames.push_back(string("ManageModeShortcutKey"));

  while(!keyNames.empty()){
    string keyName = keyNames.front();
    keyNames.pop_front();

    string shortKey = config[string(keyName)];
    if(!isValidShortcutKey(shortKey)){
      throw KasumiConfigurationLoadException(string("Invalid shortcut key configuration for ") + keyName + string(": ") + shortKey);
    }
    if(registeredKey.find(shortKey) == registeredKey.end()){
      registeredKey.insert(make_pair(shortKey,keyName));
    }else{
      throw KasumiConfigurationLoadException(string("Failed to set ") + keyName + string(" variable; ") + shortKey + string(" has been already registered as ") + registeredKey[shortKey]);
    }
  }

  // check WordClass configuration
  list<string> keyForWordClass;
  map<string,bool> validWordClass;

  keyForWordClass.push_back(string("DefaultWordClass"));
  keyForWordClass.push_back(string("DefaultAddingWordClass"));

  validWordClass.insert(make_pair(string(EUCJP_MEISHI),true));
  validWordClass.insert(make_pair(string(EUCJP_FUKUSHI),true));
  validWordClass.insert(make_pair(string(EUCJP_JINNMEI),true));
  validWordClass.insert(make_pair(string(EUCJP_CHIMEI),true));
  validWordClass.insert(make_pair(string(EUCJP_KEIYOUSHI),true));

  while(!keyForWordClass.empty()){
    string keyName = keyForWordClass.front();
    keyForWordClass.pop_front();
    string val = config[keyName];

    if(validWordClass.find(val) == validWordClass.end()){
      throw KasumiConfigurationLoadException(val + string(" is an invalid word class for ") + keyName);
    }
  }

  // no check for:
  //  DefaultSpelling
  //  DefaultSound
  //  DefaultAddingSpelling
  //  DefaultAddingSound
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

bool isValidShortcutKey(const string &key){
  unsigned int i;
  
  i = key.find("+",0);
  string shortkey = key.substr(i+1);
  if(shortkey.length() != 1){
    return false;
  }
  char c = shortkey.c_str()[0];
  if((c < 'A' || c > 'Z') && (c < '0' || c > '9')){
    return false;
  }
  
  i = key.find("+",0);
  if(i == key.npos){
    return true;
  }
  string mask = key.substr(0,i);
  if(mask == "Ctrl"){
    return true;
  }else if(mask == "Alt"){
    return true;
  }

  return false;
}
