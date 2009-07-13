/*********************************************************************
 *
 * main.cxx
 * 
 * Kasumi - a management tool for a private dictionary of anthy
 * 
 * Copyright (C) 2004-2006 Takashi Nakamoto
 * Copyright (C) 2005 Takuro Ashie
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 * 
*********************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <iostream>
#include <string>
#include "KasumiDic.hxx"
#include "KasumiMainWindow.hxx"
#include "KasumiAddWindow.hxx"
#include "KasumiConfiguration.hxx"
#include "KasumiException.hxx"
#include "intl.h"
extern "C"{  // ad-hoc solution for a defect of Anthy
#include <anthy/anthy.h>
#include <anthy/dicutil.h>
}

using namespace std;

void showHelp(){
  cout << "Usage: kasumi [option]" << endl;
  cout << "Option:" << endl;
  cout << "  -h --help        Show this message." << endl;
  cout << "  -v --version     Show Kasumi's version and copyright" << endl;;
  cout << "                   information" << endl;
  cout << "  -m --manage      Run Kasumi in Manage mode." << endl;
  cout << "                   You may abbreviate this option." << endl;
  cout << "  -a --add         Run Kasumi in Add mode." << endl;
  cout << "  -e --exclusive   Run Kasumi in Exclusive mode." << endl;
  cout << "  --sound val      Set default sound entry (only in Add mode)" << endl;
  cout << "  --spelling val   Set default spelling entry (only in Add mode)" << endl;
  cout << "  --wordclass val  Set default word class entry (only in Add mode)" << endl;
  //  cout << "  -x val           Set default horizontal window position" << endl;
  //  cout << "  -y val           Set default vertical window position" << endl;
  cout << "  -i --import      Import selected text as a spelling" << endl;
  cout << "  -I --ignore      Ignore selected text" << endl;
#ifdef HAS_ANTHY_DICUTIL_SET_ENCODING
  cout << "  -E --eucjp       Use EUC-JP encoding for dictionary" << endl;
#endif // HAS_ANTHY_DICUTIL_SET_ENCODING
  cout << endl;
}

void showVersion(){
  cout << "Kasumi " << VERSION << " - a personal dictionary manager for Anthy" << endl;
  cout << "Copyright (C) 2004-2006 Takashi Nakamoto.\n";
  cout << "This program comes with NO WARRANTY, to the extent permitted " \
          "by law. \nYou may redistribute it under the terms of the GNU " \
          "General Public License; \nsee the file named COPYING for " \
          "details.";
  cout << endl;
}


enum {
	TARGET_STRING
};

static GtkTargetEntry targets[]={
	{"STRING", 0, TARGET_STRING},
};
static GdkAtom  atom0;  /* for checking process  */
static GdkAtom  atom1;  /* for sending arguments */
static gchar   *arg_text = NULL;
static gint     instance = -1;

static void cb_selection_get(GtkWidget *widget,
			     GtkSelectionData *data,
			     guint info,
			     guint time,
			     GtkWidget *window)
{
  gchar *text = NULL;
  gint length = 0;

  if (data->selection == atom0) {
    text = "Kasumi Selection";
    length = strlen(text);
    gtk_selection_convert(window, atom1,
			  GDK_SELECTION_TYPE_STRING,
			  GDK_CURRENT_TIME);
  } else if (data->selection == atom1 && arg_text != NULL) {
    text = arg_text;
    arg_text = NULL;
    length = strlen(text);
  }

  if (text != NULL) {
    gtk_selection_data_set_text(data, text, length);
    if (data->selection == atom1)
      g_free(text);
  }
}

static void cb_selection_received(GtkWidget *widget,
				  GtkSelectionData *data,
				  guint time,
				  gpointer user_data)
{
  if (data->selection == atom0) {
    instance = MAX(data->length, 0);
  } else if (data->selection == atom1 && data->length > 0) {
  }
}

static GtkWidget *check_duplicated_process (int argc, char *argv[])
{
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gint i, length = 0;

  gtk_widget_realize(window);

  atom0 = gdk_atom_intern("Kasumi InterProcess communication 0",
			     FALSE);
  atom1 = gdk_atom_intern("Kasumi InterProcess communication 1",
			     FALSE);

  gtk_selection_add_targets(window, atom0, targets, 1);
  gtk_selection_add_targets(window, atom1, targets, 1);
  g_signal_connect (window, "selection-get",
		    G_CALLBACK(cb_selection_get), window);
  g_signal_connect (window, "selection-received",
		    G_CALLBACK(cb_selection_received), window);

  for (i = 0; i < argc; i++) {
    gint len;

    len = strlen(argv[i]) * sizeof(gchar);
    arg_text = (gchar*) g_realloc(arg_text,
				  length + len + sizeof(gchar));
    g_memmove(arg_text + length, argv[i], len);
    length += len;
    arg_text[length++] = '\n';
  }
  if (length > 0) {
    arg_text[length - 1] = '\0';
    gtk_selection_owner_set(window, atom1, GDK_CURRENT_TIME);
  }
  gtk_selection_convert(window,atom0,
			GDK_SELECTION_TYPE_STRING,
			GDK_CURRENT_TIME);
  while (instance < 0)
    while (gtk_events_pending())
      gtk_main_iteration();
  if (instance > 0) {
    /* Kasumi process already exists */
    while (arg_text != NULL)
      while (gtk_events_pending())
	gtk_main_iteration();
    gtk_widget_destroy(window);
    cerr << "Kasumi process already exists." << endl;
    return NULL;
  }
  g_free(arg_text);
  arg_text = NULL;
  gtk_selection_owner_set(window, atom0, GDK_CURRENT_TIME);

  return window;
}

int main(int argc, char *argv[])
{
  gtk_init(&argc, &argv);

#ifdef ENABLE_NLS
  // for gettext
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);
#endif // #ifdef ENABLE_NLS

  GtkWidget *server = check_duplicated_process (argc, argv);
  if (!server)
    return 0;

  try{
      anthy_dic_util_init();

    KasumiWordType::initWordTypeList();      
    KasumiConfiguration *conf = new KasumiConfiguration(argc, argv);

#ifdef HAS_ANTHY_DICUTIL_SET_ENCODING
    if (conf->getPropertyValueByBool("UseEUCJP"))
      anthy_dic_util_set_encoding(ANTHY_EUC_JP_ENCODING);
    else
      anthy_dic_util_set_encoding(ANTHY_UTF8_ENCODING);
#endif // HAS_ANTHY_DICUTIL_SET_ENCODING

    KasumiDic *dic = new KasumiDic(conf);

    string startupMode = conf->getPropertyValue("StartupMode");
    if(startupMode == string("HELP")){
      showHelp();
    }else if(startupMode == string("VERSION")){
      showVersion();
    }else if(startupMode == string("MANAGE")){
      new KasumiMainWindow(dic,conf);
      gtk_main();
    }else if(startupMode == string("ADD")){
      new KasumiAddWindow(dic,conf);
      gtk_main();
    }else if(startupMode == string("EXCLUSIVE")){
      new KasumiAddWindow(dic,conf);
      gtk_main();
    }
  }catch(KasumiException e){
    handleException(e);
  }

  gtk_widget_destroy (server);

  return 0;
}

