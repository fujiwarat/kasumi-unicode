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
    KasumiWordType::addNewWordType("動詞", "カ行5段", "#K5");
    KasumiWordType::addNewWordType("動詞", "カ行5段(連用形が名詞)", "#K5r");
    KasumiWordType::addNewWordType("動詞", "ガ行5段", "#G5");
    KasumiWordType::addNewWordType("動詞", "ガ行5段(連用形が名詞)", "#G5r");
    KasumiWordType::addNewWordType("動詞", "サ行5段", "#S5");
    KasumiWordType::addNewWordType("動詞", "サ行5段(連用形が名詞)", "#S5r");
    KasumiWordType::addNewWordType("動詞", "タ行5段", "#T5");
    KasumiWordType::addNewWordType("動詞", "タ行5段(連用形が名詞)", "#T5r");
    KasumiWordType::addNewWordType("動詞", "ナ行5段", "#N5");
    KasumiWordType::addNewWordType("動詞", "ナ行5段(連用形が名詞)", "#N5r");
    KasumiWordType::addNewWordType("動詞", "バ行5段", "#B5");
    KasumiWordType::addNewWordType("動詞", "バ行5段(連用形が名詞)", "#B5r");
    KasumiWordType::addNewWordType("動詞", "マ行5段", "#M5");
    KasumiWordType::addNewWordType("動詞", "マ行5段(連用形が名詞)", "#M5r");
    KasumiWordType::addNewWordType("動詞", "ラ行5段", "#R5");
    KasumiWordType::addNewWordType("動詞", "ラ行5段(連用形が名詞)", "#R5r");
    KasumiWordType::addNewWordType("動詞", "ワ行5段", "#W5");
    KasumiWordType::addNewWordType("動詞", "ワ行5段(連用形が名詞)", "#W5r");

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
    // return the completely same word type
    WordTypeList::iterator p = _lWordTypes.begin();
    while(p != _lWordTypes.end()){
	if( (*p)->getCannaTab() == sCannaTab )
	    return (*p);
	p++;
    }

    // return the alternative word type whose first character is equal to
    // specified one's
    char first_char = sCannaTab.c_str()[0];
    p = _lWordTypes.begin();
    while(p != _lWordTypes.end()){
	if( first_char == (*p)->getCannaTab().c_str()[0] )
	    return (*p);
	p++;
    }

    // return the first word type as a fallback
    return *(_lWordTypes.begin());
}

KasumiWordType* KasumiWordType::getWordTypeFromPos(string sPos)
{
    WordTypeList::iterator p = _lWordTypes.begin();
    while(p != _lWordTypes.end()){
	if( (*p)->getPos() == sPos )
	    return (*p);
	p++;
    }

    // return the first word type as a fallback
    return *(_lWordTypes.begin());
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
