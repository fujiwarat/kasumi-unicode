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
#include "KasumiException.hxx"
#include "intl.h"

using namespace std;

int main(int argc, char *argv[])
{
  int c;
  char *contents;
  char *home;
  enum {TOP,ADD} mode = TOP;
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
      cout << "Show help!" << endl;
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
    case '?':
      cout << "Argument error." << endl;
      exit(0);
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

  try{
    KasumiDic *dic = new KasumiDic(dic_filename);

    KasumiMainWindow window = KasumiMainWindow(dic);
  }catch(KasumiDicExaminationException e){
    cout << "line " << e.getLine() << ":" << e.getMessage() << endl;
    return 0;
  }catch(KasumiDicStoreException e){
    cout <<  e.getMessage() << endl;
    return 0;
  }

  gtk_main();
}
