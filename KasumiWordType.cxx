/*********************************************************************
 * 
 * KasumiWordType.cxx
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

#include "KasumiWordType.hxx"
#include "KasumiWord.hxx"
#include "intl.h"

// static members of KasumiWordType
WordTypeList KasumiWordType::_lWordTypes;
int KasumiWordType::comp_seed = 0;

void KasumiWordType::initWordTypeList()
{
    KasumiWordType::addNewWordType(_("Noun"), _("General Noun"), "#T35");
    KasumiWordType::addNewWordType(_("Noun"), _("followed by NA, SA and SURU"), "#T00");
    KasumiWordType::addNewWordType(_("Noun"), _("followed by NA and SA"), "#T05");
    KasumiWordType::addNewWordType(_("Noun"), _("followed by NA and SURU"), "#T10");
    KasumiWordType::addNewWordType(_("Noun"), _("followed by NA"), "#T15");
    KasumiWordType::addNewWordType(_("Noun"), _("followed by SURU"), "#T30");
    KasumiWordType::addNewWordType(_("Proper Noun"), _("Name of people"), "#JN");
    KasumiWordType::addNewWordType(_("Proper Noun"), _("Geographic Name"), "#CN");
    KasumiWordType::addNewWordType(_("Proper Noun"), _("Corporate Name"), "#KK");
    KasumiWordType::addNewWordType(_("Numeral"), "", "#NN");
    KasumiWordType::addNewWordType(_("Adjective (Keiyoushi)"), "", "#KY");
    KasumiWordType::addNewWordType(_("Adverb"), _("General Adverb"), "#F14");
    KasumiWordType::addNewWordType(_("Adverb"), _("followed by TO and TARU"), "#F02");
    KasumiWordType::addNewWordType(_("Adverb"), _("followed by TO and SURU"), "#F04");
    KasumiWordType::addNewWordType(_("Adverb"), _("followed by TO"), "#F06");
    KasumiWordType::addNewWordType(_("Adverb"), _("followed by SURU"), "#F12");
    KasumiWordType::addNewWordType(_("Interjection"), "", "#CJ");
    KasumiWordType::addNewWordType(_("Adnominal adjunct"), "", "#RT");
    KasumiWordType::addNewWordType(_("Single Kanji Character"), "", "#KJ");
    KasumiWordType::addNewWordType(_("Verb"), _("Ka 5"), "#K5");
    KasumiWordType::addNewWordType(_("Verb"), _("Ga 5"), "#G5");
    KasumiWordType::addNewWordType(_("Verb"), _("Sa 5"), "#S5");
    KasumiWordType::addNewWordType(_("Verb"), _("Ta 5"), "#T5");
    KasumiWordType::addNewWordType(_("Verb"), _("Na 5"), "#N5");
    KasumiWordType::addNewWordType(_("Verb"), _("Ba 5"), "#B5");
    KasumiWordType::addNewWordType(_("Verb"), _("Ma 5"), "#M5");
    KasumiWordType::addNewWordType(_("Verb"), _("Ra 5"), "#R5");
    KasumiWordType::addNewWordType(_("Verb"), _("Wa 5"), "#W5");
    KasumiWordType::addNewWordType(_("Verb*"), _("Ka 5"), "#K5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Ga 5"), "#G5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Sa 5"), "#S5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Ta 5"), "#T5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Na 5"), "#N5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Ba 5"), "#B5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Ma 5"), "#M5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Ra 5"), "#R5r");
    KasumiWordType::addNewWordType(_("Verb*"), _("Wa 5"), "#W5r");
}

KasumiWordType::KasumiWordType(string sCategory, string sPos, string sCannaTab)
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
    if(msPos.size() != 0)
	return msCategory + " - " + msPos;
    else
	return msCategory;
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
