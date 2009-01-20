/*********************************************************************
 * 
 * KasumiWord.cxx
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

#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#include <iconv.h>
#include "KasumiWord.hxx"
#include "KasumiException.hxx"
#include "KasumiConfiguration.hxx"
#include "KasumiString.hxx" // for EUCJP_***(constant string)
#include "intl.h"

#include <iostream>

#define VECTOR_UNIT 10

using namespace std;

iconv_t KasumiWord::IconvUTF8_To_EUCJP = iconv_open("EUC-JP", "UTF-8");
iconv_t KasumiWord::IconvEUCJP_To_UTF8 = iconv_open("UTF-8", "EUC-JP");

unsigned int KasumiWord::id_generator = 0;

vector<KasumiWord*> KasumiWord::words = vector<KasumiWord*>(VECTOR_UNIT);

string KasumiWord::convertUTF8ToEUCJP(const string &aUTF8){
    char *utf8 = (char*)malloc(strlen(aUTF8.c_str())+1);
    strcpy(utf8, aUTF8.c_str());
    size_t len = strlen(utf8)+1;
    size_t len_eucjp = len*2;
    char *eucjp_buf = (char*)malloc(len_eucjp);
    char *eucjp = eucjp_buf;

    iconv(IconvUTF8_To_EUCJP, const_cast<ICONV_CONST char**>(&utf8), &len, &eucjp_buf, &len_eucjp);
    return string(eucjp);
}

string KasumiWord::convertEUCJPToUTF8(const string &aEUCJP){
    char *eucjp = (char*)malloc(strlen(aEUCJP.c_str())+1);
    strcpy(eucjp, aEUCJP.c_str());
    size_t len = strlen(eucjp)+1;
    size_t len_utf8 = len*2;
    char *utf8_buf = (char*)malloc(len_utf8);
    char *utf8 = utf8_buf;

    iconv(IconvEUCJP_To_UTF8, const_cast<ICONV_CONST char**>(&eucjp), &len, &utf8_buf, &len_utf8);
    return string(utf8);
}

string KasumiWord::extractInvalidCharacterFromSound(string soundByUTF8){
    const char *str = soundByUTF8.c_str();
    unsigned char first,second,third;
    char res[5];
    int i,size;

    size = soundByUTF8.size();

    for(i=0;i<size;i+=3){
	first = static_cast<unsigned char> (str[i]);
	second = static_cast<unsigned char> (str[i+1]);
	third = static_cast<unsigned char> (str[i+2]);

	if(first >= 0x21 && first <= 0x7e){
	    // ASCII character
	    continue;
	}else if(first <= 0x7f){
	    res[0] = str[i];
	    res[1] = 0;
	    return string(res);
	}else if(first >= 0x80 && first <= 0xc1){
	    // Even it is not encoded in UTF-8
	    string("non-UTF8 code detected");
	}else if(first >= 0xc2 && first <= 0xdf){
	    res[0] = str[i];
	    res[1] = str[i+1];
	    res[2] = 0;
	    return string(res);
	}else if(first >= 0xe0 && first <= 0xef){
	    if(first == 0xe3 && second == 0x83 && third == 0xbc){
		// dash
		continue;
	    }else if(first == 0xe3 && second == 0x80 &&
		     (third == 0x81 || third == 0x82)){
		// punctuaiton mark
		continue;
	    }else if(first == 0xe3 && second == 0x80 &&
		     (third == 0x8c || third == 0x8d)){
		// corner brackets
		continue;
	    }else if(first == 0xe3 && second == 0x80 && third == 0x9c){
		// wave dash
		continue;
	    }else if(first == 0xe3 && second == 0x81 &&
		     third >= 0x81 && third <= 0xbf){
		// hiragana from "A" to "MI"
		continue;
	    }else if(first == 0xe3 && second == 0x82 &&
		     third >= 0x80 && third <= 0x94){
		// the rest of hiragana
		continue;
	    }else if(first == 0xef && second == 0xbc &&
		     third >= 0x81 && third <= 0xa0){
		// symbol marks of two-byte characters
		continue;
	    }else if(first == 0xe3 && second == 0x82 &&
		     third == 0x9b){
		// dakuon mark
		continue;
	    }else{
		res[0] = str[i];
		res[1] = str[i+1];
		res[2] = str[i+2];
		res[3] = 0;
		return string(res);
	    }
	}else if(first >= 0xf0 && first <= 0xf4){
	    res[0] = str[i];
	    res[1] = str[i+1];
	    res[2] = str[i+2];
	    res[3] = str[i+3];
	    res[4] = 0;
	    return string(res);
	}else{
	    // Even it is not encoded in UTF-8
	    string("non-UTF8 code detected");
	}
    }

    return string();
}

KasumiWord::KasumiWord(KasumiConfiguration *conf){
    IsEUCJP = conf->getPropertyValueByBool("UseEUCJP");
    setSound(conf->getPropertyValue("DefaultSound"));
    setSpelling(conf->getPropertyValue("DefaultSpelling"));
    Frequency = conf->getPropertyValueByInt("DefaultFrequency");
    try{
	mWordType = NULL;
	setWordType(KasumiWordType::getWordTypeFromCannaTab(conf->getPropertyValue("DefaultWordType")));
    }catch(KasumiException e){
	cout << e.getMessage() << endl;
	exit(1);
    }
}

KasumiWord* KasumiWord::createNewWord(KasumiConfiguration *conf)
{
    KasumiWord *word = new KasumiWord(conf);
    word->id = id_generator++;
    if(word->id >= words.size())
	words.resize(words.size() + VECTOR_UNIT, NULL);
    KasumiWord::words[word->id] = word;
    return word;
}

void KasumiWord::setSound(const string &aSound)
    throw(KasumiException){

    if(aSound == Sound)
	return;

    string tmp = IsEUCJP ? convertEUCJPToUTF8(Sound) : aSound;
    string invalidChar = extractInvalidCharacterFromSound(tmp);

    if(invalidChar != ""){
	string message;
	message = string(_("Sound must consist of only Hiragana characters. You have entered invalid character: "));
	message += invalidChar;
	throw KasumiException(message, ERR_DIALOG, ALERT_ONLY);
    }
  
    Sound = aSound;
    Sound_UTF8 = IsEUCJP ? convertEUCJPToUTF8(Sound) : Sound;

    for(size_t i=0;i<EventListeners.size();i++){
	EventListeners[i]->changedSound(this);
    }
}

void KasumiWord::setSoundByUTF8(const string &aSound)
    throw(KasumiException){

    if(aSound == Sound_UTF8)
	return;

    string invalidChar = extractInvalidCharacterFromSound(aSound);

    if(invalidChar != ""){
	string message;
	message = string(_("Sound must consist of only Hiragana characters. You have entered invalid character: "));
	message += invalidChar;
	throw KasumiException(message, ERR_DIALOG, ALERT_ONLY);
    }
  
    Sound_UTF8 = aSound;
    Sound = IsEUCJP ? convertUTF8ToEUCJP(Sound_UTF8) : Sound_UTF8;

    for(size_t i=0;i<EventListeners.size();i++){
	EventListeners[i]->changedSound(this);
    }
}

void KasumiWord::setSpelling(const string &aSpelling){
    if(aSpelling == Spelling)
	return;

    Spelling = aSpelling;
    Spelling_UTF8 = IsEUCJP ? convertEUCJPToUTF8(Spelling) : Spelling;

    for(size_t i=0;i<EventListeners.size();i++){
	EventListeners[i]->changedSpelling(this);
    }
}

void KasumiWord::setSpellingByUTF8(const string &aSpelling){
    if(aSpelling == Spelling_UTF8)
	return;

    Spelling_UTF8 = aSpelling;
    Spelling = IsEUCJP ? convertUTF8ToEUCJP(Spelling_UTF8) : Spelling_UTF8;

    for(size_t i=0;i<EventListeners.size();i++){
	EventListeners[i]->changedSpelling(this);
    }
}

void KasumiWord::setFrequency(int aFrequency)
{
    if(aFrequency == Frequency)
	return;

    Frequency = aFrequency;

    for(size_t i=0;i<EventListeners.size();i++){
	EventListeners[i]->changedFrequency(this);
    }
}

void KasumiWord::setWordType(KasumiWordType *aType)
{
    if(aType->equal(mWordType))
	return;

    mWordType = aType;

    for(size_t i=0;i<EventListeners.size();i++){
	EventListeners[i]->changedWordType(this);
    }
}

string KasumiWord::getWordTypeUIString()
{
    if(mWordType != NULL)
	return mWordType->getUIString();
    else
	return string("");
}

KasumiWord* KasumiWord::getWordFromID(unsigned int id)
{
    return KasumiWord::words[id];
}

void KasumiWord::registerEventListener(KasumiWordEventListener *listener){
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

void KasumiWord::removeEventListener(KasumiWordEventListener *listener){
    vector<KasumiWordEventListener*>::iterator i;
    KasumiWordEventListener *p;

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
int main(int argc, char *argv[])
{
KasumiWordType::addNewWordType("名詞", "一般名詞", "#T35");
KasumiWordType::addNewWordType("名詞", "人名", "#JN");
KasumiWordType::addNewWordType("名詞", "地名", "#CN");
KasumiWordType::addNewWordType("名詞", "会社", "#KK");
KasumiWordType::addNewWordType("名詞", "する接続", "#T30");
KasumiWordType::addNewWordType("数詞", "数詞", "#NN");
KasumiWordType::addNewWordType("形容詞", "ク活用", "#KY");
KasumiWordType::addNewWordType("形容詞", "シク活用", "#KYT");
KasumiWordType::addNewWordType("形容動詞", "", "#T05");
KasumiWordType::addNewWordType("副詞", "", "#F04");
KasumiWordType::addNewWordType("感動詞", "", "#CJ");
KasumiWordType::addNewWordType("連体詞", "", "#RT");
KasumiWordType::addNewWordType("単漢字", "", "#KJ");

KasumiWord *newWord = new KasumiWord();
newWord->setSound("たかし");
newWord->setSpelling("崇志");
newWord->setFrequency(180);
newWord->setWordType(KasumiWordType::getWordTypeFromCannaTab("#T35"));
cout << newWord->getSound() << " " << newWord->getSpelling() << " ";
cout << newWord->getFrequency() << " " << endl;
cout << newWord->getWordType()->getPos() << endl;
cout << newWord->getWordType()->getCategory() << endl;
}
*/
