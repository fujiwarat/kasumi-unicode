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
  
  void loadDefaultProperties();
  void loadConfigurationFile() throw (KasumiConfigurationLoadException);
  void loadConfigurationFromArgument(int argc, char *argv[]);
  void saveConfiguration() throw (KasumiConfigurationSaveException);

  void setPropertyValue(const string &name, const string &value);
public:
  KasumiConfiguration(int argc, char *argv[]) throw (KasumiConfigurationLoadException);
  ~KasumiConfiguration();
  string getPropertyValue(const string &name);
  int getPropertyValueByInt(const string &name);  
};

#endif
