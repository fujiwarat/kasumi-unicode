#ifndef __KASUMI_DIC_EVENT_LISTENER_HXX__
#define __KASUMI_DIC_EVENT_LISTENER_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KasumiWord.hxx"

class KasumiDicEventListener{
private:
public:
  virtual void removedWord(size_t id) = 0;
  virtual void appendedWord(KasumiWord *word) = 0;
  virtual void modifiedWord(KasumiWord *word) = 0;
};

#endif
