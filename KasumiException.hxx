#ifndef __KASUMI_EXCEPTION_HXX__
#define __KASUMI_EXCEPTION_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

class KasumiDicExaminationException{
private:
  int line;
  string message;
public:
  KasumiDicExaminationException(string aMessage, int aLine){
    line = aLine;
    message = aMessage;
  }
  string getMessage() { return message; }
  int getLine() { return line; }
};

class KasumiDicStoreException{
private:
  string message;
public:
  KasumiDicStoreException(string aMessage){
    message = aMessage;
  }
  string getMessage() { return message; }
};

class KasumiInvalidWordClassNameException{
private:
  string message;
public:
  KasumiInvalidWordClassNameException(string aMessage){
    message = aMessage;
  }
  string getMessage() { return message; }
};

class KasumiOutOfBoundException{
private:
  string message;
public:
  KasumiOutOfBoundException(string aMessage){
    message = aMessage;
  }
  string getMessage() { return message; }
};

class KasumiConfigurationLoadException{
private:
  string message;
public:
  KasumiConfigurationLoadException(string aMessage){
    message = aMessage;
  }
  string getMessage() { return message; }
};

class KasumiConfigurationSaveException{
private:
  string message;
public:
  KasumiConfigurationSaveException(string aMessage){
    message = aMessage;
  }
  string getMessage() { return message; }
};

class KasumiInvalidCharacterForSoundException{
private:
  string InvalidString;
  string InvalidCharacter;
public:
  KasumiInvalidCharacterForSoundException(string aInvalidStr, string aInvalidChar){
    InvalidString = aInvalidStr;
    InvalidCharacter = aInvalidChar;
  }
  string getInvalidString() { return InvalidString; }
  string getInvalidCharacter() { return InvalidCharacter; }
};


#endif
