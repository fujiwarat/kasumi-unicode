#include <string>
#include <map>
#include <iconv.h>
#include "KasumiWord.hxx"
#include "KasumiException.hxx"
#include "KasumiString.hxx" // for EUCJP_***(constant string)

#include <iostream>

using namespace std;

iconv_t KasumiWord::IconvUTF8_To_EUCJP = iconv_open("EUC-JP", "UTF-8");
iconv_t KasumiWord::IconvEUCJP_To_UTF8 = iconv_open("UTF-8", "EUC-JP");

string KasumiWord::convertUTF8ToEUCJP(string aUTF8){
  char *utf8 = (char*)malloc(strlen(aUTF8.c_str())+1);
  strcpy(utf8, aUTF8.c_str());
  size_t len = strlen(utf8)+1;
  size_t len_eucjp = len*2;
  char *eucjp_buf = (char*)malloc(len_eucjp);
  char *eucjp = eucjp_buf;

  iconv(IconvUTF8_To_EUCJP, &utf8, &len, &eucjp_buf, &len_eucjp);
  return string(eucjp);
}

string KasumiWord::convertEUCJPToUTF8(string aEUCJP){
  char *eucjp = (char*)malloc(strlen(aEUCJP.c_str())+1);
  strcpy(eucjp, aEUCJP.c_str());
  size_t len = strlen(eucjp)+1;
  size_t len_utf8 = len*2;
  char *utf8_buf = (char*)malloc(len_utf8);
  char *utf8 = utf8_buf;

  iconv(IconvEUCJP_To_UTF8, &eucjp, &len, &utf8_buf, &len_utf8);
  return string(utf8);
}

KasumiWord::KasumiWord(){
  Sound = string();
  Sound_UTF8 = string();
  Frequency = FREQ_DEFAULT;
  Spelling = string();
  Spelling_UTF8 = string();
  WordClass = NOUN;
}

void KasumiWord::setSound(string aSound){
  Sound = aSound;
  Sound_UTF8 = convertEUCJPToUTF8(Sound);
}

void KasumiWord::setSoundByUTF8(string aSound){
  Sound_UTF8 = aSound;
  Sound = convertUTF8ToEUCJP(Sound_UTF8);
}

string KasumiWord::getSound(){
  return Sound;
}

string KasumiWord::getSoundByUTF8(){
  return Sound_UTF8;
}

void KasumiWord::setFrequency(int aFrequency){
  Frequency = aFrequency;
}

int KasumiWord::getFrequency(){
  return Frequency;
}


void KasumiWord::setSpelling(string aSpelling){
  Spelling = aSpelling;
  Spelling_UTF8 = convertEUCJPToUTF8(Spelling);
}

void KasumiWord::setSpellingByUTF8(string aSpelling){
  Spelling_UTF8 = aSpelling;
  Spelling = convertUTF8ToEUCJP(Spelling_UTF8);
}

string KasumiWord::getSpelling(){
  return Spelling;
}

string KasumiWord::getSpellingByUTF8(){
  return Spelling_UTF8;
}

void KasumiWord::setWordClass(WordClassType aWordClass){
  WordClass = aWordClass;
}

WordClassType KasumiWord::getWordClass(){
  return WordClass;
}

void KasumiWord::setWordClassWithName(string aWordClass)
  throw(KasumiInvalidWordClassNameException){
  if(aWordClass == EUCJP_MEISHI){
    WordClass = NOUN;
  }else if(aWordClass == EUCJP_FUKUSHI){
    WordClass = ADV;
  }else if(aWordClass == EUCJP_JINNMEI){
    WordClass = PERSON;
  }else if(aWordClass == EUCJP_CHIMEI){
    WordClass = PLACE;
  }else if(aWordClass == EUCJP_KEIYOUSHI){
    WordClass = ADJ;
  }else{
    aWordClass += " cannot be used as Class Name.";
    throw KasumiInvalidWordClassNameException(aWordClass);
  }
}

void KasumiWord::setWordClassWithNameByUTF8(string aWordClass)
  throw(KasumiInvalidWordClassNameException){
  try{
    string euc = convertUTF8ToEUCJP(aWordClass);
    if(euc == EUCJP_MEISHI){
      WordClass = NOUN;
    }else if(euc == EUCJP_FUKUSHI){
      WordClass = ADV;
    }else if(euc == EUCJP_JINNMEI){
      WordClass = PERSON;
    }else if(euc == EUCJP_CHIMEI){
      WordClass = PLACE;
    }else if(euc == EUCJP_KEIYOUSHI){
      WordClass = ADJ;
    }else{
      euc += " cannot be used as Class Name.";
      throw KasumiInvalidWordClassNameException(euc);
    }
  }catch(KasumiInvalidWordClassNameException e){
    throw e;
  }
}

string KasumiWord::getStringOfWordClass(){
  switch(WordClass){
  case NOUN:
    return EUCJP_MEISHI;
  case ADV:
    return EUCJP_FUKUSHI;
  case PERSON:
    return EUCJP_JINNMEI;
  case PLACE:
    return EUCJP_CHIMEI;
  case ADJ:
    return EUCJP_KEIYOUSHI;
  }

  return "";
}

string KasumiWord::getStringOfWordClassByUTF8(){
  switch(WordClass){
  case NOUN:
    return convertEUCJPToUTF8(EUCJP_MEISHI);
  case ADV:
    return convertEUCJPToUTF8(EUCJP_FUKUSHI);
  case PERSON:
    return convertEUCJPToUTF8(EUCJP_JINNMEI);
  case PLACE:
    return convertEUCJPToUTF8(EUCJP_CHIMEI);
  case ADJ:
    return convertEUCJPToUTF8(EUCJP_KEIYOUSHI);
  }

  return "";
}

void KasumiWord::setOption(string aOptionName, bool aOption){
  map<string,bool>::iterator p;

  p = Option.find(aOptionName);

  if(p == Option.end()){
    Option.insert(make_pair(aOptionName,aOption));    
  }else{
    Option[aOptionName] = aOption;
  }
}

bool KasumiWord::getOption(string aOptionName){
  map<string, bool>::iterator p;

  p = Option.find(aOptionName);

  if(p != Option.end()){
    return p->second;
  }
  
  return false;
}


