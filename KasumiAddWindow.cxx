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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiAddWindow::KasumiAddWindow(KasumiDic *aDictionary,
                                 KasumiConfiguration *conf){
  this->conf = conf;
  
  dictionary = aDictionary;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), _("Kasumi"));
  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(_call_back_add_window_delete_event), this);

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

  // creating combo box for "Word Class"
  label = gtk_label_new(_("Word Class"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);
  
  WordClassCombo = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Noun"));  
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Adverb"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Person's name"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Place-name"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Adjective"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo), 0);
  gtk_box_pack_start(GTK_BOX(vbox),
                     GTK_WIDGET(WordClassCombo),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(WordClassCombo), "changed",
                  G_CALLBACK(_call_back_add_window_changed_word_class_combo), this);

  // creating noun option pane
  NounOptionPane = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(NounOptionPane),
                     FALSE,FALSE,0);

  NounOptionNaConnectionCheck =
    gtk_check_button_new_with_label(_("NA connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionNaConnectionCheck),
                     FALSE,FALSE,0);

  NounOptionSaConnectionCheck =
    gtk_check_button_new_with_label(_("SA connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionSaConnectionCheck),
                     FALSE,FALSE,0);

  NounOptionSuruConnectionCheck =
    gtk_check_button_new_with_label(_("SURU connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionSuruConnectionCheck),
                     FALSE,FALSE,0);

  NounOptionGokanCheck =
    gtk_check_button_new_with_label(_("Can be Bunnsetsu"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionGokanCheck),
                     FALSE,FALSE,0);
  NounOptionKakujoshiConnectionCheck =
    gtk_check_button_new_with_label(_("KAKUJOSHI connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionKakujoshiConnectionCheck),
                     FALSE,FALSE,0);

  // creating adv option pane
  AdvOptionPane = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(AdvOptionPane),
                     FALSE,FALSE,0);

  AdvOptionToConnectionCheck =
    gtk_check_button_new_with_label(_("TO connection"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionToConnectionCheck),
                     FALSE,FALSE,0);

  AdvOptionTaruConnectionCheck =
    gtk_check_button_new_with_label(_("TARU connection"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionTaruConnectionCheck),
                     FALSE,FALSE,0);

  AdvOptionSuruConnectionCheck =
    gtk_check_button_new_with_label(_("SURU connection"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionSuruConnectionCheck),
                     FALSE,FALSE,0);

  AdvOptionGokanCheck =
    gtk_check_button_new_with_label(_("Can be Bunnsetsu"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionGokanCheck),
                     FALSE,FALSE,0);
  
  // creating box for buttons
  GtkWidget *hbutton_box = gtk_hbutton_box_new();
  gtk_button_box_set_child_size(GTK_BUTTON_BOX(hbutton_box),1,1);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbutton_box),FALSE,FALSE,0);

  // creating buttons and configure shortcut key
  GtkWidget *button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Quit"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_add_window_quit),this);
  GtkAccelGroup *accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  string key = conf->getPropertyValue("QuitShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Add"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_add_window_add),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("AddShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Manage Mode"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_manage_mode),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("ManageModeShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);

  // get selection at the time of launching
  g_signal_connect(G_OBJECT(SpellingEntry),"selection_received",
                   G_CALLBACK(_call_back_selection_data_received),NULL);
  
  gtk_widget_show_all(window);
  gtk_widget_hide(AdvOptionPane);

  // set default word classes after option panes are initialized
  string wordclass = conf->getPropertyValue("DefaultAddingWordClass");
  if(wordclass == string(EUCJP_MEISHI)){
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),0);
  }else if(wordclass == string(EUCJP_FUKUSHI)){
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),1);
  }else if(wordclass == string(EUCJP_JINNMEI)){
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),2);
  }else if(wordclass == string(EUCJP_CHIMEI)){
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),3);
  }else if(wordclass == string(EUCJP_KEIYOUSHI)){
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),4);
  }else{
    cout << "Invalid word class: " << wordclass << endl;
    exit(1);
  }
  
  // resize window appropriate size
  gtk_window_reshow_with_initial_size(GTK_WINDOW(window));
  gtk_window_set_resizable(GTK_WINDOW(window),true);

  // set default window position
  int x = conf->getPropertyValueByInt("DefaultWindowPosX");
  int y = conf->getPropertyValueByInt("DefaultWindowPosY");
  if(x >= 0 && y >= 0){
    gtk_window_move(GTK_WINDOW(window),x,y);
  }

  if(conf->getPropertyValue("DefaultAddingSpelling") == ""){
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
  delete this;
  gtk_main_quit();
}

void KasumiAddWindow::ClickedAddButton(GtkWidget *widget){
  KasumiWord *word = new KasumiWord(conf);
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
    word->setWordClass(getActiveWordClass());

    if(word->getWordClass() == NOUN){
      word->setOption(string(EUCJP_SASETSUZOKU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(NounOptionSaConnectionCheck)));
      word->setOption(string(EUCJP_NASETSUZOKU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(NounOptionNaConnectionCheck)));
      word->setOption(string(EUCJP_SURUSETSUZOKU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(NounOptionSuruConnectionCheck)));
      word->setOption(string(EUCJP_GOKANNNOMIDEBUNNSETSU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(NounOptionGokanCheck)));
      word->setOption(string(EUCJP_KAKUJOSHISETSUZOKU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(NounOptionKakujoshiConnectionCheck)));
    }else if(word->getWordClass() == ADV){
      word->setOption(string(EUCJP_TOSETSUZOKU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(AdvOptionToConnectionCheck)));
      word->setOption(string(EUCJP_TARUSETSUZOKU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(AdvOptionTaruConnectionCheck)));
      word->setOption(string(EUCJP_SURUSETSUZOKU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(AdvOptionSuruConnectionCheck)));
      word->setOption(string(EUCJP_GOKANNNOMIDEBUNNSETSU),
                      gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON(AdvOptionGokanCheck)));
    }

    dictionary->appendWord(word);
    //    dictionary->store();

    gtk_entry_set_text(GTK_ENTRY(SoundEntry), "");
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry), "");
    const int FREQ_DEFAULT = conf->getPropertyValueByInt("DefaultFrequency");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),FREQ_DEFAULT);
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),0);
  }catch(KasumiInvalidCharacterForSoundException e){
      string message;
      message = string(_("Sound must consist of only Hiragana characters. You have entered invalid character: "));
      message += e.getInvalidCharacter();
      
      GtkWidget *dialog = gtk_message_dialog_new
        (GTK_WINDOW(window),
         GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_MESSAGE_ERROR,
         GTK_BUTTONS_OK,
         message.c_str());
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      delete (word);
  }
}

void KasumiAddWindow::ChangedWordClassCombo(GtkWidget *widget){
  if(getActiveWordClass() == NOUN){
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionSaConnectionCheck),
      false);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionNaConnectionCheck),
      false);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionSuruConnectionCheck),
      false);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionGokanCheck),
      false);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionKakujoshiConnectionCheck),
      false);
    
    gtk_widget_show(NounOptionPane);
    gtk_widget_hide(AdvOptionPane);

    // resize window appropriate size
    gtk_window_reshow_with_initial_size(GTK_WINDOW(window));    
  }else if(getActiveWordClass() == ADV){
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionToConnectionCheck),
      false);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionTaruConnectionCheck),
      false);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionSuruConnectionCheck),
      false);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionGokanCheck),
      false);
    
    gtk_widget_hide(NounOptionPane);
    gtk_widget_show(AdvOptionPane);
    
    // resize window appropriate size
    gtk_window_reshow_with_initial_size(GTK_WINDOW(window));    
  }else{
    gtk_widget_hide(NounOptionPane);
    gtk_widget_hide(AdvOptionPane);
    
    // resize window appropriate size
    gtk_window_reshow_with_initial_size(GTK_WINDOW(window));    
  }
}

WordClassType KasumiAddWindow::getActiveWordClass(){
  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(WordClassCombo))){
  case 0:
    return NOUN;
  case 1:
    return ADV;
  case 2:
    return PERSON;
  case 3:
    return PLACE;
  case 4:
    return ADJ;
  default:
    return NOUN;
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

void _call_back_add_window_changed_word_class_combo(GtkWidget *widget,
                                                    gpointer data){
  KasumiAddWindow *window = (KasumiAddWindow *)data;
  window->ChangedWordClassCombo(widget);
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
