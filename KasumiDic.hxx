#ifndef __KASUMI_DIC_HXX__
#define __KASUMI_DIC_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>
#include <list>
#include "KasumiWord.hxx"
#include "KasumiException.hxx"
#include "KasumiDicEventListener.hxx"
#include "KasumiWordEventListener.hxx"
#include "KasumiConfiguration.hxx"

#define ERROR 0
#define VALID 1

using namespace std;

class KasumiDic : public KasumiWordEventListener{
private:
  list<KasumiWord*> mWordList;
  vector<KasumiDicEventListener*> EventListeners;
  
  void load(KasumiConfiguration *conf) throw (KasumiException);
public:
    KasumiDic(KasumiConfiguration *conf)
	throw(KasumiException);
    ~KasumiDic();
    void store() throw(KasumiException);
    void appendWord(KasumiWord *word); // returns this word's ID
    void removeWord(size_t id);

    void registerEventListener(KasumiDicEventListener *listener);
    void removeEventListener(KasumiDicEventListener *listener);

    list<KasumiWord*>::iterator firstWordIter(){ return mWordList.begin(); };
    list<KasumiWord*>::iterator endWordIter(){ return mWordList.end(); };

    void changedFrequency(KasumiWord *word);
    void changedSpelling(KasumiWord *word);
    void changedSound(KasumiWord *word);
    void changedWordType(KasumiWord *word);

//  KasumiWord *getWordWithID(size_t id) throw(KasumiException);
//  int getUpperBoundOfWordID();
};

#endif
