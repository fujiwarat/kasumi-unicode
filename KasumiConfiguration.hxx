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
  
  void loadConfiguration() throw (KasumiConfigurationLoadException);
  void saveConfiguration() throw (KasumiConfigurationSaveException);
public:
  KasumiConfiguration() throw (KasumiConfigurationLoadException);
  ~KasumiConfiguration();
  void setConfiguration(const string &name, const string &value);
  string getConfiguration(const string &name);
};

#endif
