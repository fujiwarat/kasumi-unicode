/*********************************************************************
 * 
 * KasumiDic.cxx
 * 
 * Kasumi - a management tool for a private dictionary of anthy
 * 
 * Copyright (C) 2004-2006 Takashi Nakamoto
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 * 
*********************************************************************/

#include "KasumiDic.hxx"
#include "KasumiWord.hxx"
#include "KasumiString.hxx"
#include "KasumiException.hxx"
#include "KasumiConfiguration.hxx"
extern "C"{  // ad-hoc solution for a defect of Anthy
#include <anthy/dicutil.h>
}
#include <anthy/anthy.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

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

KasumiDic::~KasumiDic()
{
    list<KasumiWord*>::iterator p;

    while(!mWordList.empty())
    {
	p =  mWordList.begin();
	mWordList.pop_front();
	delete (*p);
    }
}

void KasumiDic::load(KasumiConfiguration *conf)
    throw(KasumiException){

  const int FREQ_LBOUND = conf->getPropertyValueByInt("MinFrequency");  
  const int FREQ_UBOUND = conf->getPropertyValueByInt("MaxFrequency");

  int anthy_version = atoi(anthy_get_version_string());
  assert(anthy_version != 0);

  try{
      if(anthy_priv_dic_select_first_entry() == -1){
	  // no word
	  return;
      }
      else if(anthy_priv_dic_select_first_entry() == -3)
      {
	  if(anthy_version >= 7716)
	  {
	      // do not throw exception in the case that this doesn't fail to
	      // read the private dictionary but the dictionary containes
	      // no word. This case happens if the version of anthy is less
	      // than 7714.
	      string message = string("Failed to read private dictionary. This problem might be a problem of Anthy.\n");
	      throw KasumiException(message, STDERR, KILL);
	  }
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
	      if (anthy_version < 7710 && spelling[0] == ' ') {
		      // Measures against a defect of anthy.
		      // "anthy_priv_dic_get_word()" function returns
		      // a string whose first character is an unwated
		      // white space.
		      newWord->setSpelling(string(spelling+1));
	      } else
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

  word->registerEventListener(this);
  mWordList.push_back(word);

  // raise event  
  for(size_t i=0;i<EventListeners.size();i++){
      EventListeners[i]->appendedWord(word);
  }
}

void KasumiDic::removeWord(unsigned int id)
{
    int flag = 0;
    
    list<KasumiWord*>::iterator p = mWordList.begin();
    while(p != mWordList.end() )
    {
	if((*p)->getID() == id)
	{
	    mWordList.erase(p);
	    (*p)->removeEventListener(this);
	    free(*p);
	    flag = 1;
	    break;
	}
	p++;
    }

    // raise event  
    if(flag)
	for(size_t i=0;i<EventListeners.size();i++){
	    EventListeners[i]->removedWord(id);
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

void KasumiDic::changedFrequency(KasumiWord *word)
{
  for(size_t i=0;i<EventListeners.size();i++){
    EventListeners[i]->modifiedWord(word);
  }
}

void KasumiDic::changedSpelling(KasumiWord *word)
{
  for(size_t i=0;i<EventListeners.size();i++){
    EventListeners[i]->modifiedWord(word);
  }
}

void KasumiDic::changedSound(KasumiWord *word)
{
  for(size_t i=0;i<EventListeners.size();i++){
    EventListeners[i]->modifiedWord(word);
  }
}

void KasumiDic::changedWordType(KasumiWord *word)
{
  for(size_t i=0;i<EventListeners.size();i++){
    EventListeners[i]->modifiedWord(word);
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

    KasumiWordType::initWordTypeList();
    KasumiConfiguration *conf = new KasumiConfiguration(argc, argv);
    KasumiDic *dic = new KasumiDic(conf);
    KasumiWord *word = KasumiWord::createNewWord(conf);
    word->setSpellingByUTF8("テスト");
    word->setSoundByUTF8("てすと");
    dic->appendWord(word);
    output(dic);

    while(1);

    dic->removeWord(1);
    output(dic);
}

*/
