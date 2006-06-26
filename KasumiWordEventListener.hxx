/*********************************************************************
 * 
 * KasumiWordEventListener.hxx
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

#ifndef __KASUMI_WORD_EVENT_LISTENER_HXX__
#define __KASUMI_WORD_EVENT_LISTENER_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KasumiWord.hxx"

class KasumiWord;

class KasumiWordEventListener{
private:
public:
    virtual ~KasumiWordEventListener() {};
    virtual void changedFrequency(KasumiWord *word) = 0;
    virtual void changedSpelling(KasumiWord *word) = 0;
    virtual void changedSound(KasumiWord *word) = 0;
    virtual void changedWordType(KasumiWord *word) = 0;
};

#endif
