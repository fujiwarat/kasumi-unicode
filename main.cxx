#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <getopt.h> /* for getopt_long() */
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

int main(int argc, char *argv[])
{
  int c;
  char *home;
  enum {MAIN,ADD} mode = MAIN;
  string dic_filename;
  
  gtk_init(&argc, &argv);

  // for gettext
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);

  int option_index = 0;
  static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"add", no_argument, NULL, 'a'},
    {"main", no_argument, NULL, 'm'},
    {0,0,0,0}
  };
  
  while(1){
    c = getopt_long(argc, argv, "hvam", long_options, &option_index);
    if(c == -1) break; // no more argument

    switch(c){
    case 'h':
      cout << "Usage: kasumi [option]" << endl;
      cout << "Option:" << endl;
      cout << "  -h --help       Show this message." << endl;
      cout << "  -v --versoin    Show Kasumi's version and copyright" << endl;;
      cout << "                  infomation" << endl;
      cout << "  -a --add        Run Kasumi with Add window." << endl;
      cout << "  -m --main       Run Kasumi with default window." << endl;
      cout << "                  You may abbreviate this option." << endl;
      cout << endl;
      exit(0);
      break;
    case 'v':
      cout << "kasumi " << VERSION << endl;
      cout << "Copyright (C) 2004 Takashi Nakamoto.\n";
      cout << "This program comes with NO WARRANTY, to the extent permitted" \
              "by law. \nYou may redistribute it under the terms of the GNU " \
              "General Public License; \nsee the file named COPYING for " \
              "details.";
      cout << endl;
      exit(0);
      break;
    case 'a':
      mode = ADD;
      break;
    case 'm':
      mode = MAIN;
      break;
    case '?':
      cout << "Argument error." << endl;
      exit(1);
      break;
    }
  }

  if(optind < argc){
    cout << "non-option ARGV-elements: ";
    while (optind < argc)
      cout << argv[optind++];
    cout << endl;
  }

  /* obtain anthy dictionary file name */
  home = getenv("HOME");
  if(home == NULL){
    cout << "Cannot find $HOME environment variable." << endl;
    exit(1);
  }

  dic_filename = string(home) + "/.anthy/private-dic.src.tmp";
  free(home);

  try{
    KasumiConfiguration *conf = new KasumiConfiguration();

    KasumiDic *dic = new KasumiDic(dic_filename);
  
    if(mode == MAIN){
      KasumiMainWindow window = KasumiMainWindow(dic,conf);
      gtk_main();
    }else if(mode == ADD){
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

