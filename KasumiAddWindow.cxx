#include <gtk/gtk.h>
#include <iostream>
#include "KasumiAddWindow.hxx"
#include "KasumiWord.hxx"
#include "KasumiDic.hxx"
#include "KasumiString.hxx"
#include "KasumiConfiguration.hxx"
#include "KasumiMainWindow.hxx"
#include <gdk/gdkkeysyms.h>
#include "intl.h"
extern "C"{  // ad-hoc solution for a defect of Anthy
#include "anthy/dicutil.h"
}


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiAddWindow::KasumiAddWindow(KasumiDic *aDictionary,
                                 KasumiConfiguration *conf){
  this->conf = conf;
  
  dictionary = aDictionary;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
  gtk_window_set_title(GTK_WINDOW(window), _("Kasumi"));
  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(_call_back_add_window_delete_event), this);

  // tooltips for every widget
  Tooltips = gtk_tooltips_new();

  // creating vbox for text entries, spin button and so on.
  GtkWidget *vbox = gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(window),vbox);  

  // creating text entries for "Spelling"
  GtkWidget *label = gtk_label_new(_("Spelling"));
  GtkWidget *alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  SpellingEntry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(SpellingEntry),
                     KasumiWord::convertEUCJPToUTF8(conf->getPropertyValue("DefaultAddingSpelling")).c_str());
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(SpellingEntry),FALSE,FALSE,0);

  // creating text entries for "Sound"
  label = gtk_label_new(_("Sound"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  SoundEntry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(SoundEntry),
                     KasumiWord::convertEUCJPToUTF8(conf->getPropertyValue("DefaultAddingSound")).c_str());
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(SoundEntry),FALSE,FALSE,0);

  // creating spin button for "Frequency"
  label = gtk_label_new(_("Frequency"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  const int FREQ_DEFAULT = conf->getPropertyValueByInt("DefaultFrequency");
  const int FREQ_LBOUND = conf->getPropertyValueByInt("MinFrequency");  
  const int FREQ_UBOUND = conf->getPropertyValueByInt("MaxFrequency");
  GtkObject *adjustment = gtk_adjustment_new(FREQ_DEFAULT,
                                             FREQ_LBOUND,
                                             FREQ_UBOUND,
                                             1,
                                             FREQ_LBOUND / 100
                                             ,0);
  FrequencySpin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment),1.0,0);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(FrequencySpin),FALSE,FALSE,0);

  // creating combo box for "Word Type"
  label = gtk_label_new(_("Word Type"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  WordTypeCombo = gtk_combo_box_new_text();
  WordTypeList::iterator p = KasumiWordType::getBeginIteratorWordTypeList();
  while( p != KasumiWordType::getEndIteratorWordTypeList() )
  {
      gtk_combo_box_append_text(GTK_COMBO_BOX(WordTypeCombo),
				(*p)->getUIString().c_str());
      p++;
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(WordTypeCombo), 0);
  gtk_box_pack_start(GTK_BOX(vbox),
                     GTK_WIDGET(WordTypeCombo),FALSE,FALSE,0);

  // creating box for buttons
  GtkWidget *hbutton_box = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(hbutton_box),GTK_BUTTONBOX_SPREAD);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbutton_box),FALSE,FALSE,0);

  // creating buttons and configure shortcut key
  GtkWidget *button = gtk_button_new();
  GtkWidget *button_image = gtk_image_new_from_stock(GTK_STOCK_QUIT,
                                                     GTK_ICON_SIZE_BUTTON);
  gtk_container_add(GTK_CONTAINER(button),button_image);
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_add_window_quit),this);
  GtkAccelGroup *accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  string key = conf->getPropertyValue("QuitShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);
  gtk_tooltips_set_tip(Tooltips, button,
                       _("Quit this application"),
                       _("Save dictionary and quit this application."));

  button = gtk_button_new();
  button_image = gtk_image_new_from_stock(GTK_STOCK_ADD,
                                          GTK_ICON_SIZE_BUTTON);
  gtk_container_add(GTK_CONTAINER(button),button_image);
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_add_window_add),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("AddShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);
  gtk_tooltips_set_tip(Tooltips, button,
                       _("Add entered word"),
                       _("If all the necessary items are filled in, add entered word."));

  button = gtk_button_new();
  button_image = gtk_image_new_from_stock(GTK_STOCK_FIND_AND_REPLACE,
                                          GTK_ICON_SIZE_BUTTON);
  gtk_container_add(GTK_CONTAINER(button),button_image);
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_manage_mode),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("ManageModeShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);
  gtk_tooltips_set_tip(Tooltips, button,
                       _("Manage mode"),
                       _("Make the shift to manage mode to modify and remove registered words."));


  // get selection at the time of launching
  g_signal_connect(G_OBJECT(SpellingEntry),"selection_received",
                   G_CALLBACK(_call_back_selection_data_received),NULL);
  
  gtk_widget_show_all(window);

  // ToDo: set default word type
  
  // resize window appropriate size
  gtk_window_reshow_with_initial_size(GTK_WINDOW(window));
  gtk_window_set_resizable(GTK_WINDOW(window),true);

  // set default window position
  //  int x = conf->getPropertyValueByInt("DefaultWindowPosX");
  //  int y = conf->getPropertyValueByInt("DefaultWindowPosY");
  //  if(x >= 0 && y >= 0){
  //    gtk_window_move(GTK_WINDOW(window),x,y);
  //  }

  if(conf->getPropertyValue("DefaultAddingSpelling") == "" && conf->getPropertyValueByBool("ImportSelectedText")){
    get_targets(SpellingEntry);
  }

}

KasumiAddWindow::~KasumiAddWindow(){
  destroy();
}

void KasumiAddWindow::destroy(){
  gtk_widget_destroy(window);
}

void KasumiAddWindow::quit(){
  dictionary->store();
  anthy_dic_util_quit();
  delete this;
  gtk_main_quit();
}

void KasumiAddWindow::ClickedAddButton(GtkWidget *widget){
    KasumiWord *word = KasumiWord::createNewWord(conf);
  try{


    if(string(gtk_entry_get_text(GTK_ENTRY(SoundEntry))) == ""){
      GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_MESSAGE_ERROR,
                                                  GTK_BUTTONS_CLOSE,
                                                  _("Invlid entry for Sound."));
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      delete(word);
      return;
    }
    word->setSoundByUTF8(string(gtk_entry_get_text(GTK_ENTRY(SoundEntry))));


    if(string(gtk_entry_get_text(GTK_ENTRY(SpellingEntry))) == ""){
      GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_MESSAGE_ERROR,
                                                  GTK_BUTTONS_CLOSE,
                                                  _("Invlid entry for Spelling.")
                                                  );
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      delete(word);
      return;
    }
    word->setSpellingByUTF8(string(gtk_entry_get_text(
                                                      GTK_ENTRY(SpellingEntry))));
    word->setFrequency(gtk_spin_button_get_value_as_int(
                                                        GTK_SPIN_BUTTON(FrequencySpin)));
//    word->setWordType(getActiveWordType());
// ToDo:

    dictionary->appendWord(word);
    //    dictionary->store();

    gtk_entry_set_text(GTK_ENTRY(SoundEntry), "");
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry), "");
    const int FREQ_DEFAULT = conf->getPropertyValueByInt("DefaultFrequency");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),FREQ_DEFAULT);
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordTypeCombo),0);
  }catch(KasumiException e){
    handleException(e);
  }
}

void KasumiAddWindow::SwitchToManageMode(){
  new KasumiMainWindow(dictionary,conf);
  delete this;
}

void _call_back_add_window_delete_event(GtkWidget *widget,
                                            GdkEvent *event,
                                            gpointer data){
  KasumiAddWindow *window = (KasumiAddWindow *)data;
  window->quit();
}

void _call_back_add_window_quit(GtkWidget *widget,
                                gpointer data){
  KasumiAddWindow *window = (KasumiAddWindow *)data;
  window->quit();
}

void _call_back_add_window_add(GtkWidget *widget,
                               gpointer data){
  KasumiAddWindow *window = (KasumiAddWindow *)data;
  window->ClickedAddButton(widget);
}

void _call_back_manage_mode(GtkWidget *widget,
                            gpointer data){
  KasumiAddWindow *window = (KasumiAddWindow *)data;
  window->SwitchToManageMode();
}

// to set selected string to SpellingEntry
void _call_back_selection_data_received(GtkWidget *widget,
                                        GtkSelectionData *selection_data,
                                        gpointer data){
  if(selection_data->length < 0){
    // failed retrieving selection
    // do nothing
    return;
  }

  string atom_name = string(gdk_atom_name(selection_data->type));

  gchar *str;
  str = reinterpret_cast<gchar*>(selection_data->data);

  if(atom_name == "UTF8_STRING"){
    gtk_entry_set_text(GTK_ENTRY(widget), str);
  }

  return;
} 

// to get selected string
void get_targets(GtkWidget *data){
  static GdkAtom targets_atom = GDK_NONE;
  GtkWidget *window = (GtkWidget *)data;	

  targets_atom = gdk_atom_intern("UTF8_STRING", FALSE);
  gtk_selection_convert(window, GDK_SELECTION_PRIMARY, targets_atom,
                        GDK_CURRENT_TIME);
}
