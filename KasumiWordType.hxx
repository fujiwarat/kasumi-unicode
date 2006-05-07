#ifndef __KASUMI_WORD_TYPE_HXX__
#define __KASUMI_WORD_TYPE_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <list>

using namespace std;

class KasumiWordType; // pre-declaration

typedef list<KasumiWordType*> WordTypeList;

class KasumiWordType
{
private:
    string msPos; // part of speech such as noun, verb and so on
    string msCategory; // subcategory of Pos
    string msCannaTab;

    KasumiWordType(string sPos, string sCategory, string sCannaTab);

    static WordTypeList _lWordTypes;
public:
    static void initWordTypeList();

    string getPos(){ return msPos; };
    string getCategory(){ return msCategory; };
    string getCannaTab(){ return msCannaTab; };
    string getUIString(){ return msPos + " - " + msCategory; };

    static WordTypeList::iterator getBeginIteratorWordTypeList(){ return _lWordTypes.begin(); };
    static WordTypeList::iterator getEndIteratorWordTypeList(){ return _lWordTypes.end(); };

    static void addNewWordType(string sPos, string sCategory, string sCannaTab);
    static KasumiWordType* getWordTypeFromCannaTab(string sCannaTab);
    static KasumiWordType* getWordTypeFromPos(string sPos);
};

#endif
