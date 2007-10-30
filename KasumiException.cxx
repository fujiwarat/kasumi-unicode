/*********************************************************************
 * 
 * KasumiException.cxx
 * 
 * Kasumi - a management tool for a private dictionary of anthy
 * 
 * Copyright (C) 2004-2006 Takashi Nakamoto
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

#include <gtk/gtk.h>
#include <cstdlib>
#include <iostream>
#include "KasumiException.hxx"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

void handleException(KasumiException e){
  GtkWidget *dialog;
  switch(e.getOutput()){
  case STDERR:
    cerr << e.getMessage() << endl;
    break;
  case STDOUT:
    cout << e.getMessage() << endl;
    break;
  case ERR_DIALOG:
    dialog = gtk_message_dialog_new(NULL,
                                    (GtkDialogFlags)0,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_CLOSE,
				    "%s",
                                    e.getMessage().c_str());
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
    break;
  case WARNING_DIALOG:
    dialog = gtk_message_dialog_new(NULL,
                                    (GtkDialogFlags)0,
                                    GTK_MESSAGE_WARNING,
                                    GTK_BUTTONS_OK,
				    "%s",
                                    e.getMessage().c_str());
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
    break;
  }

  switch(e.getDisposal()){
  case KILL:
    exit(1);
    break;
  case QUIT:
    exit(0);
    break;
  case ALERT_ONLY:
    // nothing to do
    break;
  }
}
