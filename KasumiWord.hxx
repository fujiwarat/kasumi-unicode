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

enum _WordClassEnum {NOUN, ADV, PERSON, PLACE, ADJ};
typedef _WordClassEnum WordClassType;

class KasumiWord{
private:
  string Sound; // corresponding "Yomi" in private-dic.src
  string Sound_UTF8;
  int Frequency; // corresponding "Hindo"
  string Spelling; // corresponding "Tango"
  string Spelling_UTF8;
  WordClassType WordClass; // corresponding "Hinshi"
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
    throw(KasumiInvalidCharacterForSoundException);
  void setSoundByUTF8(const string &aSound)
    throw(KasumiInvalidCharacterForSoundException);
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
    throw(KasumiInvalidWordClassNameException);
  void setWordClassWithNameByUTF8(const string &aWordClass)
    throw(KasumiInvalidWordClassNameException);
  string getStringOfWordClass();
  string getStringOfWordClassByUTF8();  
  
  void setOption(const string &aOptionName, bool aOption);
  bool getOption(const string &aOptionName);

  static string convertUTF8ToEUCJP(const string &aEUCJP);
  static string convertEUCJPToUTF8(const string &aUTF8);  
};

#endif
