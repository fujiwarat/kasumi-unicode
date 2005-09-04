#include "KasumiDic.hxx"
#include "KasumiWord.hxx"
#include "KasumiString.hxx"
#include "KasumiException.hxx"
#include "KasumiConfiguration.hxx"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define OptionOutput( Word, OptionName ) (string(OptionName) + " = " + (Word->getOption(OptionName) ? "y" : "n"))

KasumiDic::KasumiDic(const string aDicFileName, KasumiConfiguration *conf)
  throw(KasumiException){

  DicFileName = aDicFileName;

  try{
    load(conf);
  }catch(KasumiException e){
    throw e;
  }
}

void KasumiDic::load(KasumiConfiguration *conf)
  throw(KasumiException){
  
  int line = 0;
  int freq;
  string DicContents = string();
  KasumiString Buffer;

  string command = "anthy-dic-tool --dump > " + DicFileName;
  if(system(command.c_str()) != 0){
    string message = string("Failed to dump Anthy dictionary to ") +
      DicFileName + string(".");
    throw KasumiException(message, STDERR, KILL);
  }

  ifstream DicFile(DicFileName.c_str());
  
  if(!DicFile.is_open()){
    string message = string("Failed to open ") + DicFileName + string(".");
    throw KasumiException(message, STDERR, KILL);
  }

  const int FREQ_LBOUND = conf->getPropertyValueByInt("MinFrequency");  
  const int FREQ_UBOUND = conf->getPropertyValueByInt("MaxFrequency");
  
  // analyze Anthy Dictionary reading each line
  while(getline(DicFile, Buffer, '\n')){
    line++;

    if(Buffer.isCommentLine()){
      // commented line; nothing to do
    }else if(Buffer.isEmptyLine()){
      // empty line; nothing to do
    }else if(Buffer.isEntryLine()){
      KasumiWord *newWord = new KasumiWord(conf);

      newWord->setSound(Buffer.getSound());
      freq = Buffer.getFrequency();
      if(freq < FREQ_LBOUND){
        newWord->setFrequency(FREQ_LBOUND);
      }else if(freq > FREQ_UBOUND){
        newWord->setFrequency(FREQ_UBOUND);
      }else{
        newWord->setFrequency(freq);
      }
      newWord->setSpelling(Buffer.getSpelling());

      while(getline(DicFile, Buffer, '\n')){
        line++;
        if(Buffer.isCommentLine()){
          // commented line; nothing to do
        }else if(Buffer.isEmptyLine()){
          // empty line; end of entry analysis
          break;
        }else if(Buffer.isKeyValLine()){
          if(Buffer.getKey() == EUCJP_HINNSHI){
            try{
              newWord->setWordClassWithName(Buffer.getVal());
            }catch(KasumiException e){
              throw e;
            }
          }else if(Buffer.getKey() == EUCJP_KATSUYOU){
            newWord->setVerbTypeWithName(Buffer.getVal());
          }else{
            if(Buffer.getVal() == "y"){
              newWord->setOption(Buffer.getKey(), true);
            }else{
              newWord->setOption(Buffer.getKey(), false);
            }
          }
        }else{
          string message = DicFileName + string(":") + int2str(line) +
            string(": Invalid entry");
          throw KasumiException(message, STDERR, KILL);
        }
      }

      appendWord(newWord);
    }else{
      // not classfied line; Anthy Dicitionary is invalid!
      string message = DicFileName + string(":") + int2str(line) +
        string(": Invalid entry");
      throw KasumiException(message, STDERR, KILL);
    }
  }

}

int KasumiDic::appendWord(KasumiWord *word){
  size_t i;
  
  WordList.push_back(word);
  
  for(i=0;i<EventListeners.size();i++){
    EventListeners[i]->appendedWord(getUpperBoundOfWordID());
  }

  return getUpperBoundOfWordID();
}

void KasumiDic::removeWord(size_t id)
  throw(KasumiException){

  size_t i;
  
  if(id >= WordList.size() || id < 0){
    throw KasumiException("internal error: \"id\" is out of bound!",
                          ERR_DIALOG, KILL);
  }

  KasumiWord *word = WordList[id];
  word->setFrequency(0);

  for(i=0;i<EventListeners.size();i++){
    EventListeners[i]->removedWord(getUpperBoundOfWordID());
  }
}

void KasumiDic::modifyWord(size_t id)
  throw(KasumiException){

  size_t i;
  
  if(id >= WordList.size() || id < 0){
    throw KasumiException("internal error: \"id\" is out of bound!",
                          ERR_DIALOG, KILL);
  }

  for(i=0;i<EventListeners.size();i++){
    EventListeners[i]->modifiedWord(id);
  }
}


void KasumiDic::store()
  throw(KasumiException){

  size_t i;
  ofstream DicFile(DicFileName.c_str());
  if(!DicFile.is_open()){
    cout << "Cannot overwrite data to" << DicFileName << "." << endl;
    return;
  }
  string ret = string();

  FILE *fp;
  char str[256],*ptr;

  fp=popen("anthy-dic-tool --version","r");
  while(1){
    fgets(str,256,fp);
    if(feof(fp)){
      break;
    }
     ptr=strchr(str,'\n');
    if(ptr!=NULL){
      *ptr='\0';
    }
  }
  pclose(fp);

  string version_str = string(str).substr(string("Anthy-dic-util ").size(),4);
  int anthy_version = atoi(version_str.c_str());

  for(i=0; i<WordList.size(); i++){
    ostringstream ostr;

    if(WordList[i] == NULL)
      continue;

    // anthy 6131 or later support verb registration
    if(WordList[i]->getWordClass() == VERB && anthy_version < 6131){
      cout << "Anthy " << anthy_version << " cannot support verb registration.\n";
      cout << "Skip " << WordList[i]->getSpelling() << "\n";
      continue;
    }

    ret += WordList[i]->getSound() + " ";
    ostr << WordList[i]->getFrequency();
    ret += ostr.str() + " ";
    ret += WordList[i]->getSpelling() + "\n";
    ret += string(EUCJP_HINNSHI) + " = " +
      WordList[i]->getStringOfWordClass() + "\n";

    if(WordList[i]->getWordClass() == NOUN){
      ret += OptionOutput(WordList[i], EUCJP_NASETSUZOKU) + "\n";
      ret += OptionOutput(WordList[i], EUCJP_SASETSUZOKU) + "\n";
      ret += OptionOutput(WordList[i], EUCJP_SURUSETSUZOKU) + "\n";
      ret += OptionOutput(WordList[i], EUCJP_GOKANNNOMIDEBUNNSETSU) + "\n";
      ret += OptionOutput(WordList[i], EUCJP_KAKUJOSHISETSUZOKU) + "\n";
    }else if(WordList[i]->getWordClass() == ADV){
      ret += OptionOutput(WordList[i], EUCJP_TOSETSUZOKU) + "\n";      
      ret += OptionOutput(WordList[i], EUCJP_TARUSETSUZOKU) + "\n";      
      ret += OptionOutput(WordList[i], EUCJP_SURUSETSUZOKU) + "\n";
      ret += OptionOutput(WordList[i], EUCJP_GOKANNNOMIDEBUNNSETSU) + "\n";
    }else if(WordList[i]->getStringOfWordClass() == EUCJP_JINNMEI){
      // nothing to do
    }else if(WordList[i]->getStringOfWordClass() == EUCJP_CHIMEI){
      // nothing to do
    }else if(WordList[i]->getStringOfWordClass() == EUCJP_KEIYOUSHI){
      // nothing to do
    }else if(WordList[i]->getWordClass() == VERB){
      ret += string(EUCJP_KATSUYOU) + " = " + WordList[i]->getStringOfVerbType() + "\n";
      ret += OptionOutput(WordList[i], EUCJP_RENNYOUKEINOMEISHIKA) + "\n";
    }else{
      throw KasumiException(string("Internal error while saving."),
                            ERR_DIALOG, KILL);
    }
    
    ret += "\n";
  }

  DicFile << ret;
  DicFile.close();

  string command = "cat " + DicFileName + "|anthy-dic-tool --load";
  if(system(command.c_str()) == 0){
    cout << "correctly registered dictionary" << endl;
  }else{
    cout << "cannot register dicitionary for some reason" << endl;
    exit(1);
  }
}

void KasumiDic::registerEventListener(KasumiDicEventListener *listener){
  int i,size;

  size = EventListeners.size();

  // if the listener have been already registered, nothing to do
  // assuring no duplication
  for(i=0;i<size;i++){
    if(EventListeners[i] == listener){
      return;
    }
  }
  
  EventListeners.push_back(listener);  
}

void KasumiDic::removeEventListener(KasumiDicEventListener *listener){
  vector<KasumiDicEventListener*>::iterator i;
  KasumiDicEventListener *p;

  for(i=EventListeners.begin();i!=EventListeners.end();i++){
    p = *i;
    if(p == listener){
      EventListeners.erase(i);
      return;
    }
  }
}

KasumiWord *KasumiDic::getWordWithID(size_t id)
  throw(KasumiException){

  if(id >= WordList.size() || id < 0){
    throw KasumiException("internal error: \"id\" is out of bound!",
                          ERR_DIALOG, KILL);
  }
  
  return WordList[id];
}

int KasumiDic::getUpperBoundOfWordID(){
  return WordList.size()-1;
}
