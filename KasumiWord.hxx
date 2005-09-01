#ifndef __KASUMI_WORD_HXX__
#define __KASUMI_WORD_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <map>
#include "KasumiException.hxx"
#include "KasumiConfiguration.hxx"
#include <iconv.h>

using namespace std;

enum _WordClassEnum {NOUN, ADV, PERSON, PLACE, ADJ, VERB};
typedef _WordClassEnum WordClassType;

enum _VerbTypeEnum {B5, G5, K5, M5, N5, R5, S5, T5, W5};
typedef _VerbTypeEnum VerbType;

class KasumiWord{
private:
  string Sound; // corresponding "Yomi" in private-dic.src
  string Sound_UTF8;
  int Frequency; // corresponding "Hindo"
  string Spelling; // corresponding "Tango"
  string Spelling_UTF8;
  WordClassType WordClass; // corresponding "Hinshi"
  VerbType eVerbType; // corresponding "KATSUYOU"
  map<string, bool> Option; // corresponding "** Setsuzoku = (y or n)"
                                      // ("y" -> true, "n" -> false)

  static iconv_t IconvUTF8_To_EUCJP;
  static iconv_t IconvEUCJP_To_UTF8;
  
  // confirms if the given sound consists of only Hiragana
  // if no invalid character, returns empty string
  static string extractInvalidCharacterFromSound(string soundByUTF8);
public:
  KasumiWord(KasumiConfiguration *conf);

  // property functions
  void setSound(const string &aSound)
    throw(KasumiException);
  void setSoundByUTF8(const string &aSound)
    throw(KasumiException);
  string getSound();
  string getSoundByUTF8();

  void setFrequency(int aFrequency);
  int getFrequency();

  void setSpelling(const string &aSpelling);
  void setSpellingByUTF8(const string &aSpelling);  
  string getSpelling();
  string getSpellingByUTF8();  

  void setWordClass(WordClassType aWordClass);
  WordClassType getWordClass();

  void setWordClassWithName(const string &aWordClass)
    throw(KasumiException);
  void setWordClassWithNameByUTF8(const string &aWordClass)
    throw(KasumiException);
  string getStringOfWordClass();
  string getStringOfWordClassByUTF8();  
  
  void setOption(const string &aOptionName, bool aOption);
  bool getOption(const string &aOptionName);

  void setVerbType(VerbType aVerbType);
  void setVerbTypeWithName(const string &aVerbType)
    throw(KasumiException);
  void setVerbTypeWithNameByUTF8(const string &aVerbType)
    throw(KasumiException);
  VerbType getVerbType();
  string getStringOfVerbType();
  string getStringOfVerbTypeByUTF8();

  static string convertUTF8ToEUCJP(const string &aEUCJP);
  static string convertEUCJPToUTF8(const string &aUTF8);  
};

#endif
