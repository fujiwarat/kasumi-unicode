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
  throw(KasumiDicExaminationException){

  DicFileName = aDicFileName;

  try{
    load(conf);
  }catch(KasumiDicExaminationException e){
    throw e;
  }
}

void KasumiDic::load(KasumiConfiguration *conf)
  throw(KasumiDicExaminationException){
  
  int line = 0;
  int freq;
  string DicContents = string();
  KasumiString Buffer;

  string command = "anthy-dic-tool --dump > " + DicFileName;
  if(system(command.c_str()) != 0){
    throw KasumiDicExaminationException(
            "Cannot dump Anthy dictoinary to " + DicFileName,
            0);
  }

  ifstream DicFile(DicFileName.c_str());
  
  if(!DicFile.is_open()){
    throw KasumiDicExaminationException(
            "Cannot open Anthy dicitionary file.",
            0);
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
            }catch(KasumiInvalidWordClassNameException e){
              throw KasumiDicExaminationException(e.getMessage(), line);
            }
          }else{
            if(Buffer.getVal() == "y"){
              newWord->setOption(Buffer.getKey(), true);
            }else{
              newWord->setOption(Buffer.getKey(), false);
            }
          }
        }else{
          throw KasumiDicExaminationException(
                  "Invalid entry in Anthy private dictionary file.",
                  line);
        }
      }

      appendWord(newWord);
    }else{
      // not classfied line; Anthy Dicitionary is invalid!
          throw KasumiDicExaminationException(
                  "Invalid entry in Anthy private dictionary file.",
                  line);
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
  throw(KasumiOutOfBoundException){

  size_t i;
  
  if(id >= WordList.size() || id < 0){
    throw new KasumiOutOfBoundException("Out of Bound!");
  }

  KasumiWord *word = WordList[id];
  word->setFrequency(0);

  for(i=0;i<EventListeners.size();i++){
    EventListeners[i]->removedWord(getUpperBoundOfWordID());
  }
}

void KasumiDic::modifyWord(size_t id)
  throw(KasumiOutOfBoundException){

  size_t i;
  
  if(id >= WordList.size() || id < 0){
    throw new KasumiOutOfBoundException("Out of Bound!");
  }

  for(i=0;i<EventListeners.size();i++){
    EventListeners[i]->modifiedWord(id);
  }
}


void KasumiDic::store()
  throw(KasumiDicStoreException){

  size_t i;
  ofstream DicFile(DicFileName.c_str());
  if(!DicFile.is_open()){
    cout << "Cannot overwrite data to" << DicFileName << "." << endl;
    return;
  }
  string ret = string();

  for(i=0; i<WordList.size(); i++){
    ostringstream ostr;

    if(WordList[i] == NULL)
      continue;

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
    }else{
      throw KasumiDicStoreException(
              "internal error while saving Anthy Dictionary.");
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
    if(p = listener){
      EventListeners.erase(i);
      return;
    }
  }
}

KasumiWord *KasumiDic::getWordWithID(size_t id)
  throw(KasumiOutOfBoundException){

  if(id >= WordList.size() || id < 0){
    throw new KasumiOutOfBoundException("Out of Bound!");
  }
  
  return WordList[id];
}

int KasumiDic::getUpperBoundOfWordID(){
  return WordList.size()-1;
}
