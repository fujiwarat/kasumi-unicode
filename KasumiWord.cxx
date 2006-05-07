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

size_t KasumiWord::id_generator = 0;

vector<KasumiWord*> KasumiWord::words(0);

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
	setWordType(KasumiWordType::getWordTypeFromPos(conf->getPropertyValue("DefaultWordType")));
    }catch(KasumiException e){
	cout << e.getMessage() << endl;
	exit(1);
    }
}

KasumiWord* KasumiWord::createNewWord(KasumiConfiguration *conf)
{
    KasumiWord *word = new KasumiWord(conf);
    KasumiWord::words[word->id = KasumiWord::id_generator++] = word;
    return word;
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

void KasumiWord::setSpelling(const string &aSpelling){
  Spelling = aSpelling;
  Spelling_UTF8 = convertEUCJPToUTF8(Spelling);
}

void KasumiWord::setSpellingByUTF8(const string &aSpelling){
  Spelling_UTF8 = aSpelling;
  Spelling = convertUTF8ToEUCJP(Spelling_UTF8);
}

string KasumiWord::getWordTypeUIString()
{
    if(mWordType != NULL)
	return mWordType->getUIString();
    else
	return string("");
}

KasumiWord* KasumiWord::getWordFromID(size_t id)
{
    return KasumiWord::words[id];
}

/*
// for debug
int main(int argc, char *argv[])
{
    KasumiWordType::addNewWordType("Ì¾»ì", "°ìÈÌÌ¾»ì", "#T35");
    KasumiWordType::addNewWordType("Ì¾»ì", "¿ÍÌ¾", "#JN");
    KasumiWordType::addNewWordType("Ì¾»ì", "ÃÏÌ¾", "#CN");
    KasumiWordType::addNewWordType("Ì¾»ì", "²ñ¼Ò", "#KK");
    KasumiWordType::addNewWordType("Ì¾»ì", "¤¹¤ëÀÜÂ³", "#T30");
    KasumiWordType::addNewWordType("¿ô»ì", "¿ô»ì", "#NN");
    KasumiWordType::addNewWordType("·ÁÍÆ»ì", "¥¯³èÍÑ", "#KY");
    KasumiWordType::addNewWordType("·ÁÍÆ»ì", "¥·¥¯³èÍÑ", "#KYT");
    KasumiWordType::addNewWordType("·ÁÍÆÆ°»ì", "", "#T05");
    KasumiWordType::addNewWordType("Éû»ì", "", "#F04");
    KasumiWordType::addNewWordType("´¶Æ°»ì", "", "#CJ");
    KasumiWordType::addNewWordType("Ï¢ÂÎ»ì", "", "#RT");
    KasumiWordType::addNewWordType("Ã±´Á»ú", "", "#KJ");

    KasumiWord *newWord = new KasumiWord();
    newWord->setSound("¤¿¤«¤·");
    newWord->setSpelling("¿ò»Ö");
    newWord->setFrequency(180);
    newWord->setWordType(KasumiWordType::getWordTypeFromPos("Ì¾»ì"));
    cout << newWord->getSound() << " " << newWord->getSpelling() << " ";
    cout << newWord->getFrequency() << " " << endl;
    cout << newWord->getWordType()->getPos() << endl;
    cout << newWord->getWordType()->getCategory() << endl;
}
*/
