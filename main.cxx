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
  cout << "                   information" << endl;
  cout << "  -m --manage      Run Kasumi in Manage mode." << endl;
  cout << "                   You may abbreviate this option." << endl;
  cout << "  -a --add         Run Kasumi in Add mode." << endl;
  cout << "  --sound val      Set default sound entry (only in Add mode)" << endl;
  cout << "  --spelling val   Set default spelling entry (only in Add mode)" << endl;
  cout << "  --wordclass val  Set default word class entry (only in Add mode)" << endl;
  cout << "  -x val           Set default horizontal window position" << endl;
  cout << "  -y val           Set default vertical window position" << endl;
  cout << endl;
}

void showVersion(){
  cout << "kasumi " << VERSION << ": a personal dictionary manager for Anthy" << endl;
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
    }else if(startupMode == string("MANAGE")){
      KasumiMainWindow *window = new KasumiMainWindow(dic,conf);
      gtk_main();
    }else if(startupMode == string("ADD")){
      KasumiAddWindow *window = new KasumiAddWindow(dic,conf);
      gtk_main();
    }
  }catch(KasumiConfigurationLoadException e){
    cout << e.getMessage() << endl;
    exit(1);
  }catch(KasumiDicExaminationException e){
    cout << "line " << e.getLine() << ": " << e.getMessage() << endl;
    exit(1);
  }
}

