#include "KasumiDic.hxx"
#include "KasumiWord.hxx"
#include "KasumiString.hxx"
#include "KasumiException.hxx"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define OptionOutput( Word, OptionName ) (string(OptionName) + " = " + (Word->getOption(OptionName) ? "y" : "n"))

KasumiDic::KasumiDic(const string aDicFileName) throw(KasumiDicExaminationException){
  DicFileName = aDicFileName;

  try{
    load();
  }catch(KasumiDicExaminationException e){
    throw e;
  }
}

void KasumiDic::load() throw(KasumiDicExaminationException){
  int line = 0;
  string DicContents = string();
  KasumiString Buffer;

  ifstream DicFile(DicFileName.c_str());

  if(!DicFile.is_open()){
    DicFile.close();    
    ofstream NewDicFile(DicFileName.c_str());
    NewDicFile.close();
    DicFile.open(DicFileName.c_str());
  }
  
  if(!DicFile.is_open()){
    throw KasumiDicExaminationException("Cannot open Anthy dicitionary file.", 0);
  }

  // analyze Anthy Dictionary reading each line
  while(getline(DicFile, Buffer, '\n')){
    line++;

    if(Buffer.isCommentLine()){
      // commented line; nothing to do
    }else if(Buffer.isEmptyLine()){
      // empty line; nothing to do
    }else if(Buffer.isEntryLine()){
      KasumiWord *newWord = new KasumiWord();

      newWord->setSound(Buffer.getSound());
      newWord->setFrequency(Buffer.getFrequency());
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
          throw KasumiDicExaminationException("Invalid entry in Anthy private dictionary file.", line);
        }
      }

      appendWord(newWord);
    }else{
      // not classfied line; Anthy Dicitionary is invalid!
          throw KasumiDicExaminationException("Invalid entry in Anthy private dictionary file.", line);
    }
  }
}

int KasumiDic::appendWord(KasumiWord *word){
  int i;
  
  WordList.push_back(word);
  
  for(i=0;i<EventListeners.size();i++){
    EventListeners[i]->appendedWord(getUpperBoundOfWordID());
  }

  return getUpperBoundOfWordID();
}

void KasumiDic::removeWord(int id) throw(KasumiOutOfBoundException){
  int i;
  
  if(id >= WordList.size() || id < 0){
    throw new KasumiOutOfBoundException("Out of Bound!");
  }

  KasumiWord *word = WordList[id];
  WordList[id] = NULL;

  for(i=0;i<EventListeners.size();i++){
    EventListeners[i]->removedWord(getUpperBoundOfWordID());
  }
  
  free(word);
}

void KasumiDic::store() throw(KasumiDicStoreException){
  int i;
  ofstream DicFile(DicFileName.c_str());
  if(!DicFile.is_open()){
    cout << "Cannot overwrite data to" << DicFileName << "." << endl;
    return;
  }
  string ret = string();
  
  for(i=0; i<WordList.size(); i++){
    ostringstream ostr;

    ret += WordList[i]->getSound() + " ";
    ostr << WordList[i]->getFrequency();
    ret += ostr.str() + " ";
    ret += WordList[i]->getSpelling() + "\n";
    ret += string(EUCJP_HINNSHI) + " = " + WordList[i]->getStringOfWordClass() + "\n";

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
      throw KasumiDicStoreException("internal error while saving Anthy Dictionary.");
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
  EventListeners.push_back(listener);  
}

KasumiWord *KasumiDic::getWordWithID(int id) throw(KasumiOutOfBoundException){
  if(id >= WordList.size() || id < 0){
    throw new KasumiOutOfBoundException("Out of Bound!");
  }
  
  return WordList[id];
}

int KasumiDic::getUpperBoundOfWordID(){
  return WordList.size()-1;
}
