/*********************************************************************
 * 
 * KasumiWord.hxx
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
#include "KasumiWordEventListener.hxx"
#include <iconv.h>
#include <vector>

using namespace std;

class KasumiWord{
private:
    bool IsEUCJP;
    string Sound; // corresponding "Yomi" in private-dic.src
    string Sound_UTF8;
    int Frequency; // corresponding "Hindo"
    string Spelling; // corresponding "Tango"
    string Spelling_UTF8;
    unsigned int id;
    vector<KasumiWordEventListener*> EventListeners;

    KasumiWordType *mWordType;

    static iconv_t IconvUTF8_To_EUCJP;
    static iconv_t IconvEUCJP_To_UTF8;
  
    // confirms if the given sound consists of only Hiragana
    // if no invalid character, returns empty string
    static string extractInvalidCharacterFromSound(string soundByUTF8);

    static unsigned int id_generator;
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

    void setFrequency(int aFrequency);
    int getFrequency(){ return Frequency; };

    void setSpelling(const string &aSpelling);
    void setSpellingByUTF8(const string &aSpelling);  
    string getSpelling(){ return Spelling; };
    string getSpellingByUTF8(){ return Spelling_UTF8; };  

    void setWordType(KasumiWordType *aType);
    KasumiWordType *getWordType() { return mWordType; };
    string getWordTypeUIString(); // UTF8

    unsigned int getID(){ return id; };

    void registerEventListener(KasumiWordEventListener *listener);
    void removeEventListener(KasumiWordEventListener *listener);

    static KasumiWord *getWordFromID(unsigned int id);

    static string convertUTF8ToEUCJP(const string &aEUCJP);
    static string convertEUCJPToUTF8(const string &aUTF8);  
};

#endif
