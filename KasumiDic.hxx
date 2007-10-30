/*********************************************************************
 * 
 * KasumiDic.hxx
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
    void removeWord(unsigned int id);

    void registerEventListener(KasumiDicEventListener *listener);
    void removeEventListener(KasumiDicEventListener *listener);

    list<KasumiWord*>::iterator firstWordIter(){ return mWordList.begin(); };
    list<KasumiWord*>::iterator endWordIter(){ return mWordList.end(); };

    void changedFrequency(KasumiWord *word);
    void changedSpelling(KasumiWord *word);
    void changedSound(KasumiWord *word);
    void changedWordType(KasumiWord *word);

//  KasumiWord *getWordWithID(unsigned int id) throw(KasumiException);
//  int getUpperBoundOfWordID();
};

#endif
