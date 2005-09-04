#include <gtk/gtk.h>
#include <iostream>
#include "KasumiMainWindow.hxx"
#include "KasumiException.hxx"
#include "KasumiWord.hxx"
#include "KasumiDic.hxx"
#include "KasumiString.hxx"
#include "KasumiAddWindow.hxx"
#include "KasumiConfiguration.hxx"
#include <gdk/gdkkeysyms.h>
#include "intl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiMainWindow::KasumiMainWindow(KasumiDic *aDictionary,
                                   KasumiConfiguration *conf){
  this->conf = conf;
  
  dictionary = aDictionary;
  modificationFlag = false;
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), _("Kasumi"));
  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(_call_back_delete_event), this);

  // creating top vbox
  GtkWidget *vbox = gtk_vbox_new(FALSE,8);
  gtk_container_set_border_width(GTK_CONTAINER(vbox),8);
  gtk_container_add(GTK_CONTAINER(window),vbox);

  // creating hbox for text entries, spin button and so on
  GtkWidget *hbox = gtk_hbox_new(FALSE,8);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox),TRUE,TRUE,0);

  // creating scrolled window for list view
  ScrolledWindow = gtk_scrolled_window_new(NULL,NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ScrolledWindow),
                                 GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(GTK_WIDGET(ScrolledWindow),300,300);
  gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(ScrolledWindow),TRUE,TRUE,0);

  // creating tree(list) view for words
  GtkWidget *WordListView = gtk_tree_view_new();
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  SpellingColumn = gtk_tree_view_column_new_with_attributes(_("Spelling"),
                                                            renderer,
                                                            "text",
                                                            COL_WORD,
                                                            NULL);
  gtk_tree_view_column_set_resizable(SpellingColumn, true);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(WordListView),SpellingColumn,-1);

  renderer = gtk_cell_renderer_text_new();
  SoundColumn = gtk_tree_view_column_new_with_attributes(_("Sound"),
                                                         renderer,
                                                         "text",
                                                         COL_YOMI,
                                                         NULL);
  gtk_tree_view_column_set_resizable(SoundColumn, true);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(WordListView),SoundColumn,-1);
  gtk_tree_view_column_set_clickable(SoundColumn,TRUE);
  g_signal_connect(G_OBJECT(SoundColumn), "clicked",
                   G_CALLBACK(_call_back_clicked_column_header), this);

  renderer = gtk_cell_renderer_text_new();
  FreqColumn = gtk_tree_view_column_new_with_attributes(_("Frequency"),
                                                        renderer,
                                                        "text",
                                                        COL_FREQ,
                                                        NULL);
  gtk_tree_view_column_set_resizable(FreqColumn, true);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(WordListView),FreqColumn,-1);
  gtk_tree_view_column_set_clickable(FreqColumn, TRUE);
  g_signal_connect(G_OBJECT(FreqColumn), "clicked",
                   G_CALLBACK(_call_back_clicked_column_header), this);
  /*
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(WordListView),-1,
                                              _("Frequency"),renderer,
                                              "text",COL_FREQ,
                                              NULL);
  */
  renderer = gtk_cell_renderer_text_new();
  WordClassColumn = gtk_tree_view_column_new_with_attributes(_("WordClass"),
                                                             renderer,
                                                             "text",
                                                             COL_PART,
                                                             NULL);
  gtk_tree_view_column_set_resizable(WordClassColumn, true);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(WordListView),WordClassColumn,-1);
  gtk_tree_view_column_set_clickable(WordClassColumn, TRUE);
  g_signal_connect(G_OBJECT(WordClassColumn), "clicked",
                   G_CALLBACK(_call_back_clicked_column_header), this);
  
  WordList = gtk_list_store_new(NUM_COLS,G_TYPE_UINT,
                                G_TYPE_STRING,G_TYPE_STRING,
                                G_TYPE_UINT,G_TYPE_STRING);
  SortList = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(WordList));
  gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(SortList), COL_YOMI,
                                  sortFuncBySound,(gpointer)dictionary,
                                  NULL);
  gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(SortList), COL_FREQ,
                                  sortFuncByFreq,(gpointer)dictionary,
                                  NULL);
  gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(SortList), COL_PART,
                                  sortFuncByWordClass,(gpointer)dictionary,
                                  NULL);
  gtk_tree_view_set_model(GTK_TREE_VIEW(WordListView),
                          GTK_TREE_MODEL(SortList));

  SortListSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(WordListView));
  gtk_tree_selection_set_mode(SortListSelection, GTK_SELECTION_SINGLE);
  g_signal_connect(G_OBJECT(SortListSelection), "changed",
                   G_CALLBACK(_call_back_changed_list_cursor), this);

  // destroy model automatically with view
  g_object_unref(GTK_TREE_MODEL(WordList));

  gtk_container_add(GTK_CONTAINER(ScrolledWindow),GTK_WIDGET(WordListView));

  // separator between word list box and text entries
  GtkWidget *vsep = gtk_vseparator_new();
  gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(vsep),FALSE,FALSE,0);

  // creating vbox for text entries
  GtkWidget *entry_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(entry_vbox),FALSE,FALSE,0);

  // creating text entries for "Spelling"
  GtkWidget *label = gtk_label_new(_("Spelling"));
  GtkWidget *alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,2);

  SpellingEntry = gtk_entry_new();
  HandlerIDOfSpellingEntry =
    g_signal_connect(G_OBJECT(SpellingEntry),
                     "changed",
                     G_CALLBACK(_call_back_changed_spelling_entry),
                     this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),
                     GTK_WIDGET(SpellingEntry),
                     FALSE,
                     FALSE,
                     2);

  // creating text entries for "Sound"
  label = gtk_label_new(_("Sound"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,2);

  SoundEntry = gtk_entry_new();
  HandlerIDOfSoundEntry = g_signal_connect(G_OBJECT(SoundEntry), "changed",
               G_CALLBACK(_call_back_changed_sound_entry), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(SoundEntry),FALSE,FALSE,2);
  previousSoundEntryText = string(gtk_entry_get_text(GTK_ENTRY(SoundEntry)));

  // creating spin button for "Frequency"
  label = gtk_label_new(_("Frequency"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,2);

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
  HandlerIDOfFrequencySpin =
    g_signal_connect(G_OBJECT(FrequencySpin),
                     "value-changed",
                     G_CALLBACK(_call_back_changed_frequency_spin),
                     this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),
                     GTK_WIDGET(FrequencySpin),
                     FALSE,
                     FALSE,
                     2);

  // creating combo box for "Word Class"
  label = gtk_label_new(_("Word Class"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,2);
  
  WordClassCombo = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Noun"));  
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Adverb"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Person's name"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Place-name"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Adjective"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Verb"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo), 0);
  HandlerIDOfWordClassCombo =
    g_signal_connect(G_OBJECT(WordClassCombo), "changed",
                     G_CALLBACK(_call_back_changed_word_class_combo), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),
                     GTK_WIDGET(WordClassCombo),FALSE,FALSE,2);

  // creating noun option pane
  NounOptionPane = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(NounOptionPane),
                     FALSE,FALSE,0);

  NounOptionNaConnectionCheck =
    gtk_check_button_new_with_label(_("NA connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionNaConnectionCheck),
                     FALSE,FALSE,0);
  HandlerIDOfNounOptionNaConnectionCheck =
    g_signal_connect(G_OBJECT(NounOptionNaConnectionCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  NounOptionSaConnectionCheck =
    gtk_check_button_new_with_label(_("SA connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionSaConnectionCheck),
                     FALSE,FALSE,0);
  HandlerIDOfNounOptionSaConnectionCheck =
    g_signal_connect(G_OBJECT(NounOptionSaConnectionCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  NounOptionSuruConnectionCheck =
    gtk_check_button_new_with_label(_("SURU connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionSuruConnectionCheck),
                     FALSE,FALSE,0);
  HandlerIDOfNounOptionSuruConnectionCheck =
    g_signal_connect(G_OBJECT(NounOptionSuruConnectionCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  NounOptionGokanCheck =
    gtk_check_button_new_with_label(_("Can be Bunnsetsu"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionGokanCheck),
                     FALSE,FALSE,0);
  HandlerIDOfNounOptionGokanCheck =
    g_signal_connect(G_OBJECT(NounOptionGokanCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  NounOptionKakujoshiConnectionCheck =
    gtk_check_button_new_with_label(_("KAKUJOSHI connection"));
  gtk_box_pack_start(GTK_BOX(NounOptionPane),
                     GTK_WIDGET(NounOptionKakujoshiConnectionCheck),
                     FALSE,FALSE,0);
  HandlerIDOfNounOptionKakujoshiConnectionCheck =
    g_signal_connect(G_OBJECT(NounOptionKakujoshiConnectionCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  // creating adv option pane
  AdvOptionPane = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(AdvOptionPane),
                     FALSE,FALSE,0);

  AdvOptionToConnectionCheck =
    gtk_check_button_new_with_label(_("TO connection"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionToConnectionCheck),
                     FALSE,FALSE,0);
  HandlerIDOfAdvOptionToConnectionCheck =
    g_signal_connect(G_OBJECT(AdvOptionToConnectionCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  AdvOptionTaruConnectionCheck =
    gtk_check_button_new_with_label(_("TARU connection"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionTaruConnectionCheck),
                     FALSE,FALSE,0);
  HandlerIDOfAdvOptionTaruConnectionCheck =
    g_signal_connect(G_OBJECT(AdvOptionTaruConnectionCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  AdvOptionSuruConnectionCheck =
    gtk_check_button_new_with_label(_("SURU connection"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionSuruConnectionCheck),
                     FALSE,FALSE,0);
  HandlerIDOfAdvOptionSuruConnectionCheck =
    g_signal_connect(G_OBJECT(AdvOptionSuruConnectionCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  AdvOptionGokanCheck =
    gtk_check_button_new_with_label(_("Can be Bunnsetsu"));
  gtk_box_pack_start(GTK_BOX(AdvOptionPane),
                     GTK_WIDGET(AdvOptionGokanCheck),
                     FALSE,FALSE,0);
  HandlerIDOfAdvOptionGokanCheck =
    g_signal_connect(G_OBJECT(AdvOptionGokanCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  // creating verb option pane
  VerbOptionPane = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(VerbOptionPane),
                     FALSE,FALSE,0);

  label = gtk_label_new(_("VerbType"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(VerbOptionPane),GTK_WIDGET(alignment),
                             FALSE,FALSE,2);

  VerbTypeCombo = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Ba line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Ga line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Ka line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Ma line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Na line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Ra line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Sa line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Ta line, 5 columns"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(VerbTypeCombo), _("Wa line, 5 columns"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo), 0);
  HandlerIDOfVerbTypeCombo =
    g_signal_connect(G_OBJECT(VerbTypeCombo), "changed",
                     G_CALLBACK(_call_back_changed_verb_type_combo), this);
  gtk_box_pack_start(GTK_BOX(VerbOptionPane),
                     GTK_WIDGET(VerbTypeCombo),
                     FALSE,FALSE,0);

  VerbOptionRentaiCheck =
    gtk_check_button_new_with_label(_("Can be Meishi when Renyou type"));
  gtk_box_pack_start(GTK_BOX(VerbOptionPane),
                     GTK_WIDGET(VerbOptionRentaiCheck),
                     FALSE,FALSE,0);
  HandlerIDOfVerbOptionRentaiCheck =
    g_signal_connect(G_OBJECT(VerbOptionRentaiCheck),"toggled",
                     G_CALLBACK(_call_back_toggled_check),this);

  GtkWidget *hsep = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hsep),FALSE,FALSE,0);

  // creating expandable box for search function
  GtkWidget *search_expander = gtk_expander_new(_("Search"));
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(search_expander),FALSE,FALSE,0);

  GtkWidget *search_hbox = gtk_hbox_new(FALSE,8);
  gtk_container_add(GTK_CONTAINER(search_expander),search_hbox);

  // creating Entry and Button for search function
  SearchEntry = gtk_entry_new();
  g_signal_connect(G_OBJECT(SearchEntry),
                   "changed",
                   G_CALLBACK(_call_back_changed_search_entry),
                   this);
  g_signal_connect(G_OBJECT(SearchEntry),
                   "activate",
                   G_CALLBACK(_call_back_activate_search_entry),
                   this); // called when Entry key is pressed
  gtk_box_pack_start(GTK_BOX(search_hbox),
                     GTK_WIDGET(SearchEntry),
                     FALSE,
                     FALSE,
                     0);

  SearchBySpellingRadio = gtk_radio_button_new_with_label(NULL,
                                                          _("Find By Spelling"));
  gtk_box_pack_start(GTK_BOX(search_hbox),SearchBySpellingRadio,
                     TRUE,TRUE,0);
  
  SearchBySoundRadio =
    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(SearchBySpellingRadio),
                                                _("Find By Sound"));
  gtk_box_pack_start(GTK_BOX(search_hbox),SearchBySoundRadio,
                     TRUE,TRUE,0);

  hsep = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hsep),FALSE,FALSE,0);

  /* creating box for buttons */
  GtkWidget *hbutton_box = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(hbutton_box),GTK_BUTTONBOX_SPREAD);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbutton_box),FALSE,FALSE,0);


  // creating buttons and shortcut key configuration
  GtkWidget *button = gtk_button_new_from_stock(GTK_STOCK_QUIT);
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_quit),this);
  GtkAccelGroup *accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  string key = conf->getPropertyValue("QuitShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);

  button = gtk_button_new_from_stock(GTK_STOCK_SAVE);
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_store),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("StoreShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);

  button = gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_add),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("NewWordShortcutKey");  
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);
  
  button = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_remove),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("RemoveShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Adding Mode"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_adding_mode),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  key = conf->getPropertyValue("AddingModeShortcutKey");
  gtk_widget_add_accelerator(button, "clicked", accel,
                             getAccelKey(key),
                             getModifierType(key),
                             GTK_ACCEL_VISIBLE);

  gtk_widget_show_all(window);
  gtk_widget_hide(AdvOptionPane);
  gtk_widget_hide(VerbOptionPane);

  // set default window position
  int x = conf->getPropertyValueByInt("DefaultWindowPosX");
  int y = conf->getPropertyValueByInt("DefaultWindowPosY");
  if(x >= 0 && y >= 0){
    gtk_window_move(GTK_WINDOW(window),x,y);
  }

  refresh();

  dictionary->registerEventListener(this);
}

void KasumiMainWindow::refresh(){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int i = 0;

  gtk_list_store_clear(WordList);
  
  for(i=0;i<=dictionary->getUpperBoundOfWordID();i++){
    try{
      KasumiWord *word = dictionary->getWordWithID(i);

      if(word != NULL && word->getFrequency() != 0){
        gtk_list_store_append(WordList,&iter);
      
        gtk_list_store_set(WordList,&iter,
                           COL_ID,i,
                           COL_WORD,word->getSpellingByUTF8().c_str(),
                           COL_YOMI,word->getSoundByUTF8().c_str(),
                           COL_FREQ,word->getFrequency(),
                           COL_PART,word->getStringOfWordClassByUTF8().c_str(),
                           -1);
      }
    }catch(KasumiException e){
      handleException(e);
    }
  }

  if(!gtk_tree_model_get_iter_first(model, &iter)){
    // If no words, disable text entries and other visible widgets
    gtk_widget_set_sensitive(SpellingEntry,false);
    gtk_widget_set_sensitive(SoundEntry,false);
    gtk_widget_set_sensitive(FrequencySpin,false);
    gtk_widget_set_sensitive(WordClassCombo,false);
    gtk_widget_set_sensitive(NounOptionSaConnectionCheck,false);
    gtk_widget_set_sensitive(NounOptionNaConnectionCheck,false);
    gtk_widget_set_sensitive(NounOptionSuruConnectionCheck,false);
    gtk_widget_set_sensitive(NounOptionGokanCheck,false);
    gtk_widget_set_sensitive(NounOptionKakujoshiConnectionCheck,false);

    return;
  }

  // select first word
  gtk_tree_selection_select_iter(SortListSelection,&iter);  

}

KasumiMainWindow::~KasumiMainWindow(){
  dictionary->removeEventListener(this);
  destroy();
}

void KasumiMainWindow::destroy(){
  gtk_widget_destroy(window);
}

void KasumiMainWindow::quit(){
  if(modificationFlag){
    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_QUESTION,
                                     GTK_BUTTONS_YES_NO,
                                     _("Your dictionary went through several "
                                       "changes. Do you save these changes "
                                       "before Kasumi quits?"));
    switch(gtk_dialog_run (GTK_DIALOG (dialog))){
    case GTK_RESPONSE_YES:
      dictionary->store();
      break;
    case GTK_RESPONSE_NO:
      break;
    default:
      cout << "Error: Unexcepted return value of dialog's run" << endl;
      exit(1);
    }
    gtk_widget_destroy (dialog);
  }
  delete this;
  gtk_main_quit();
}

void KasumiMainWindow::ClickedStoreButton(){
  try{
    dictionary->store();
    modificationFlag = false;
  }catch(KasumiException e){
      cout <<  e.getMessage() << endl;
      exit(1);
  }
}

void KasumiMainWindow::ClickedAddButton(){
  KasumiWord *word = new KasumiWord(conf);
  dictionary->appendWord(word);
}

void KasumiMainWindow::ClickedRemoveButton(){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    dictionary->removeWord(id);
  }
}

void KasumiMainWindow::ChangedListCursor(){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);

    g_signal_handler_block(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_block(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_block(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_block(WordClassCombo, HandlerIDOfWordClassCombo);
    g_signal_handler_block(VerbTypeCombo, HandlerIDOfVerbTypeCombo);
                           
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry),
                       word->getSpellingByUTF8().c_str());
    gtk_entry_set_text(GTK_ENTRY(SoundEntry),
                       word->getSoundByUTF8().c_str());
    previousSoundEntryText = string(gtk_entry_get_text(GTK_ENTRY(SoundEntry)));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),
                              word->getFrequency());
    setActiveWordClass(word->getWordClass());
    setActiveVerbType(word->getVerbType());

    g_signal_handler_unblock(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_unblock(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_unblock(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_unblock(WordClassCombo, HandlerIDOfWordClassCombo);
    g_signal_handler_unblock(VerbTypeCombo, HandlerIDOfVerbTypeCombo);

    synchronizeOptionCheckButton(word);
  }else{
    g_signal_handler_block(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_block(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_block(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_block(WordClassCombo, HandlerIDOfWordClassCombo);
    g_signal_handler_block(VerbTypeCombo, HandlerIDOfVerbTypeCombo);
    
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry),"");
    gtk_entry_set_text(GTK_ENTRY(SoundEntry),"");
    previousSoundEntryText = string(gtk_entry_get_text(GTK_ENTRY(SoundEntry)));
    const int FREQ_DEFAULT = conf->getPropertyValueByInt("DefaultFrequency");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),FREQ_DEFAULT);
    setActiveWordClass(NOUN);
    setActiveVerbType(B5);
    
    g_signal_handler_unblock(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_unblock(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_unblock(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_unblock(WordClassCombo, HandlerIDOfWordClassCombo);
    g_signal_handler_unblock(VerbTypeCombo, HandlerIDOfVerbTypeCombo);
    
    synchronizeOptionCheckButton(NULL);
  }

  flipOptionPane();
}

void KasumiMainWindow::ChangedSoundEntry(){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    try{
      word->setSoundByUTF8(string(gtk_entry_get_text(GTK_ENTRY(SoundEntry))));
      dictionary->modifyWord(id);
      previousSoundEntryText = string(gtk_entry_get_text(GTK_ENTRY(SoundEntry)));
    }catch(KasumiException e){
      handleException(e);

      // take back invalid entry
      g_signal_handler_block(SoundEntry, HandlerIDOfSoundEntry);
      gtk_entry_set_text(GTK_ENTRY(SoundEntry),
                         previousSoundEntryText.c_str());
      g_signal_handler_unblock(SoundEntry, HandlerIDOfSoundEntry);
    }
  }
}

void KasumiMainWindow::ChangedSpellingEntry(){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setSpellingByUTF8(string(gtk_entry_get_text(
      GTK_ENTRY(SpellingEntry))));
    //    modifiedWord(id);
    dictionary->modifyWord(id);
  }
}

void KasumiMainWindow::ChangedFrequencySpin(){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setFrequency(gtk_spin_button_get_value_as_int(
      GTK_SPIN_BUTTON(FrequencySpin)));
    //modifiedWord(id);
    dictionary->modifyWord(id);
  }
}

void KasumiMainWindow::ChangedWordClassCombo(){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setWordClass(getActiveWordClass());
    synchronizeOptionCheckButton(word);
    //    modifiedWord(id);
    dictionary->modifyWord(id);
  }

  flipOptionPane();  
}

void KasumiMainWindow::ChangedVerbTypeCombo(){
 GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setVerbType(getActiveVerbType());
    //    modifiedWord(id);
    dictionary->modifyWord(id);
  }
}

void KasumiMainWindow::ChangedOption(GtkWidget *widget){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  int id;
  string OptionName = string("");
  bool val;

  if(widget == NounOptionSaConnectionCheck){
    OptionName = string(EUCJP_SASETSUZOKU);
  }else if(widget == NounOptionNaConnectionCheck){
    OptionName = string(EUCJP_NASETSUZOKU);
  }else if(widget == NounOptionSuruConnectionCheck){
    OptionName = string(EUCJP_SURUSETSUZOKU);
  }else if(widget == NounOptionGokanCheck){
    OptionName = string(EUCJP_GOKANNNOMIDEBUNNSETSU);
  }else if(widget == NounOptionKakujoshiConnectionCheck){
    OptionName = string(EUCJP_KAKUJOSHISETSUZOKU);
  }else if(widget == AdvOptionToConnectionCheck){
    OptionName = string(EUCJP_TOSETSUZOKU);
  }else if(widget == AdvOptionTaruConnectionCheck){
    OptionName = string(EUCJP_TARUSETSUZOKU);
  }else if(widget == AdvOptionSuruConnectionCheck){
    OptionName = string(EUCJP_SURUSETSUZOKU);    
  }else if(widget == AdvOptionGokanCheck){
    OptionName = string(EUCJP_GOKANNNOMIDEBUNNSETSU);
  }else if(widget == VerbOptionRentaiCheck){
    OptionName = string(EUCJP_RENNYOUKEINOMEISHIKA);
  }

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    val = true;
  else
    val = false;

  if(gtk_tree_selection_get_selected(SortListSelection, &model, &iter) &&
     (OptionName != "")){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setOption(OptionName, val);
    //    modifiedWord(id);
    dictionary->modifyWord(id);
  }
}

void KasumiMainWindow::SwitchToAddingMode(){
  new KasumiAddWindow(dictionary,conf);
  delete this;
}

void KasumiMainWindow::FindNext(bool fromCurrent){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  KasumiWord *word;
  bool fromFirst = false;
  GtkTreeIter StartIter;
  int id;
  string searchString = string(gtk_entry_get_text(GTK_ENTRY(SearchEntry)));
  string comparedString;
  GtkWidget *dialog;

  SearchBy by = SPELLING;
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SearchBySoundRadio))){
    by = SOUND;
  }

  if(!gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    if(!gtk_tree_model_get_iter_first(model, &iter)){
      // If no words, do nothing.
      return;
    }
    fromFirst = true;
  }
  StartIter = iter;

  if(fromCurrent){
    // Search from the selected world
    // nothing to do ... discourage against calling gtk_tree_model_iter_next
  }else if(!fromFirst && !gtk_tree_model_iter_next(model,&iter)){
    // Search from next word if a certain word is selected.
    // If the selected is the last word, seek from the first word.
    
    // If no words, do nothing;    
    if(!gtk_tree_model_get_iter_first(model, &iter))
      return;

    fromFirst = true;
  }

  // liner search!
  do{
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    word = dictionary->getWordWithID(id);

    if(by == SPELLING){
      comparedString = word->getSpellingByUTF8();
    }else{
      comparedString = word->getSoundByUTF8();
    }

    if(comparedString.find(searchString,0) == 0){
      // if found, select that word and don't search any more
      gtk_tree_selection_select_iter(SortListSelection,&iter);
      return;
    }
  }while(gtk_tree_model_iter_next(model, &iter));

  
  // Unless searched from the first word, seek from the head again.
  if(!fromFirst){
    dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_QUESTION,
                                     GTK_BUTTONS_YES_NO,
                                     _("Cannot find a specific word. Search "
                                       "from first?"));
    if(gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES){
      gtk_widget_destroy (dialog);
    
      if(!gtk_tree_model_get_iter_first(model, &iter))
        return;

      // liner search to selected word!
      do{
        gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
        word = dictionary->getWordWithID(id);

        if(by == SPELLING){
          comparedString = word->getSpellingByUTF8();
        }else{
          comparedString = word->getSoundByUTF8();
        }

        if(comparedString.find(searchString,0) == 0){
          gtk_tree_selection_select_iter(SortListSelection,&iter);
          return;
        }
      }while(gtk_tree_model_iter_next(model, &iter) &&
             (StartIter.user_data != iter.user_data ||
              StartIter.user_data2 != iter.user_data2 ||
              StartIter.user_data3 != iter.user_data3));
    }else{
      gtk_widget_destroy(dialog);
      //      gtk_tree_selection_unselect_all(SortListSelection);      
      return;
    }
  }

  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_WARNING,
                                  GTK_BUTTONS_OK,
                                  _("Cannot find a specific word."));
  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
  //  gtk_tree_selection_unselect_all(SortListSelection);
}

void KasumiMainWindow::SortBy(GtkTreeViewColumn *column){
  GtkSortType order;

  order = gtk_tree_view_column_get_sort_order(column);
  order = (order == GTK_SORT_ASCENDING) ?
    GTK_SORT_DESCENDING : GTK_SORT_ASCENDING;
  
  gtk_tree_view_column_set_sort_indicator(SoundColumn,FALSE);
  gtk_tree_view_column_set_sort_indicator(FreqColumn,FALSE);
  gtk_tree_view_column_set_sort_indicator(WordClassColumn,FALSE);

  if(column == SoundColumn){
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(SortList),
                                         COL_YOMI, order);
  }else if(column == FreqColumn){
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(SortList),
                                         COL_FREQ, order);
  }else if(column == WordClassColumn){
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(SortList),
                                         COL_PART, order);
  }
  gtk_tree_view_column_set_sort_indicator(column,TRUE);
  gtk_tree_view_column_set_sort_order(column,order);
}

void KasumiMainWindow::removedWord(int id){
  refresh();
  modificationFlag = true;  
}

void KasumiMainWindow::appendedWord(int id){
  GtkTreeIter iter;
  KasumiWord *word = dictionary->getWordWithID(id);
  
  gtk_list_store_append(WordList,&iter);

  // activate disabled widget
  gtk_widget_set_sensitive(SpellingEntry,true);
  gtk_widget_set_sensitive(SoundEntry,true);
  gtk_widget_set_sensitive(FrequencySpin,true);
  gtk_widget_set_sensitive(WordClassCombo,true);
  gtk_widget_set_sensitive(NounOptionSaConnectionCheck,true);
  gtk_widget_set_sensitive(NounOptionNaConnectionCheck,true);
  gtk_widget_set_sensitive(NounOptionSuruConnectionCheck,true);
  gtk_widget_set_sensitive(NounOptionGokanCheck,true);
  gtk_widget_set_sensitive(NounOptionKakujoshiConnectionCheck,true);
  
  gtk_list_store_set(WordList, &iter,
                     COL_ID, id,
                     COL_WORD, word->getSpellingByUTF8().c_str(),
                     COL_YOMI, word->getSoundByUTF8().c_str(),
                     COL_FREQ, word->getFrequency(),
                     COL_PART, word->getStringOfWordClassByUTF8().c_str(),
                     -1);
  GtkTreeIter sort_iter;
  gtk_tree_model_sort_convert_child_iter_to_iter(GTK_TREE_MODEL_SORT(SortList),
                                                 &sort_iter,&iter);
  gtk_tree_selection_select_iter(SortListSelection,&sort_iter);

  GtkAdjustment *adjustment =
    gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(ScrolledWindow));
  gtk_adjustment_set_value(adjustment, adjustment->upper);
  modificationFlag = true;  
}

void KasumiMainWindow::modifiedWord(int id){
  GtkTreeIter *iter = findCorrespondingIter(id);

  if(iter != NULL){
    KasumiWord *word = dictionary->getWordWithID(id);
    
    gtk_list_store_set(WordList,iter,
                       COL_ID,id,
                       COL_WORD,word->getSpellingByUTF8().c_str(),
                       COL_YOMI,word->getSoundByUTF8().c_str(),
                       COL_FREQ,word->getFrequency(),
                       COL_PART,word->getStringOfWordClassByUTF8().c_str(),
                       -1);
  }
  modificationFlag = true;  
}

// Do not returns iter of SortList but WordList
GtkTreeIter *KasumiMainWindow::findCorrespondingIter(int id){
  int i;
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter *iter = (GtkTreeIter*)calloc(1,sizeof(GtkTreeIter));

  gtk_tree_model_get_iter_first(model, iter);
  do{
    gtk_tree_model_get(model, iter, COL_ID, &i, -1);
    if(id == i){
      return iter;
    }
  }while(gtk_tree_model_iter_next(model, iter));

  return NULL;
}

void KasumiMainWindow::setActiveVerbType(VerbType type){
  switch(type){
  case B5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),0);
    break;
  case G5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),1);
    break;
  case K5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),2);
    break;
  case M5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),3);
    break;
  case N5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),4);
    break;
  case R5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),5);
    break;
  case S5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),6);
    break;
  case T5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),7);
    break;
  case W5:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),8);
    break;
  default:
    gtk_combo_box_set_active(GTK_COMBO_BOX(VerbTypeCombo),0);
    break;
  }
}

VerbType KasumiMainWindow::getActiveVerbType(){
  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(VerbTypeCombo))){
  case 0:
    return B5;
  case 1:
    return G5;
  case 2:
    return K5;
  case 3:
    return M5;
  case 4:
    return N5;
  case 5:
    return R5;
  case 6:
    return S5;
  case 7:
    return T5;
  case 8:
    return W5;
  default:
    return B5;
  }
}

void KasumiMainWindow::setActiveWordClass(WordClassType type){
  switch(type){
  case NOUN:
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),0);
    break;
  case ADV:
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),1);
    break;
  case PERSON:
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),2);
    break;
  case PLACE:
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),3);
    break;
  case ADJ:
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),4);
    break;
  case VERB:
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),5);
    break;
  default:
    gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo),0);
    break;
  }
}

WordClassType KasumiMainWindow::getActiveWordClass(){
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
  case 5:
    return VERB;
  default:
    return NOUN;
  }
}

void KasumiMainWindow::synchronizeOptionCheckButton(KasumiWord *word){
  if(word == NULL){
    g_signal_handler_block(NounOptionSaConnectionCheck,
                           HandlerIDOfNounOptionSaConnectionCheck);
    g_signal_handler_block(NounOptionNaConnectionCheck,
                           HandlerIDOfNounOptionNaConnectionCheck);
    g_signal_handler_block(NounOptionSuruConnectionCheck,
                           HandlerIDOfNounOptionSuruConnectionCheck);
    g_signal_handler_block(NounOptionGokanCheck,
                           HandlerIDOfNounOptionGokanCheck);
    g_signal_handler_block(NounOptionKakujoshiConnectionCheck,
                           HandlerIDOfNounOptionKakujoshiConnectionCheck);
      
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
    
    g_signal_handler_unblock(NounOptionSaConnectionCheck,
                             HandlerIDOfNounOptionSaConnectionCheck);
    g_signal_handler_unblock(NounOptionNaConnectionCheck,
                             HandlerIDOfNounOptionNaConnectionCheck);
    g_signal_handler_unblock(NounOptionSuruConnectionCheck,
                             HandlerIDOfNounOptionSuruConnectionCheck);
    g_signal_handler_unblock(NounOptionGokanCheck,
                             HandlerIDOfNounOptionGokanCheck);
    g_signal_handler_unblock(NounOptionKakujoshiConnectionCheck,
                             HandlerIDOfNounOptionKakujoshiConnectionCheck);
    return;
  }
  
  if(word->getWordClass() == NOUN){
    g_signal_handler_block(NounOptionSaConnectionCheck,
                           HandlerIDOfNounOptionSaConnectionCheck);
    g_signal_handler_block(NounOptionNaConnectionCheck,
                           HandlerIDOfNounOptionNaConnectionCheck);
    g_signal_handler_block(NounOptionSuruConnectionCheck,
                           HandlerIDOfNounOptionSuruConnectionCheck);
    g_signal_handler_block(NounOptionGokanCheck,
                           HandlerIDOfNounOptionGokanCheck);
    g_signal_handler_block(NounOptionKakujoshiConnectionCheck,
                           HandlerIDOfNounOptionKakujoshiConnectionCheck);
      
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionSaConnectionCheck),
      word->getOption(string(EUCJP_SASETSUZOKU)));
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionNaConnectionCheck),
      word->getOption(string(EUCJP_NASETSUZOKU)));
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionSuruConnectionCheck),
      word->getOption(string(EUCJP_SURUSETSUZOKU)));
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionGokanCheck),
      word->getOption(string(EUCJP_GOKANNNOMIDEBUNNSETSU)));
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(NounOptionKakujoshiConnectionCheck),
      word->getOption(string(EUCJP_KAKUJOSHISETSUZOKU)));
    
    g_signal_handler_unblock(NounOptionSaConnectionCheck,
                             HandlerIDOfNounOptionSaConnectionCheck);
    g_signal_handler_unblock(NounOptionNaConnectionCheck,
                             HandlerIDOfNounOptionNaConnectionCheck);
    g_signal_handler_unblock(NounOptionSuruConnectionCheck,
                             HandlerIDOfNounOptionSuruConnectionCheck);
    g_signal_handler_unblock(NounOptionGokanCheck,
                             HandlerIDOfNounOptionGokanCheck);
    g_signal_handler_unblock(NounOptionKakujoshiConnectionCheck,
                             HandlerIDOfNounOptionKakujoshiConnectionCheck);
  }else if(word->getWordClass() == ADV){
    g_signal_handler_block(AdvOptionToConnectionCheck,
                           HandlerIDOfAdvOptionToConnectionCheck);
    g_signal_handler_block(AdvOptionTaruConnectionCheck,
                           HandlerIDOfAdvOptionTaruConnectionCheck);
    g_signal_handler_block(AdvOptionSuruConnectionCheck,
                           HandlerIDOfAdvOptionSuruConnectionCheck);
    g_signal_handler_block(AdvOptionGokanCheck,
                           HandlerIDOfAdvOptionGokanCheck);
      
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionToConnectionCheck),
      word->getOption(string(EUCJP_TOSETSUZOKU)));
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionTaruConnectionCheck),
      word->getOption(string(EUCJP_TARUSETSUZOKU)));
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionSuruConnectionCheck),
      word->getOption(string(EUCJP_SURUSETSUZOKU)));
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(AdvOptionGokanCheck),
      word->getOption(string(EUCJP_GOKANNNOMIDEBUNNSETSU)));

    g_signal_handler_unblock(AdvOptionToConnectionCheck,
                             HandlerIDOfAdvOptionToConnectionCheck);
    g_signal_handler_unblock(AdvOptionTaruConnectionCheck,
                             HandlerIDOfAdvOptionTaruConnectionCheck);
    g_signal_handler_unblock(AdvOptionSuruConnectionCheck,
                             HandlerIDOfAdvOptionSuruConnectionCheck);
    g_signal_handler_unblock(AdvOptionGokanCheck,
                             HandlerIDOfAdvOptionGokanCheck);
  }else if(word->getWordClass() == VERB){
    g_signal_handler_block(VerbOptionRentaiCheck,
                           HandlerIDOfVerbOptionRentaiCheck);
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(VerbOptionRentaiCheck),
      word->getOption(string(EUCJP_RENNYOUKEINOMEISHIKA)));
    g_signal_handler_unblock(VerbOptionRentaiCheck,
                             HandlerIDOfVerbOptionRentaiCheck);

  }
}

void KasumiMainWindow::flipOptionPane(){
  if(getActiveWordClass() == NOUN){
    gtk_widget_show(NounOptionPane);
    gtk_widget_hide(AdvOptionPane);
    gtk_widget_hide(VerbOptionPane);
  }else if(getActiveWordClass() == ADV){
    gtk_widget_hide(NounOptionPane);
    gtk_widget_show(AdvOptionPane);
    gtk_widget_hide(VerbOptionPane);
  }else if(getActiveWordClass() == VERB){
    gtk_widget_hide(NounOptionPane);
    gtk_widget_hide(AdvOptionPane);
    gtk_widget_show(VerbOptionPane);
  }else{
    gtk_widget_hide(NounOptionPane);
    gtk_widget_hide(AdvOptionPane);
    gtk_widget_hide(VerbOptionPane);
    
  }
}

void _call_back_delete_event(GtkWidget *widget,
                             GdkEvent *event,
                             gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->quit();
}

void _call_back_quit(GtkWidget *widget,
                     gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->quit();
}

void _call_back_store(GtkWidget *widget,
                      gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedStoreButton();
}

void _call_back_add(GtkWidget *widget,
                    gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedAddButton();
}

void _call_back_remove(GtkWidget *widget,
                       gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedRemoveButton();
}

void _call_back_adding_mode(GtkWidget *widget,
                            gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->SwitchToAddingMode();
}

void _call_back_changed_list_cursor(GtkWidget *widget,
                                    gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedListCursor();
}

void _call_back_changed_sound_entry(GtkWidget *widget,
                                    gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedSoundEntry();
}

void _call_back_changed_spelling_entry(GtkWidget *widget,
                                       gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedSpellingEntry();
}

void _call_back_changed_frequency_spin(GtkWidget *widget,
                                       gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedFrequencySpin();
}

void _call_back_changed_word_class_combo(GtkWidget *widget,
                                         gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedWordClassCombo();
}

void _call_back_changed_verb_type_combo(GtkWidget *widget,
                                         gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedVerbTypeCombo();
}

void _call_back_toggled_check(GtkWidget *widget,
                              gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedOption(widget);
}

void _call_back_changed_search_entry(GtkWidget *widget,
                                       gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->FindNext(true);
}

void _call_back_activate_search_entry(GtkWidget *widget,
                                       gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->FindNext(false);
}

void _call_back_clicked_column_header(GtkTreeViewColumn *column,
                                             gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->SortBy(column);
}


guint getAccelKey(const string &key){
  string::size_type i;

  i = key.find("+",0);

  string shortkey = key.substr(i+1);

  if(shortkey == "A") return GDK_A;
  else if(shortkey == "B") return GDK_B;
  else if(shortkey == "C") return GDK_C;
  else if(shortkey == "D") return GDK_D;
  else if(shortkey == "E") return GDK_E;
  else if(shortkey == "F") return GDK_F;
  else if(shortkey == "G") return GDK_G;
  else if(shortkey == "H") return GDK_H;
  else if(shortkey == "I") return GDK_I;
  else if(shortkey == "J") return GDK_J;
  else if(shortkey == "K") return GDK_K;
  else if(shortkey == "L") return GDK_L;
  else if(shortkey == "M") return GDK_M;
  else if(shortkey == "N") return GDK_N;
  else if(shortkey == "O") return GDK_O;
  else if(shortkey == "P") return GDK_P;
  else if(shortkey == "Q") return GDK_Q;
  else if(shortkey == "R") return GDK_R;
  else if(shortkey == "S") return GDK_S;
  else if(shortkey == "T") return GDK_T;
  else if(shortkey == "U") return GDK_U;
  else if(shortkey == "V") return GDK_V;
  else if(shortkey == "W") return GDK_W;
  else if(shortkey == "X") return GDK_X;
  else if(shortkey == "Y") return GDK_Y;
  else if(shortkey == "Z") return GDK_Z;
  else if(shortkey == "0") return GDK_0;
  else if(shortkey == "1") return GDK_1;
  else if(shortkey == "2") return GDK_2;
  else if(shortkey == "3") return GDK_3;
  else if(shortkey == "4") return GDK_4;
  else if(shortkey == "5") return GDK_5;
  else if(shortkey == "6") return GDK_5;
  else if(shortkey == "7") return GDK_7;
  else if(shortkey == "8") return GDK_8;
  else if(shortkey == "9") return GDK_9;
  
  cout << "Invalid shortcut key option: " << key << endl;
  exit(1);
}

GdkModifierType getModifierType(const string &key){
  string::size_type i;

  i = key.find("+",0);

  if(i == key.npos){
    return (GdkModifierType)0;
  }

  string mask = key.substr(0,i);
  if(mask == "Ctrl"){
    return GDK_CONTROL_MASK;
  }else if(mask == "Alt"){
    return GDK_MOD1_MASK;
  }

  cout << "Invalid mask option: " << key << endl;
  exit(1);
}

gint sortFuncByFreq(GtkTreeModel *model,
                    GtkTreeIter *a,
                    GtkTreeIter *b,
                    gpointer user_data){
  KasumiDic *dictionary = (KasumiDic*)user_data;
  int id_a, id_b;
  gtk_tree_model_get(model, a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, b, COL_ID, &id_b, -1);
  KasumiWord *word_a = dictionary->getWordWithID(id_a);
  KasumiWord *word_b = dictionary->getWordWithID(id_b);

  return word_a->getFrequency() - word_b->getFrequency();
}

gint sortFuncBySound(GtkTreeModel *model,
                     GtkTreeIter *iter_a,
                     GtkTreeIter *iter_b,
                     gpointer user_data){
  KasumiDic *dictionary = (KasumiDic*)user_data;
  int id_a, id_b;
  gtk_tree_model_get(model, iter_a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, iter_b, COL_ID, &id_b, -1);
  KasumiWord *word_a = dictionary->getWordWithID(id_a);
  KasumiWord *word_b = dictionary->getWordWithID(id_b);
  const char *str_a = word_a->getSoundByUTF8().c_str();
  const char *str_b = word_b->getSoundByUTF8().c_str();
  int size_a = word_a->getSound().size();
  int size_b = word_b->getSound().size();
  int size = (size_a > size_b) ? size_a : size_b;
  int i,a,b;
  unsigned char first_a,second_a,third_a;
  unsigned char first_b,second_b,third_b;
  
  // compare Hiragana string encoded in UTF8
  for(i=0;i<size;i+=3){
    first_a = static_cast<unsigned char> (str_a[i]);
    second_a = static_cast<unsigned char> (str_a[i+1]);
    third_a = static_cast<unsigned char> (str_a[i+2]);
    first_b = static_cast<unsigned char> (str_b[i]);
    second_b = static_cast<unsigned char> (str_b[i+1]);
    third_b = static_cast<unsigned char> (str_b[i+2]);

    // confirm that current characters are Hiragana
    if(first_a == 0xe3 && first_b == 0xe3){
      if((second_a == 0x81 && third_a >= 0x81 && third_a <= 0xbf) ||
         (second_a == 0x82 && third_a >= 0x80 && third_a <= 0x94) ||
         (second_a == 0x83 && third_a == 0xbc)){
        a = second_a * 256 + third_a;
      }else{
        cout << "invalid character or not hiragana" << endl;
        cout << str_a << endl;
        exit(1);
      }

      if((second_b == 0x81 && third_b >= 0x81 && third_b <= 0xbf) ||
         (second_b == 0x82 && third_b >= 0x80 && third_b <= 0x94) ||
         (second_b == 0x83 && third_b == 0xbc)){
        b = second_b * 256 + third_b;
      }else{
        cout << "invalid character or not hiragana" << endl;
        cout << str_b << endl;
        exit(1);
      }

      if(a != b){
        return a - b;
      }
    }else{
        cout << "invalid character or not hiragana" << endl;
        cout << str_a << endl;
        cout << str_b << endl;
        exit(1);
    }
  }
  
  // one string is the beginning part of another string
  // compare string size
  return size_a - size_b;
}

gint sortFuncByWordClass(GtkTreeModel *model,
                         GtkTreeIter *iter_a,
                         GtkTreeIter *iter_b,
                         gpointer user_data){
  KasumiDic *dictionary = (KasumiDic*)user_data;
  int id_a, id_b;
  gtk_tree_model_get(model, iter_a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, iter_b, COL_ID, &id_b, -1);
  KasumiWord *word_a = dictionary->getWordWithID(id_a);
  KasumiWord *word_b = dictionary->getWordWithID(id_b);

  return word_a->getWordClass() - word_b->getWordClass();
}
