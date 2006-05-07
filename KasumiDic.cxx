#include "KasumiDic.hxx"
#include "KasumiWord.hxx"
//#include "KasumiString.hxx"
#include "KasumiException.hxx"
#include "KasumiConfiguration.hxx"
extern "C"{  // ad-hoc solution for a defect of Anthy
#include "anthy/dicutil.h"
}
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define OptionOutput( Word, OptionName ) (string(OptionName) + " = " + (Word->getOption(OptionName) ? "y" : "n"))
#define BUFFER_SIZE (255)

KasumiDic::KasumiDic(KasumiConfiguration *conf)
  throw(KasumiException){

  try{
    load(conf);
  }catch(KasumiException e){
    throw e;
  }
}

void KasumiDic::load(KasumiConfiguration *conf)
    throw(KasumiException){

  const int FREQ_LBOUND = conf->getPropertyValueByInt("MinFrequency");  
  const int FREQ_UBOUND = conf->getPropertyValueByInt("MaxFrequency");

  try{
      if(anthy_priv_dic_select_first_entry() == -1) {
	  string message = string("Failed to read private dictionary. This problem might be a problem of Anthy.\n");
	  throw new KasumiException(message, STDERR, KILL);
      }

      char sound[BUFFER_SIZE], wt[BUFFER_SIZE], spelling[BUFFER_SIZE];
      int freq;

      do{
	  if (anthy_priv_dic_get_index(sound, BUFFER_SIZE) &&
	      anthy_priv_dic_get_wtype(wt, BUFFER_SIZE) &&
	      anthy_priv_dic_get_word(spelling, BUFFER_SIZE)) {
	      freq = anthy_priv_dic_get_freq();

	      // corret frequency value crossing the bounds
	      if(FREQ_LBOUND > freq)
		  freq = FREQ_LBOUND;
	      if(FREQ_UBOUND < freq)
		  freq = FREQ_UBOUND;
	      
	      KasumiWord *newWord = KasumiWord::createNewWord(conf);
	      
	      newWord->setSound(string(sound));
	      newWord->setSpelling(string(spelling));
	      newWord->setFrequency(freq);
	      newWord->setWordType(KasumiWordType::getWordTypeFromCannaTab(string(wt)));
	      
	      appendWord(newWord);
	  }
      }while(anthy_priv_dic_select_next_entry() == 0);
  }catch(KasumiException e){
      throw e;
  }
}

void KasumiDic::appendWord(KasumiWord *word){
  // check duplication
  list<KasumiWord*>::iterator p = mWordList.begin();
  while(p != mWordList.end() )
  {
      if((*p)->getID() == word->getID())
	  return; // nothing to do
      p++;
  }

  mWordList.push_back(word);
  
  for(size_t i=0;i<EventListeners.size();i++){
      EventListeners[i]->appendedWord(word);
  }
}

void KasumiDic::removeWord(size_t id)
{
    int flag = 0;

    list<KasumiWord*>::iterator p = mWordList.begin();
    while(p != mWordList.end() )
    {
	if((*p)->getID() == id)
	{
	    mWordList.erase(p);
	    free(*p);
	    flag = 1;
	    break;
	}
	p++;
    }

    if(flag)
	for(size_t i=0;i<EventListeners.size();i++){
	    EventListeners[i]->removedWord(id);
	}
}

void KasumiDic::modifyWord(KasumiWord *word)
{
  for(size_t i=0;i<EventListeners.size();i++){
    EventListeners[i]->modifiedWord(word);
  }
}

void KasumiDic::store()
    throw(KasumiException)
{
    list<KasumiWord*>::iterator p = mWordList.begin();

    anthy_priv_dic_delete();

    while( p != mWordList.end() )
    {
	string spelling = (*p)->getSpelling();
	string sound = (*p)->getSound();
	string wt = (*p)->getWordType()->getCannaTab();
	int freq = (*p)->getFrequency();

	int ret = anthy_priv_dic_add_entry(sound.c_str(),
					   spelling.c_str(),
					   wt.c_str(),
					   freq);

	if (ret == -1)
	{
	    throw KasumiException(string("Failed to register") + sound,
				  ERR_DIALOG,
				  KILL);
	}

	p++;
    }
}

void KasumiDic::registerEventListener(KasumiDicEventListener *listener){
  int i,size;

  size = EventListeners.size();

  // if the listener have been already registered, nothing to do
  // assuring no duplication
  for(i=0;i<size;i++){
    if(EventListeners[i] == listener){
      return;
    }
  }
  
  EventListeners.push_back(listener);  
}

void KasumiDic::removeEventListener(KasumiDicEventListener *listener){
  vector<KasumiDicEventListener*>::iterator i;
  KasumiDicEventListener *p;

  for(i=EventListeners.begin();i!=EventListeners.end();i++){
    p = *i;
    if(p == listener){
      EventListeners.erase(i);
      return;
    }
  }
}
/*
// for debug
// 
// % g++ -g KasumiDic.cxx KasumiWord.cxx KasumiException.cxx KasumiConfiguration.cxx KasumiWordType.cxx KasumiString.cxx `pkg-config --libs --cflags anthy gtk+-2.0`
void output(KasumiDic *dic)
{
    list<KasumiWord*>::iterator p = dic->firstWordIter();
    while(p != dic->endWordIter()){
	cout << (*p)->getID() << " " << (*p)->getSpelling() << endl;
	p++;
    }
}

int main(int argc, char *argv[])
{
    anthy_dic_util_init();

    KasumiConfiguration *conf = new KasumiConfiguration(argc, argv);
    KasumiDic *dic = new KasumiDic(conf);
    KasumiWord *word = KasumiWord::createNewWord(conf);
    word->setSpellingByUTF8("テスト");
    word->setSoundByUTF8("てすと");
    dic->appendWord(word);
    dic->appendWord(word);
    output(dic);

//    dic->removeWord(1);
//    output(dic);
}

*/
