/*********************************************************************
 * 
 * KasumiConfiguration.cxx
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <getopt.h> /* for getopt_long() */
#include <anthy/dicutil.h>

#include "KasumiConfiguration.hxx"
#include "KasumiException.hxx"
#include "KasumiString.hxx"
#include "KasumiWordType.hxx"
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
  throw(KasumiException){

  try{  
    loadDefaultProperties();

    // ~/.kasumi must be encoded in EUC-JP
  
    char *home = getenv("HOME");
    if(home == NULL){
      throw KasumiException(string("Cannot find $HOME environment variable."),
                            STDERR,
                            KILL);
    }

    ConfFileName = string(home) + "/.kasumi";

    loadConfigurationFile();
  }catch(KasumiException e){
    throw e;
  }

  loadConfigurationFromArgument(argc, argv);
  checkValidity();
}

KasumiConfiguration::~KasumiConfiguration(){
  saveConfiguration();
}

void KasumiConfiguration::loadDefaultProperties() throw(KasumiException){
  char *home = getenv("HOME");
  if(home == NULL){
      throw KasumiException(string("Cannot find $HOME environment variable."),
                            STDERR,

                            KILL);
  }
  
  config[string("StartupMode")] = string("MANAGE");
  config[string("DefaultFrequency")] = string("500");
  config[string("MaxFrequency")] = string("1000");
  config[string("MinFrequency")] = string("1");
//  config[string("QuitShortcutKey")] = string("Ctrl+Q");
//  config[string("StoreShortcutKey")] = string("Ctrl+S");
//  config[string("NewWordShortcutKey")] = string("Ctrl+N");
//  config[string("RemoveShortcutKey")] = string("Ctrl+R");
//  config[string("AddShortcutKey")] = string("Ctrl+A");
//  config[string("AddingModeShortcutKey")] = string("Ctrl+J");
//  config[string("ManageModeShortcutKey")] = string("Ctrl+M");
  config[string("DefaultSpelling")] = string("");
  config[string("DefaultSound")] = string("");
  config[string("DefaultWordType")] = string("#T35");
  config[string("DefaultAddingSpelling")] = string("");
  config[string("DefaultAddingSound")] = string("");
  config[string("DefaultAddingWordType")] = string("#T35");
  //  config[string("DefaultWindowPosX")] = string("-1");
  //  config[string("DefaultWindowPosY")] = string("-1");
  config[string("ImportSelectedText")] = string("true");
#ifdef HAS_ANTHY_DICUTIL_SET_ENCODING
  config[string("UseEUCJP")] = string("false");
#else // HAS_ANTHY_DICUTIL_SET_ENCODING
  config[string("UseEUCJP")] = string("true");
#endif // HAS_ANTHY_DICUTIL_SET_ENCODING
}

void KasumiConfiguration::loadConfigurationFromArgument(int argc, char *argv[])
  throw(KasumiException){
  int option_index = 0;
  static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"add", no_argument, NULL, 'a'},
    {"exclusive", no_argument, NULL, 'e'},
    {"manage", no_argument, NULL, 'm'},
    {"sound", required_argument, NULL, 's'},
    {"spelling", required_argument, NULL, 't'},
    {"wordclass", required_argument, NULL, 'w'},
    //    {"x", required_argument, NULL, 'x'},
    //    {"y", required_argument, NULL, 'y'},
    {"import", no_argument, NULL, 'i'},
    {"ignore", no_argument, NULL, 'I'},
#ifdef HAS_ANTHY_DICUTIL_SET_ENCODING
    {"eucjp", no_argument, NULL, 'E'},
#endif // HAS_ANTHY_DICUTIL_SET_ENCODING
    {0,0,0,0}
  };

  string message;
  int c;    
  while(1){
    //    c = getopt_long(argc, argv, "hvamiIns:t:w:x:y:", long_options, &option_index);
#ifdef HAS_ANTHY_DICUTIL_SET_ENCODING
    c = getopt_long(argc, argv, "hvaemiInEs:t:w:", long_options, &option_index);
#else // HAS_ANTHY_DICUTIL_SET_ENCODING
    c = getopt_long(argc, argv, "hvaemiIns:t:w:", long_options, &option_index);
#endif // HAS_ANTHY_DICUTIL_SET_ENCODING
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
    case 'e':
      setPropertyValue(string("StartupMode"),string("EXCLUSIVE"));
      break;
    case 's':
      setPropertyValue(string("DefaultAddingSound"),string(optarg));
      break;
    case 't':
      setPropertyValue(string("DefaultAddingSpelling"),string(optarg));
      break;
    case 'w':
      setPropertyValue(string("DefaultAddingWordType"),string(optarg));
      break;
      //    case 'x':
      //      setPropertyValue(string("DefaultWindowPosX"),string(optarg));
      //      break;
      //    case 'y':
      //      setPropertyValue(string("DefaultWindowPosY"),string(optarg));
      //      break;
    case 'i':
      setPropertyValue(string("ImportSelectedText"),string("true"));
      break;
    case 'I':
      setPropertyValue(string("ImportSelectedText"),string("false"));
      break;
    case 'E':
      setPropertyValue(string("UseEUCJP"),string("true"));
      break;
    case '?':
    case ':':
      message = string("Invalid argument error. Try '") + argv[0] +
        string(" --help' for more information.");
      throw KasumiException(message, STDERR, KILL);
      break;
    }
  }

  if(optind < argc){
    message = string("Found non-option argument '") + argv[optind] +
      string("'. Try '") + argv[0] +
      string(" --help' for more information.");
    throw KasumiException(message, STDERR, KILL);
  }
}


void KasumiConfiguration::loadConfigurationFile()
  throw(KasumiException){

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
      string message = ConfFileName + string(":") + int2str(line)
        + string(": invalid entry in configuration file.");
      throw KasumiException(message, STDERR, KILL);
    }
  }
}

// ToDo: implement saveConfiguration method
void KasumiConfiguration::saveConfiguration()
  throw(KasumiException){

}

void KasumiConfiguration::checkValidity()
  throw(KasumiException){
  
  if(config[string("StartupMode")] != string("MANAGE") &&
     config[string("StartupMode")] != string("ADD") &&
     config[string("StartupMode")] != string("EXCLUSIVE") &&
     config[string("StartupMode")] != string("HELP") &&
     config[string("StartupMode")] != string("VERSION")){
    string message("StartupMode variable must be \"MANAGE\", \"EXCLUSIVE\" or \"ADD\"");
    throw KasumiException(message, STDERR, KILL);
  }

  // check conrresponding settings being an integer
  list<string> intValueKeyNames;

  intValueKeyNames.push_back(string("DefaultFrequency"));
  intValueKeyNames.push_back(string("MaxFrequency"));
  intValueKeyNames.push_back(string("MinFrequency"));
  //  intValueKeyNames.push_back(string("DefaultWindowPosX"));
  //  intValueKeyNames.push_back(string("DefaultWindowPosY"));

  while(!intValueKeyNames.empty()){
    string keyName = intValueKeyNames.front();
    intValueKeyNames.pop_front();
    
    if(!isInt(config[keyName])){
      string message = keyName + string(" variable must be an integer");
      throw KasumiException(message, STDERR, KILL);
    }
  }

  // check integer value are suitable
  int def = str2int(config[string("DefaultFrequency")]);
  int max = str2int(config[string("MaxFrequency")]);
  int min = str2int(config[string("MinFrequency")]);
  if(min < 1){
    throw KasumiException(string("MinFrequency must be greater than 0"),
                          STDERR, KILL);
  }else if(max < min){
    throw KasumiException(string("MinFrequency must not be greater than MaxFrequency."),
                          STDERR, KILL);
  }else if(def > max){
    throw KasumiException(string("DefaultFrequency must not be greater than MaxFrequency"),
                          STDERR, KILL);
  }else if(def < min){
    throw KasumiException(string("DefaultFrequency must not be less than MinFrequency"),
                          STDERR, KILL);
  }

  //  int x = str2int(config[string("DefaultWindowPosX")]);
  //  int y = str2int(config[string("DefaultWindowPosY")]);
  //  if(x < -1){
  //    throw KasumiException(string("DefaultWindowPosX must be -1 or more"),
  //                          STDERR, KILL);
  //  }else if(y < -1){
  //    throw KasumiException(string("DefaultWindowPosY must be -1 or more"),
  //                          STDERR, KILL);
  //  }

  // check key configurations
  // throws exeption if there is an invalid key or duplication
//  map<string,string> registeredKey;
//  list<string> keyNames;
  
//  keyNames.push_back(string("QuitShortcutKey"));
//  keyNames.push_back(string("StoreShortcutKey"));
//  keyNames.push_back(string("NewWordShortcutKey"));
//  keyNames.push_back(string("RemoveShortcutKey"));
//  keyNames.push_back(string("AddShortcutKey"));
//  keyNames.push_back(string("AddingModeShortcutKey"));
//  keyNames.push_back(string("ManageModeShortcutKey"));

//  while(!keyNames.empty()){
//    string keyName = keyNames.front();
//    keyNames.pop_front();
//
//    string shortKey = config[string(keyName)];
//    if(!isValidShortcutKey(shortKey)){
//      string message = string("Invalid shortcut key configuration for ") +
//        keyName + string(": ") + shortKey;
//      throw KasumiException(message, STDERR, KILL);
//    }
  //  if(registeredKey.find(shortKey) == registeredKey.end()){
//      registeredKey.insert(make_pair(shortKey,keyName));
//    }else{
//      string message = string("Failed to set ") + keyName + string(" variable; ") + shortKey + string(" has been already registered as ") + registeredKey[shortKey];
//      throw KasumiException(message, STDERR, KILL);
//    }
//  }

  // check WordType configuration
  list<string> keyForWordType;
  map<string,bool> validWordType;

  keyForWordType.push_back(string("DefaultWordType"));
  keyForWordType.push_back(string("DefaultAddingWordType"));

  WordTypeList::iterator p = KasumiWordType::beginWordTypeList();
  while(p != KasumiWordType::endWordTypeList())
  {
      validWordType.insert(make_pair((*p)->getCannaTab(),true));
      p++;
  }

  while(!keyForWordType.empty()){
    string keyName = keyForWordType.front();
    keyForWordType.pop_front();
    string val = config[keyName];

    if(validWordType.find(val) == validWordType.end()){
      string message = val + string(" is an invalid word type for ") + keyName;
      throw KasumiException(message, STDERR, KILL);
    }
  }
  
  // check conrresponding settings being an boolean
  list<string> booleanValueKeyNames;

  booleanValueKeyNames.push_back(string("ImportSelectedText"));
  
  while(!booleanValueKeyNames.empty()){
    string keyName = booleanValueKeyNames.front();
    booleanValueKeyNames.pop_front();
    
    if(config[keyName] != "true" && config[keyName] != "false"){
      throw KasumiException(keyName + string(" variable must be a boolean"),
                            STDERR, KILL);
    }
  }

  // no check for:
  //  DefaultSpelling
  //  DefaultSound
  //  DefaultAddingSpelling
  //  DefaultAddingSound
  // ToDo: confirm default sounds do not have invalid character
}

void KasumiConfiguration::setPropertyValue(const string &name, const string &value){
  map<string,string>::iterator p;
  
  p = config.find(name);

  if(p == config.end()){
    cerr << "error: you cannot set " << name << " property." << endl;
    exit(1);
  }
  
  config[name] = value;
}

string KasumiConfiguration::getPropertyValue(const string &name){
  map<string,string>::iterator p;

  p = config.find(name);

  if(p == config.end()){
    cerr << "error: " << name << " property has not been set yet." << endl;
    exit(1);
  }
  
  return p->second;
}

int KasumiConfiguration::getPropertyValueByInt(const string &name){
  map<string,string>::iterator p;

  p = config.find(name);

  if(p == config.end()){
    cerr << "error: " << name << " property has not been set yet." << endl;
    exit(1);
  }
  
  return str2int(p->second);
}

bool KasumiConfiguration::getPropertyValueByBool(const string &name){
  map<string,string>::iterator p;

  p = config.find(name);
  if(p == config.end()){
    cerr << "error: " << name << " property has not been set yet." << endl;
    exit(1);
  }
  
  if(p->second == "true"){
    return true;
  } else{
    return false;
  }
}

/*
bool isValidShortcutKey(const string &key){
  string::size_type i;
  
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
*/
