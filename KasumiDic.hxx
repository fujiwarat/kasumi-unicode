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

#define ERROR 0
#define VALID 1

using namespace std;

class KasumiDic{
private:
  string DicFileName;
  vector<KasumiWord*> WordList; // manage words with id
  vector<KasumiDicEventListener*> EventListeners;
  
  void load() throw (KasumiDicExaminationException);
public:
  KasumiDic(const string aDicFileName) throw(KasumiDicExaminationException);
  void store() throw(KasumiDicStoreException);
  int appendWord(KasumiWord *word); // returns this word's ID
  void removeWord(size_t id) throw(KasumiOutOfBoundException);

  void registerEventListener(KasumiDicEventListener *listener);

  KasumiWord *getWordWithID(size_t id) throw(KasumiOutOfBoundException);
  int getUpperBoundOfWordID();
};

#endif
