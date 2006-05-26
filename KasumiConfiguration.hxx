/*********************************************************************
 * 
 * KasumiConfiguration.hxx
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

#ifndef __KASUMI_CONFIGURATION_HXX__
#define __KASUMI_CONFIGURATION_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include "KasumiException.hxx"

class KasumiConfiguration{
private:
  map<string, string> config;
  string ConfFileName;
  
  void loadDefaultProperties() throw(KasumiException);
  void loadConfigurationFile() throw (KasumiException);
  void loadConfigurationFromArgument(int argc, char *argv[])
    throw(KasumiException);
  void saveConfiguration() throw (KasumiException);

  void setPropertyValue(const string &name, const string &value);
  void checkValidity() throw(KasumiException);
public:
  KasumiConfiguration(int argc, char *argv[]) throw (KasumiException);
  ~KasumiConfiguration();
  string getPropertyValue(const string &name);
  int getPropertyValueByInt(const string &name);  
  bool getPropertyValueByBool(const string &name);
};

bool isValidShortcutKey(const string &key);

#endif
