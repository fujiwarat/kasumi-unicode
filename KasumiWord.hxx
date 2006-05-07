#ifndef __KASUMI_WORD_HXX__
#define __KASUMI_WORD_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <map>
#include "KasumiException.hxx"
#include "KasumiConfiguration.hxx"
#include "KasumiWordType.hxx"
#include <iconv.h>
#include <vector>

using namespace std;

class KasumiWord{
private:
    string Sound; // corresponding "Yomi" in private-dic.src
    string Sound_UTF8;
    int Frequency; // corresponding "Hindo"
    string Spelling; // corresponding "Tango"
    string Spelling_UTF8;
    size_t id;

    KasumiWordType *mWordType;

    static iconv_t IconvUTF8_To_EUCJP;
    static iconv_t IconvEUCJP_To_UTF8;
  
    // confirms if the given sound consists of only Hiragana
    // if no invalid character, returns empty string
    static string extractInvalidCharacterFromSound(string soundByUTF8);

    static size_t id_generator;
    static vector<KasumiWord*> words;

    KasumiWord(KasumiConfiguration *conf);
public:
    static KasumiWord* createNewWord(KasumiConfiguration *conf);

    // property functions
    void setSound(const string &aSound)
      throw(KasumiException);
    void setSoundByUTF8(const string &aSound)
      throw(KasumiException);
    string getSound(){ return Sound; };
    string getSoundByUTF8(){ return Sound_UTF8; };

    void setFrequency(int aFrequency){ Frequency = aFrequency; };
    int getFrequency(){ return Frequency; };

    void setSpelling(const string &aSpelling);
    void setSpellingByUTF8(const string &aSpelling);  
    string getSpelling(){ return Spelling; };
    string getSpellingByUTF8(){ return Spelling_UTF8; };  

    void setWordType(KasumiWordType *aType) { mWordType = aType; };
    KasumiWordType *getWordType() { return mWordType; };
    string getWordTypeUIString(); // UTF8

    size_t getID(){ return id; };

    static KasumiWord *getWordFromID(size_t id);

    static string convertUTF8ToEUCJP(const string &aEUCJP);
    static string convertEUCJPToUTF8(const string &aUTF8);  
};

#endif
