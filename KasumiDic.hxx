#ifndef __KASUMI_DIC_HXX__
#define __KASUMI_DIC_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>
#include "KasumiWord.hxx"
#include "KasumiException.hxx"
#include "KasumiDicEventListener.hxx"
#include "KasumiConfiguration.hxx"

#define ERROR 0
#define VALID 1

using namespace std;

class KasumiDic{
private:
  string DicFileName;
  vector<KasumiWord*> WordList; // manage words with id
  vector<KasumiDicEventListener*> EventListeners;
  
  void load(KasumiConfiguration *conf) throw (KasumiDicExaminationException);
public:
  KasumiDic(const string aDicFileName, KasumiConfiguration *conf)
    throw(KasumiDicExaminationException);
  void store() throw(KasumiDicStoreException);
  int appendWord(KasumiWord *word); // returns this word's ID
  void removeWord(size_t id) throw(KasumiOutOfBoundException);
  void modifyWord(size_t id) throw(KasumiOutOfBoundException);

  void registerEventListener(KasumiDicEventListener *listener);
  void removeEventListener(KasumiDicEventListener *listener);

  KasumiWord *getWordWithID(size_t id) throw(KasumiOutOfBoundException);
  int getUpperBoundOfWordID();
};

#endif
