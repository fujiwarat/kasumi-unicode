#include <gtk/gtk.h>
#include <iostream>
#include "KasumiMainWindow.hxx"
#include "KasumiException.hxx"
#include "intl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiMainWindow::KasumiMainWindow(KasumiDic *aDictionary){
  dictionary = aDictionary;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), _("Kasumi"));
  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(_call_back_delete_event), this);
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(_call_back_destroy), this);

  /* creating top vbox */
  GtkWidget *vbox = gtk_vbox_new(FALSE,8);
  gtk_container_set_border_width(GTK_CONTAINER(vbox),8);
  gtk_container_add(GTK_CONTAINER(window),vbox);

  /* creating hbox for wordlist and text entries */
  GtkWidget *hbox = gtk_hbox_new(FALSE,8);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox),TRUE,TRUE,0);

  /* creating scrolled window for wordlist */
  GtkWidget *scroll = gtk_scrolled_window_new(NULL,NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                 GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(GTK_WIDGET(scroll),300,300);
  gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(scroll),TRUE,TRUE,0);


  /* creating tree(list) view for words */
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
  gtk_tree_view_set_model(GTK_TREE_VIEW(WordListView),GTK_TREE_MODEL(WordList));

  WordListSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(WordListView));
  gtk_tree_selection_set_mode(WordListSelection, GTK_SELECTION_BROWSE);
  g_signal_connect(G_OBJECT(WordListSelection), "changed",
                   G_CALLBACK(_call_back_changed_list_cursor), this);

  /* destroy model automatically with view */  
  g_object_unref(GTK_TREE_MODEL(WordList));

  gtk_container_add(GTK_CONTAINER(scroll),GTK_WIDGET(WordListView));  

  /* creating vbox for text entries */
  GtkWidget *entry_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(entry_vbox),FALSE,FALSE,0);

  /* creating text entries for "Word" */
  GtkWidget *label = gtk_label_new(_("Spelling"));
  GtkWidget *alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  SpellingEntry = gtk_entry_new();
  HandlerIDOfSpellingEntry = g_signal_connect(G_OBJECT(SpellingEntry), "changed",
                   G_CALLBACK(_call_back_changed_spelling_entry), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(SpellingEntry),FALSE,FALSE,0);

  /* creating text entries for "Yomi" */
  label = gtk_label_new(_("Sound"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  SoundEntry = gtk_entry_new();
  HandlerIDOfSoundEntry = g_signal_connect(G_OBJECT(SoundEntry), "changed",
               G_CALLBACK(_call_back_changed_sound_entry), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(SoundEntry),FALSE,FALSE,0);

  /* creating spin button for "Frequency" */
  label = gtk_label_new(_("Frequency"));
  alignment = gtk_alignment_new(0,0.5,0,0);
  gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

  GtkObject *adjustment = gtk_adjustment_new(1,1,32767,1,100,0);
  FrequencySpin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment),1.0,0);
  HandlerIDOfFrequencySpin = g_signal_connect(G_OBJECT(FrequencySpin), "value-changed",
                   G_CALLBACK(_call_back_changed_frequency_spin), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(FrequencySpin),FALSE,FALSE,0);

  /* creating combo box for "Word Class" */
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
  HandlerIDOfWordClassCombo = g_signal_connect(G_OBJECT(WordClassCombo), "changed",
                   G_CALLBACK(_call_back_changed_word_class_combo), this);
  gtk_box_pack_start(GTK_BOX(entry_vbox),GTK_WIDGET(WordClassCombo),FALSE,FALSE,0);

  /* creating box for buttons */
  GtkWidget *hbutton_box = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(hbutton_box),GTK_BUTTONBOX_SPREAD);
  gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbutton_box),FALSE,FALSE,0);

  /* creating buttons */
  GtkWidget *button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Quit"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_quit),this);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Store"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_store),this);

  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Add"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_add),this);
  
  button = gtk_button_new();
  gtk_button_set_label(GTK_BUTTON(button),_("Remove"));
  gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),FALSE,FALSE,0);
  g_signal_connect(G_OBJECT(button),"clicked",
                   G_CALLBACK(_call_back_remove),this);
  
  gtk_widget_show_all(window);

  refresh();

  dictionary->registerEventListener(this);
}

void KasumiMainWindow::refresh(){
  int i = 0;

  gtk_list_store_clear(WordList);
  
  for(i=0;i<=dictionary->getUpperBoundOfWordID();i++){
    try{
      KasumiWord *word = dictionary->getWordWithID(i);

      if(word != NULL){
        GtkTreeIter iter;
        GtkTreePath *path;
      
        gtk_list_store_append(WordList,&iter);
      
        gtk_list_store_set(WordList, &iter,
                           COL_ID, i,
                           COL_WORD, word->getSpellingByUTF8().c_str(),
                           COL_YOMI, word->getSoundByUTF8().c_str(),
                           COL_FREQ, word->getFrequency(),
                           COL_PART, word->getStringOfWordClassByUTF8().c_str(),
                           -1);
      }
    }catch(KasumiOutOfBoundException e){
      cout << e.getMessage() << endl;
      break;
    }
  }
}

void KasumiMainWindow::destroy(GtkWidget *widget){
  gtk_main_quit();
}

gboolean KasumiMainWindow::delete_event(GtkWidget *widget, GdkEvent *event){
  return FALSE;
}

void KasumiMainWindow::ClickedQuitButton(GtkWidget *widget){
  if(!delete_event(widget,NULL)){
    gtk_main_quit();
  }
}

void KasumiMainWindow::ClickedStoreButton(GtkWidget *widget){
  dictionary->store();
}

void KasumiMainWindow::ClickedAddButton(GtkWidget *widget){
  KasumiWord *word = new KasumiWord();
  word->setSpellingByUTF8(string(gtk_entry_get_text(GTK_ENTRY(SpellingEntry))));
  word->setSoundByUTF8(string(gtk_entry_get_text(GTK_ENTRY(SoundEntry))));
  word->setFrequency(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(FrequencySpin)));
  word->setWordClass(getActiveWordClass());

  dictionary->appendWord(word);
}

void KasumiMainWindow::ClickedRemoveButton(GtkWidget *widget){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    dictionary->removeWord(id);
  }
}

void KasumiMainWindow::ChangedListCursor(GtkWidget *widget){
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
                           
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry),word->getSpellingByUTF8().c_str());
    gtk_entry_set_text(GTK_ENTRY(SoundEntry),word->getSoundByUTF8().c_str());
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),word->getFrequency());
    setActiveWordClass(word->getWordClass());

    g_signal_handler_unblock(SoundEntry, HandlerIDOfSoundEntry);
    g_signal_handler_unblock(SpellingEntry, HandlerIDOfSpellingEntry);
    g_signal_handler_unblock(FrequencySpin, HandlerIDOfFrequencySpin);
    g_signal_handler_unblock(WordClassCombo, HandlerIDOfWordClassCombo);  
  }
}

void KasumiMainWindow::ChangedSoundEntry(GtkWidget *widget){
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

void KasumiMainWindow::ChangedSpellingEntry(GtkWidget *widget){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setSpellingByUTF8(string(gtk_entry_get_text(GTK_ENTRY(SpellingEntry))));
    modifiedWord(id);
  }
}

void KasumiMainWindow::ChangedFrequencySpin(GtkWidget *widget){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setFrequency(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(FrequencySpin)));
    modifiedWord(id);
  }
}

void KasumiMainWindow::ChangedWordClassCombo(GtkWidget *widget){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter iter;
  int id;
  
  if(gtk_tree_selection_get_selected(WordListSelection, &model, &iter)){
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
    KasumiWord *word = dictionary->getWordWithID(id);
    word->setWordClass(getActiveWordClass());
    modifiedWord(id);
  }
}

void KasumiMainWindow::removedWord(int id){
  refresh();
}

void KasumiMainWindow::appendedWord(int id){
  refresh();
}

void KasumiMainWindow::modifiedWord(int id){
  GtkTreeModel *model = GTK_TREE_MODEL(WordList);
  GtkTreeIter *iter = findCorrespondingIter(id);

  if(iter != NULL){
    KasumiWord *word = dictionary->getWordWithID(id);
    
    gtk_list_store_set(WordList, iter,
                       COL_ID, id,
                       COL_WORD, word->getSpellingByUTF8().c_str(),
                       COL_YOMI, word->getSoundByUTF8().c_str(),
                       COL_FREQ, word->getFrequency(),
                       COL_PART, word->getStringOfWordClassByUTF8().c_str(),
                       -1);
  }
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

void _call_back_destroy(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->destroy(widget);
}

gboolean _call_back_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  return window->delete_event(widget, event);
}

void _call_back_quit(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedQuitButton(widget);
}

void _call_back_store(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedStoreButton(widget);
}

void _call_back_add(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedAddButton(widget);
}

void _call_back_remove(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ClickedRemoveButton(widget);
}

void _call_back_changed_list_cursor(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedListCursor(widget);
}

void _call_back_changed_sound_entry(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedSoundEntry(widget);
}

void _call_back_changed_spelling_entry(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedSpellingEntry(widget);
}

void _call_back_changed_frequency_spin(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedFrequencySpin(widget);
}

void _call_back_changed_word_class_combo(GtkWidget *widget, gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->ChangedWordClassCombo(widget);
}
