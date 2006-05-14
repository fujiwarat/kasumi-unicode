#include "KasumiWordType.hxx"
#include "KasumiWord.hxx"
#include <iostream> // for debug

// static members of KasumiWordType
WordTypeList KasumiWordType::_lWordTypes;
int KasumiWordType::comp_seed = 0;

void KasumiWordType::initWordTypeList()
{
    KasumiWordType::addNewWordType("名詞", "一般名詞", "#T35");
    KasumiWordType::addNewWordType("名詞", "人名", "#JNM");
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
}

KasumiWordType::KasumiWordType(string sPos, string sCategory, string sCannaTab)
{
    msPos = sPos;
    msCategory = sCategory;
    msCannaTab = sCannaTab;
    mnComp = comp_seed++;
}

void KasumiWordType::addNewWordType(string sPos, string sCategory, string sCannaTab)
{
    _lWordTypes.push_back(new KasumiWordType(sPos, sCategory, sCannaTab));
}

KasumiWordType* KasumiWordType::getWordTypeFromCannaTab(string sCannaTab)
{
    WordTypeList::iterator p = _lWordTypes.begin();
    while(p != _lWordTypes.end()){
	if( (*p)->getCannaTab() == sCannaTab )
	    return (*p);
	p++;
    }

    return NULL;
}

KasumiWordType* KasumiWordType::getWordTypeFromPos(string sPos)
{
    WordTypeList::iterator p = _lWordTypes.begin();
    while(p != _lWordTypes.end()){
	if( (*p)->getPos() == sPos )
	    return (*p);
	p++;
    }

    return NULL;
}

string KasumiWordType::getUIString()
{
    if(msCategory.size() != 0)
	return msPos + " - " + msCategory;
    else
	return msPos;
}

/*
// for debug
int main()
{
    KasumiWordType::addNewWordType("名詞", "一般名詞", "#T35");
    KasumiWordType::addNewWordType("名詞", "人名", "#JN");
    KasumiWordType::addNewWordType("名詞", "地名", "#CN");
}
*/
