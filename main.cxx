#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <getopt.h> /* for getopt_long() */
#include <stdlib.h>
#include <iostream>
#include <string>
#include "KasumiDic.hxx"
#include "KasumiMainWindow.hxx"
#include "KasumiAddWindow.hxx"
#include "KasumiException.hxx"
#include "intl.h"

using namespace std;

int main(int argc, char *argv[])
{
  int c;
  char *contents;
  char *home;
  enum {MAIN,ADD} mode = MAIN;
  string dic_filename;
  
  gtk_init(&argc, &argv);

  // for gettext
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);

  while(1){
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"help", no_argument, NULL, 'h'},
      {"version", no_argument, NULL, 'v'},
      {"add", no_argument, NULL, 'a'}
    };
    
    c = getopt_long(argc, argv, "hv", long_options, &option_index);
    if(c == -1) break;

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
      cout << "This program read ${HOME}/.anthy/private-dic.src automatically"\
        "when it starts. If you have existing dictionary which is not"\
        "located there, copy your dictionary to "\
        "${HOME}/.anthy/private-dic.src before you launch this program."\
        "If you have no dictionary, the new dictionary will be created"\
        "implicitly.";
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
  if(home != NULL){
    dic_filename = string(home);
    dic_filename.append("/.anthy/private-dic.src");
    free(home);
  }else{
    cout << "Cannot find $HOME environment variable." << endl;
  }

  KasumiDic *dic;
  try{
    dic = new KasumiDic(dic_filename);
  }catch(KasumiDicExaminationException e){
    cout << "line " << e.getLine() << ":" << e.getMessage() << endl;
    exit(1);
  }
  
  if(mode == MAIN){
    KasumiMainWindow window = KasumiMainWindow(dic);
    gtk_main();
  }else if(mode == ADD){
    KasumiAddWindow window = KasumiAddWindow(dic);
    gtk_main();
  }
}
