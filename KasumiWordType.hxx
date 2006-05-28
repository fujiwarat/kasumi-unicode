/*********************************************************************
 * 
 * KasumiWordType.hxx
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
    int mnComp;
    static int comp_seed;

    KasumiWordType(string sPos, string sCategory, string sCannaTab);

    static WordTypeList _lWordTypes;
public:
    static void initWordTypeList();

    string getPos(){ return msPos; };
    string getCategory(){ return msCategory; };
    string getCannaTab(){ return msCannaTab; };
    string getUIString();

    static WordTypeList::iterator beginWordTypeList(){ return _lWordTypes.begin(); };
    static WordTypeList::iterator endWordTypeList(){ return _lWordTypes.end(); };

    static void addNewWordType(string sPos, string sCategory, string sCannaTab);
    static KasumiWordType* getWordTypeFromCannaTab(string sCannaTab);
    static KasumiWordType* getWordTypeFromPos(string sPos);

    int comp(KasumiWordType *op){ return op ? mnComp - op->mnComp : -1; }; // for sort function
    bool equal(KasumiWordType *op){ return comp(op) == 0 ? true : false; };
};

#endif
