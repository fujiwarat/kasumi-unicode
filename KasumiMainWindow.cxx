#include <gtk/gtk.h>
#include <iostream>
#include "KasumiMainWindow.hxx"
#include "KasumiException.hxx"
#include "KasumiWord.hxx"
#include "KasumiDic.hxx"
#include "KasumiString.hxx"
#include "KasumiConfiguration.hxx"
#include <gdk/gdkkeysyms.h>
#include "intl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiMainWindow::KasumiMainWindow(KasumiDic *aDictionary,
                                   KasumiConfiguration *conf){
  dictionary = aDictionary;
  modificationFlag = false;
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), _("Kasumi"));
  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(_call_back_delete_event), this);
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(_call_back_destroy), this);

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
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(WordListView),-1,
                                              _("Spelling"),renderer,
                                              "text",COL_WORD,
                                              NULL);
  gtk_tree_view_column_new();
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(WordListView),-1,
                                              _("Sound"),renderer,
                                              "text",COL_YOMI,
                                              NULL);
  gtk_tree_view_column_new();  
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(WordListView),-1,
                                              _("Frequency"),renderer,
                                              "text",COL_FREQ,
                                              NULL);
  gtk_tree_view_column_new();  
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(WordListView),-1,
                                              _("Word Class"),renderer,
                                              "text",COL_PART,
                                              NULL);
  WordList = gtk_list_store_new(NUM_COLS,G_TYPE_UINT,G_TYPE_STRING,
                                G_TYPE_STRING,G_TYPE_UINT,G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(WordListView),
                          GTK_TREE_MODEL(WordList));

  WordListSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(WordListView));
  gtk_tree_selection_set_mode(WordListSelection, GTK_SELECTION_SINGLE);
  g_signal_connect(G_OBJECT(WordListSelection), "changed",
                   G_CALLBACK(_call_back_changed_list_cursor), this);

  // destroy model automatically with view
  g_object_unref(GTK_TREE_MODEL(WordList));

  gtk_container_add(GTK_CONTAINER(ScrolledWindow),GTK_WIDGET(WordListView));  

  // creating vbox for text entries
  GtkWidget *entry_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(entry_vbox),FALSE,FALSE,0);

  // creating text entries for "Spelling"
  GtkWidget *label = gtk_label_new(_("Spelling"));
  GtkWidget *alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

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
                     0);

  // creating text entries for "Sound"
  label = gtk_label_new(_("Sound"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  SoundEntry = gtk_entry_new();
  HandlerIDOfSoundEntry = g_signal_connect(G_OBJECT(SoundEntry), "changed",
               G_CALLBACK(_call_back_changed_sound_entry), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(SoundEntry),FALSE,FALSE,0);

  // creating spin button for "Frequency"
  label = gtk_label_new(_("Frequency"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

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
                     0);

  // creating combo box for "Word Class"
  label = gtk_label_new(_("Word Class"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);
  
  WordClassCombo = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Noun"));  
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Adverb"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Person's name"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Place-name"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(WordClassCombo), _("Adjective"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(WordClassCombo), 0);
  HandlerIDOfWordClassCombo =
    g_signal_connect(G_OBJECT(WordClassCombo), "changed",
                     G_CALLBACK(_call_back_changed_word_class_combo), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),
                     GTK_WIDGET(WordClassCombo),FALSE,FALSE,0);

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

  // creating box for search function
  GtkWidget *search_hbox = gtk_hbox_new(FALSE,8);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(search_hbox),FALSE,FALSE,0);

  // creating Entry and Button for search function
  SearchEntry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(search_hbox),
                     GTK_WIDGET(SearchEntry),
                     FALSE,
                     FALSE,
                     0);

  GtkWidget *button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Find Next By Sound"));
  gtk_box_pack_start(GTK_BOX(search_hbox),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_find_next_by_sound),this);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Find Next By Spelling"));
  gtk_box_pack_start(GTK_BOX(search_hbox),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_find_next_by_spelling),this);

  PrefixSearchCheck = gtk_check_button_new_with_label(_("Prefix Search"));
  gtk_box_pack_start(GTK_BOX(search_hbox),GTK_WIDGET(PrefixSearchCheck),
                     FALSE,FALSE,0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(PrefixSearchCheck),true);
  

  /* creating box for buttons */
  GtkWidget *hbutton_box = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(hbutton_box),GTK_BUTTONBOX_SPREAD);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbutton_box),FALSE,FALSE,0);


  // creating buttons and shortcut key configuration
  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Quit"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_quit),this);
  GtkAccelGroup *accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  gtk_widget_add_accelerator(button, "clicked", accel,
                             GDK_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Store"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_store),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  gtk_widget_add_accelerator(button, "clicked", accel,
                             GDK_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Add"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_add),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  gtk_widget_add_accelerator(button, "clicked", accel,
                             GDK_A, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  
  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Remove"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_remove),this);
  accel = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel);
  gtk_widget_add_accelerator(button, "clicked", accel,
                             GDK_D, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  
  gtk_widget_show_all(window);
  gtk_widget_hide(AdvOptionPane);

  refresh();

  dictionary->registerEventListener(this);
}

void KasumiMainWindow::refresh(){
  int i = 0;

  gtk_list_store_clear(WordList);
  
  for(i=0;i<=dictionary->getUpperBoundOfWordID();i++){
    try{
      KasumiWord *word = dictionary->getWordWithID(i);

      if(word != NULL && word->getFrequency() != 0){
        GtkTreeIter iter;
      
        gtk_list_store_append(WordList,&iter);
      
        gtk_list_store_set(WordList,&iter,
                           COL_ID,i,
                           COL_WORD,word->getSpellingByUTF8().c_str(),
                           COL_YOMI,word->getSoundByUTF8().c_str(),
                           COL_FREQ,word->getFrequency(),
                           COL_PART,word->getStringOfWordClassByUTF8().c_str(),
                           -1);
      }
    }catch(KasumiOutOfBoundException e){
      cout << e.getMessage() << endl;
      exit(1);
    }
  }
}

void KasumiMainWindow::destroy(){
  gtk_main_quit();
}

gboolean KasumiMainWindow::delete_event(GdkEvent *event){
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
  return FALSE;
}

void KasumiMainWindow::ClickedQuitButton(){
  if(!delete_event(NULL)){
    destroy();
  }
}

void KasumiMainWindow::ClickedStoreButton(){
  try{
    dictionary->store();
    modificationFlag = false;
  }catch(KasumiDicStoreException e){
      cout <<  e.getMessage() << endl;
      exit(1);
  }
}

void KasumiMainWindow::ClickedAddButton(){
  KasumiWord *word = new KasumiWord();
  word->setSoundByUTF8(string(gtk_entry_get_text(
    GTK_ENTRY(SoundEntry))));
  word->setSpellingByUTF8(string(gtk_entry_get_text(
    GTK_ENTRY(SpellingEntry))));
  word->setFrequency(gtk_spin_button_get_value_as_int(
    GTK_SPIN_BUTTON(FrequencySpin)));
  word->setWordClass(getActiveWordClass());

  dictionary->appendWord(word);
}

void KasumiMainWindow::ClickedRemoveButton(){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    dictionary->removeWord(id);
  }
}

void KasumiMainWindow::ChangedListCursor(){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);

    g_signal_handler_block(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_block(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_block(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_block(WordClassCombo, HandlerIDOfWordClassCombo);  
                           
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry),
                       word->getSpellingByUTF8().c_str());
    gtk_entry_set_text(GTK_ENTRY(SoundEntry),
                       word->getSoundByUTF8().c_str());
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),
                              word->getFrequency());
    setActiveWordClass(word->getWordClass());

    g_signal_handler_unblock(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_unblock(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_unblock(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_unblock(WordClassCombo, HandlerIDOfWordClassCombo);

    synchronizeOptionCheckButton(word);
  }else{
    g_signal_handler_block(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_block(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_block(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_block(WordClassCombo, HandlerIDOfWordClassCombo);  
    
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry),"");
    gtk_entry_set_text(GTK_ENTRY(SoundEntry),"");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),FREQ_DEFAULT);
    setActiveWordClass(NOUN);
    
    g_signal_handler_unblock(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_unblock(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_unblock(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_unblock(WordClassCombo, HandlerIDOfWordClassCombo);
    
    synchronizeOptionCheckButton(NULL);
  }

  flipOptionPane();
}

void KasumiMainWindow::ChangedSoundEntry(){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setSoundByUTF8(string(gtk_entry_get_text(GTK_ENTRY(SoundEntry))));
    modifiedWord(id);
  }
}

void KasumiMainWindow::ChangedSpellingEntry(){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setSpellingByUTF8(string(gtk_entry_get_text(
      GTK_ENTRY(SpellingEntry))));
    modifiedWord(id);
  }
}

void KasumiMainWindow::ChangedFrequencySpin(){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setFrequency(gtk_spin_button_get_value_as_int(
      GTK_SPIN_BUTTON(FrequencySpin)));
    modifiedWord(id);
  }
}

void KasumiMainWindow::ChangedWordClassCombo(){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setWordClass(getActiveWordClass());
    synchronizeOptionCheckButton(word);
    modifiedWord(id);
  }

  flipOptionPane();  
}

void KasumiMainWindow::ChangedOption(GtkWidget *widget){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
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
  }

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    val = true;
  else
    val = false;

  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter) &&
     (OptionName != "")){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setOption(OptionName, val);
    modifiedWord(id);
  }
}

void KasumiMainWindow::FindNextBySound(GtkWidget *widget){
  FindNext(SOUND);
}

void KasumiMainWindow::FindNextBySpelling(GtkWidget *widget){
  FindNext(SPELLING);
}

void KasumiMainWindow::FindNext(SearchBy by){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  KasumiWord *word;
  bool fromFirst = false;
  bool prefixSearch =
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(PrefixSearchCheck));
  GtkTreeIter StartIter;
  int id;
  string searchString = string(gtk_entry_get_text(GTK_ENTRY(SearchEntry)));
  string comparedString;
  GtkWidget *dialog;

  if(!gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    if(!gtk_tree_model_get_iter_first(model, &iter)){
      // If no words, do nothing.
      return;
    }
    fromFirst = true;
  }
  StartIter = iter;
  
  // Search from next word if a certain word is selected.
  // If the selected is the last word, seek from the first word.
  if(!fromFirst && !gtk_tree_model_iter_next(model,&iter)){
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

    if((comparedString == searchString) ||
       (prefixSearch && comparedString.find(searchString,0) == 0)){
      // if found, select that word and don't search any more
      gtk_tree_selection_select_iter(WordListSelection,&iter);
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

        if((comparedString == searchString) ||
           (prefixSearch && comparedString.find(searchString,0) == 0)){
          gtk_tree_selection_select_iter(WordListSelection,&iter);
          return;
        }
      }while(gtk_tree_model_iter_next(model, &iter) &&
             (StartIter.user_data != iter.user_data ||
              StartIter.user_data2 != iter.user_data2 ||
              StartIter.user_data3 != iter.user_data3));
    }else{
      gtk_widget_destroy(dialog);
      gtk_tree_selection_unselect_all(WordListSelection);      
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
  gtk_tree_selection_unselect_all(WordListSelection);
}

void KasumiMainWindow::removedWord(int id){
  refresh();
  modificationFlag = true;  
}

void KasumiMainWindow::appendedWord(int id){
  GtkTreeIter iter;
  KasumiWord *word = dictionary->getWordWithID(id);
  
  gtk_list_store_append(WordList,&iter);
  
  gtk_list_store_set(WordList, &iter,
                     COL_ID, id,
                     COL_WORD, word->getSpellingByUTF8().c_str(),
                     COL_YOMI, word->getSoundByUTF8().c_str(),
                     COL_FREQ, word->getFrequency(),
                     COL_PART, word->getStringOfWordClassByUTF8().c_str(),
                     -1);
  gtk_tree_selection_select_iter(WordListSelection,&iter);

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
  }
}

void KasumiMainWindow::flipOptionPane(){
  if(getActiveWordClass() == NOUN){
    gtk_widget_show(NounOptionPane);
    gtk_widget_hide(AdvOptionPane);
  }else if(getActiveWordClass() == ADV){
    gtk_widget_hide(NounOptionPane);
    gtk_widget_show(AdvOptionPane);
  }else{
    gtk_widget_hide(NounOptionPane);
    gtk_widget_hide(AdvOptionPane);
  }
}

void _call_back_destroy(GtkWidget *widget,
                        gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->destroy();
}

gboolean _call_back_delete_event(GtkWidget *widget,
                                 GdkEvent *event,
                                 gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  return window->delete_event(event);
}

void _call_back_quit(GtkWidget *widget,
                     gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedQuitButton();
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

void _call_back_toggled_check(GtkWidget *widget,
                              gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedOption(widget);
}

void _call_back_find_next_by_sound(GtkWidget *widget,
                                   gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->FindNextBySound(widget);
}

void _call_back_find_next_by_spelling(GtkWidget *widget,
                                      gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->FindNextBySpelling(widget);
}

