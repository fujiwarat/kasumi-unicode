#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include "KasumiDic.hxx"
#include "KasumiMainWindow.hxx"
#include "KasumiAddWindow.hxx"
#include "KasumiConfiguration.hxx"
#include "KasumiException.hxx"
#include "intl.h"

using namespace std;

void showHelp(){
  cout << "Usage: kasumi [option]" << endl;
  cout << "Option:" << endl;
  cout << "  -h --help        Show this message." << endl;
  cout << "  -v --versoin     Show Kasumi's version and copyright" << endl;;
  cout << "                   infomation" << endl;
  cout << "  -m --main        Run Kasumi in Manage mode." << endl;
  cout << "                   You may abbreviate this option." << endl;
  cout << "  -a --add         Run Kasumi in Add mode." << endl;
  cout << "  --sound val      Set defalut sound entry (only in Add mode)" << endl;
  cout << "  --spelling val   Set defalut spelling entry (only in Add mode)" << endl;
  cout << "  --wordclass val  Set defalut word class entry (only in Add mode)" << endl;
  cout << endl;
}

void showVersion(){
  cout << "kasumi " << VERSION << endl;
  cout << "Copyright (C) 2004 Takashi Nakamoto.\n";
  cout << "This program comes with NO WARRANTY, to the extent permitted " \
          "by law. \nYou may redistribute it under the terms of the GNU " \
          "General Public License; \nsee the file named COPYING for " \
          "details.";
  cout << endl;
}

int main(int argc, char *argv[])
{
  gtk_init(&argc, &argv);

  // for gettext
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);

  try{
    KasumiConfiguration *conf = new KasumiConfiguration(argc, argv);

    KasumiDic *dic = new KasumiDic(conf->getPropertyValue("DictionaryPath"),conf);

    string startupMode = conf->getPropertyValue("StartupMode");
    if(startupMode == string("HELP")){
      showHelp();
    }else if(startupMode == string("VERSION")){
      showVersion();
    }else if(startupMode == string("MAIN")){
      KasumiMainWindow window = KasumiMainWindow(dic,conf);
      gtk_main();
    }else if(startupMode == string("ADD")){
      KasumiAddWindow window = KasumiAddWindow(dic,conf);
      gtk_main();
    }
  }catch(KasumiConfigurationLoadException e){
    cout << "line " << e.getLine() << ": " << e.getMessage() << endl;
    exit(1);
  }catch(KasumiDicExaminationException e){
    cout << "line " << e.getLine() << ": " << e.getMessage() << endl;
    exit(1);
  }
}

