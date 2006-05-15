#ifndef __KASUMI_WORD_EVENT_LISTENER_HXX__
#define __KASUMI_WORD_EVENT_LISTENER_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KasumiWord.hxx"

class KasumiWord;

class KasumiWordEventListener{
private:
public:
    virtual void changedFrequency(KasumiWord *word) = 0;
    virtual void changedSpelling(KasumiWord *word) = 0;
    virtual void changedSound(KasumiWord *word) = 0;
    virtual void changedWordType(KasumiWord *word) = 0;
};

#endif
