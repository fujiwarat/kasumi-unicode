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
