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
#define EUCJP_NASETSUZOKU "\xa4\xca\xc0\xdc\xc2\xb3"
#define EUCJP_GOKANNNOMIDEBUNNSETSU "\xb8\xec\xb4\xb4\xa4\xce\xa4\xdf\xa4\xc7\xca\xb8\xc0\xe1"
#define EUCJP_SASETSUZOKU "\xa4\xb5\xc0\xdc\xc2\xb3"
#define EUCJP_SURUSETSUZOKU "\xa4\xb9\xa4\xeb\xc0\xdc\xc2\xb3"
#define EUCJP_KAKUJOSHISETSUZOKU "\xb3\xca\xbd\xf5\xbb\xec\xc0\xdc\xc2\xb3"
#define EUCJP_TOSETSUZOKU "\xa4\xc8\xc0\xdc\xc2\xb3"
#define EUCJP_TARUSETSUZOKU "\xa4\xbf\xa4\xeb\xc0\xdc\xc2\xb3"

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
  int getFrequency();
  string getSpelling();
  string getKey();  
  string getVal();
};

#endif
