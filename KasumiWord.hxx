#ifndef __KASUMI_WORD_HXX__
#define __KASUMI_WORD_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <map>
#include "KasumiException.hxx"
#include <iconv.h>

using namespace std;

#define FREQ_UBOUND 1000
#define FREQ_LBOUND 1
#define FREQ_DEFAULT 500

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

  static string convertUTF8ToEUCJP(string aEUCJP);
  static string convertEUCJPToUTF8(string aUTF8);  
public:
  KasumiWord();

  // property functions
  void setSound(string aSound);
  void setSoundByUTF8(string aSound);
  string getSound();
  string getSoundByUTF8();

  void setFrequency(int aFrequency);
  int getFrequency();

  void setSpelling(string aSpelling);
  void setSpellingByUTF8(string aSpelling);  
  string getSpelling();
  string getSpellingByUTF8();  

  void setWordClass(WordClassType aWordClass);
  WordClassType getWordClass();

  void setWordClassWithName(string aWordClass) throw(KasumiInvalidWordClassNameException);
  void setWordClassWithNameByUTF8(string aWordClass) throw(KasumiInvalidWordClassNameException);
  string getStringOfWordClass();
  string getStringOfWordClassByUTF8();  
  
  void setOption(string aOptionName, bool aOption);
  bool getOption(string aOptionName);
};

#endif
