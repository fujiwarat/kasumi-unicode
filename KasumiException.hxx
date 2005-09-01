#ifndef __KASUMI_EXCEPTION_HXX__
#define __KASUMI_EXCEPTION_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

enum _output {STDOUT, STDERR, ERR_DIALOG, WARNING_DIALOG};
typedef _output Output;

enum _disposal {KILL, QUIT, ALERT_ONLY};
typedef _disposal Disposal;

class KasumiException{
private:
  string message;
  Output output;
  Disposal disposal;
public:
  KasumiException(string aMessage, Output aOutput, Disposal aDisposal){
    message = aMessage;
    output = aOutput;
    disposal = aDisposal;
  }
  string getMessage() { return message; }
  Output getOutput() { return output; }
  Disposal getDisposal() { return disposal; }
};

void handleException(KasumiException e);

class KasumiOutOfBoundException{
private:
  string message;
public:
  KasumiOutOfBoundException(string aMessage){
    message = aMessage;
  }
  string getMessage() { return message; }
};

#endif
