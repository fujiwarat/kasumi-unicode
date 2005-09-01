#include <string>
#include <map>
#include <iconv.h>
#include "KasumiWord.hxx"
#include "KasumiException.hxx"
#include "KasumiConfiguration.hxx"
#include "KasumiString.hxx" // for EUCJP_***(constant string)
#include "intl.h"

#include <iostream>

using namespace std;

iconv_t KasumiWord::IconvUTF8_To_EUCJP = iconv_open("EUC-JP", "UTF-8");
iconv_t KasumiWord::IconvEUCJP_To_UTF8 = iconv_open("UTF-8", "EUC-JP");

string KasumiWord::convertUTF8ToEUCJP(const string &aUTF8){
  char *utf8 = (char*)malloc(strlen(aUTF8.c_str())+1);
  strcpy(utf8, aUTF8.c_str());
  size_t len = strlen(utf8)+1;
  size_t len_eucjp = len*2;
  char *eucjp_buf = (char*)malloc(len_eucjp);
  char *eucjp = eucjp_buf;

  iconv(IconvUTF8_To_EUCJP, &utf8, &len, &eucjp_buf, &len_eucjp);
  return string(eucjp);
}

string KasumiWord::convertEUCJPToUTF8(const string &aEUCJP){
  char *eucjp = (char*)malloc(strlen(aEUCJP.c_str())+1);
  strcpy(eucjp, aEUCJP.c_str());
  size_t len = strlen(eucjp)+1;
  size_t len_utf8 = len*2;
  char *utf8_buf = (char*)malloc(len_utf8);
  char *utf8 = utf8_buf;

  iconv(IconvEUCJP_To_UTF8, &eucjp, &len, &utf8_buf, &len_utf8);
  return string(utf8);
}

string KasumiWord::extractInvalidCharacterFromSound(string soundByUTF8){
  const char *str = soundByUTF8.c_str();
  unsigned char first,second,third;
  char res[5];
  int i,size;

  size = soundByUTF8.size();

  for(i=0;i<size;i+=3){
    first = static_cast<unsigned char> (str[i]);
    second = static_cast<unsigned char> (str[i+1]);
    third = static_cast<unsigned char> (str[i+2]);
    
    if(first <= 0x7f){
      res[0] = str[i];
      res[1] = 0;
      return string(res);
    }else if(first >= 0x80 && first <= 0xc1){
      // Even it is not encoded in UTF-8
      string("non-UTF8 code detected");
    }else if(first >= 0xc2 && first <= 0xdf){
      res[0] = str[i];
      res[1] = str[i+1];
      res[2] = 0;
      return string(res);
    }else if(first >= 0xe0 && first <= 0xef){
      if(first == 0xe3 && second == 0x83 && third == 0xbc){
        // dash
        continue;
      }else if(first == 0xe3 && second == 0x81 && third >= 0x81 && third <= 0xbf){
        // hiragana from "A" to "MI"
        continue;
      }else if(first == 0xe3 && second == 0x82 && third >= 0x80 && third <= 0x94){
        // the rest of hiragana
        continue;
      }else{
        res[0] = str[i];
        res[1] = str[i+1];
        res[2] = str[i+2];
        res[3] = 0;
        return string(res);
      }
    }else if(first >= 0xf0 && first <= 0xf4){
      res[0] = str[i];
      res[1] = str[i+1];
      res[2] = str[i+2];
      res[3] = str[i+3];
      res[4] = 0;
      return string(res);
    }else{
      // Even it is not encoded in UTF-8
      string("non-UTF8 code detected");
    }
  }

  return string();
}

KasumiWord::KasumiWord(KasumiConfiguration *conf){
  setSound(conf->getPropertyValue("DefaultSound"));
  setSpelling(conf->getPropertyValue("DefaultSpelling"));
  Frequency = conf->getPropertyValueByInt("DefaultFrequency");
  try{
    setWordClassWithName(conf->getPropertyValue("DefaultWordClass"));
  }catch(KasumiException e){
    cout << e.getMessage() << endl;
    exit(1);
  }
}

void KasumiWord::setSound(const string &aSound)
  throw(KasumiException){
  string tmp = convertEUCJPToUTF8(Sound);
  string invalidChar = extractInvalidCharacterFromSound(tmp);

  if(invalidChar != ""){
      string message;
      message = string(_("Sound must consist of only Hiragana characters. You have entered invalid character: "));
      message += invalidChar;
      throw KasumiException(message, ERR_DIALOG, ALERT_ONLY);
  }
  
  Sound = aSound;
  Sound_UTF8 = convertEUCJPToUTF8(Sound);
}

void KasumiWord::setSoundByUTF8(const string &aSound)
  throw(KasumiException){
  string invalidChar = extractInvalidCharacterFromSound(aSound);

  if(invalidChar != ""){
      string message;
      message = string(_("Sound must consist of only Hiragana characters. You have entered invalid character: "));
      message += invalidChar;
      throw KasumiException(message, ERR_DIALOG, ALERT_ONLY);
  }
  
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


void KasumiWord::setSpelling(const string &aSpelling){
  Spelling = aSpelling;
  Spelling_UTF8 = convertEUCJPToUTF8(Spelling);
}

void KasumiWord::setSpellingByUTF8(const string &aSpelling){
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

void KasumiWord::setWordClassWithName(const string &aWordClass)
  throw(KasumiException){
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
  }else if(aWordClass == EUCJP_DOUSHI){
    WordClass = VERB;
  }else{
    string message = string("Invalid word class name: ") + aWordClass;
    throw KasumiException(message, ERR_DIALOG, ALERT_ONLY);
  }
  
}

void KasumiWord::setWordClassWithNameByUTF8(const string &aWordClass)
  throw(KasumiException){
  try{
    string euc = convertUTF8ToEUCJP(aWordClass);
    setWordClassWithName(euc);
  }catch(KasumiException e){
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
  case VERB:
    return EUCJP_DOUSHI;
  }

  return "";
}

string KasumiWord::getStringOfWordClassByUTF8(){
  string euc = getStringOfWordClass();
  return convertEUCJPToUTF8(euc);
}

void KasumiWord::setVerbType(VerbType aVerbType){
  eVerbType = aVerbType;
}

void KasumiWord::setVerbTypeWithName(const string &aVerbType)
  throw(KasumiException){
    if(aVerbType == EUCJP_BAGYOUGODAN){
      eVerbType = B5;
    }else if(aVerbType == EUCJP_GAGYOUGODAN){
      eVerbType = G5;
    }else if(aVerbType == EUCJP_KAGYOUGODAN){
      eVerbType = K5;
    }else if(aVerbType == EUCJP_MAGYOUGODAN){
      eVerbType = M5;
    }else if(aVerbType == EUCJP_NAGYOUGODAN){
      eVerbType = N5;
    }else if(aVerbType == EUCJP_RAGYOUGODAN){
      eVerbType = R5;
    }else if(aVerbType == EUCJP_SAGYOUGODAN){
      eVerbType = S5;
    }else if(aVerbType == EUCJP_TAGYOUGODAN){
      eVerbType = T5;
    }else if(aVerbType == EUCJP_WAGYOUGODAN){
      eVerbType = W5;
    }else{
      string message = string("Invalid verb type name: ") + aVerbType;
      throw KasumiException(message, ERR_DIALOG, ALERT_ONLY);
    }
}

void KasumiWord::setVerbTypeWithNameByUTF8(const string &aVerbType)
  throw(KasumiException){
  try{
    string euc = convertUTF8ToEUCJP(aVerbType);
    setVerbTypeWithName(euc);
  }catch(KasumiException e){
    throw e;
  }
}

VerbType KasumiWord::getVerbType(){
  return eVerbType;
}

string KasumiWord::getStringOfVerbType(){
  switch(eVerbType){
  case B5:
    return EUCJP_BAGYOUGODAN;
  case G5:
    return EUCJP_GAGYOUGODAN;
  case K5:
    return EUCJP_KAGYOUGODAN;
  case M5:
    return EUCJP_MAGYOUGODAN;
  case N5:
    return EUCJP_NAGYOUGODAN;
  case R5:
    return EUCJP_RAGYOUGODAN;
  case S5:
    return EUCJP_SAGYOUGODAN;
  case T5:
    return EUCJP_TAGYOUGODAN;
  case W5:
    return EUCJP_WAGYOUGODAN;
  }

  return "";
}

string KasumiWord::getStringOfVerbTypeByUTF8(){
  string euc = getStringOfVerbType();
  return convertEUCJPToUTF8(euc);
}


void KasumiWord::setOption(const string &aOptionName, bool aOption){
  map<string,bool>::iterator p;

  p = Option.find(aOptionName);

  if(p == Option.end()){
    Option.insert(make_pair(aOptionName,aOption));    
  }else{
    Option[aOptionName] = aOption;
  }
}

bool KasumiWord::getOption(const string &aOptionName){
  map<string, bool>::iterator p;

  p = Option.find(aOptionName);

  if(p != Option.end()){
    return p->second;
  }
  
  return false;
}


