/*********************************************************************
 * 
 * KasumiString.hxx
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

#ifndef __KASUMI_STRING_HXX__
#define __KASUMI_STRING_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>

#define EUCJP_HINNSHI "\xc9\xca\xbb\xec"
#define EUCJP_MEISHI "\xcc\xbe\xbb\xec"
#define EUCJP_FUKUSHI "\xc9\xfb\xbb\xec"
#define EUCJP_JINNMEI "\xbf\xcd\xcc\xbe"
#define EUCJP_CHIMEI "\xc3\xcf\xcc\xbe"
#define EUCJP_KEIYOUSHI "\xb7\xc1\xcd\xc6\xbb\xec"
#define EUCJP_DOUSHI "\xc6\xb0\xbb\xec"
#define EUCJP_NASETSUZOKU "\xa4\xca\xc0\xdc\xc2\xb3"
#define EUCJP_GOKANNNOMIDEBUNNSETSU "\xb8\xec\xb4\xb4\xa4\xce\xa4\xdf\xa4\xc7\xca\xb8\xc0\xe1"
#define EUCJP_SASETSUZOKU "\xa4\xb5\xc0\xdc\xc2\xb3"
#define EUCJP_SURUSETSUZOKU "\xa4\xb9\xa4\xeb\xc0\xdc\xc2\xb3"
#define EUCJP_KAKUJOSHISETSUZOKU "\xb3\xca\xbd\xf5\xbb\xec\xc0\xdc\xc2\xb3"
#define EUCJP_TOSETSUZOKU "\xa4\xc8\xc0\xdc\xc2\xb3"
#define EUCJP_TARUSETSUZOKU "\xa4\xbf\xa4\xeb\xc0\xdc\xc2\xb3"

#define EUCJP_KATSUYOU "\xb3\xe8\xcd\xd1"
#define EUCJP_BAGYOUGODAN "\xa5\xd0\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_GAGYOUGODAN "\xa5\xac\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_KAGYOUGODAN "\xa5\xab\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_MAGYOUGODAN "\xa5\xde\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_NAGYOUGODAN "\xa5\xca\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_RAGYOUGODAN "\xa5\xe9\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_SAGYOUGODAN "\xa5\xb5\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_TAGYOUGODAN "\xa5\xbf\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_WAGYOUGODAN "\xa5\xef\xb9\xd4\xb8\xde\xc3\xca"
#define EUCJP_RENNYOUKEINOMEISHIKA "\xcf\xa2\xcd\xd1\xb7\xc1\xa4\xce\xcc\xbe\xbb\xec\xb2\xbd"

using namespace std;

class KasumiString : public string{
private:

public:
  // methods to examine one line string
  bool isCommentLine();
  bool isEmptyLine();
  bool isEntryLine();
  bool isKeyValLine();
  
  string getSound();
  unsigned int getFrequency();
  string getSpelling();
  string getKey();  
  string getVal();
};

int str2int(const string &str);
string int2str(int i);
bool isInt(const string &str);

#endif
