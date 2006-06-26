#include <gtk/gtk.h>
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
