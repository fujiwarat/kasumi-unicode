#ifndef __KASUMI_DIC_EVENT_LISTENER_HXX__
#define __KASUMI_DIC_EVENT_LISTENER_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KasumiDic.hxx"

class KasumiDicEventListener{
private:
public:
  virtual void removedWord(int id) = 0;
  virtual void appendedWord(int id) = 0;
  virtual void modifiedWord(int id) = 0;
};

#endif
