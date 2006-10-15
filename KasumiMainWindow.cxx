/*********************************************************************
 * 
 * KasumiMainWindow.cxx
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

#include <gtk/gtk.h>
#include "cellrendererspin.h"
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
extern "C"{  // ad-hoc solution for a defect of Anthy
#include "anthy/dicutil.h"
}

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

KasumiMainWindow::KasumiMainWindow(KasumiDic *aDictionary,
                                   KasumiConfiguration *conf){
  this->conf = conf;
  
  dictionary = aDictionary;
  modificationFlag = false;

  createWindow();
  createWordList();
  registerCallbackFunctions();
  gtk_window_set_position (GTK_WINDOW(mWindow), GTK_WIN_POS_MOUSE);
  gtk_window_resize(GTK_WINDOW(mWindow), 300,300); // ToDo: reset window size to be same as the window size when Kasumi quit the last time
  gtk_widget_show(mWindow);

  refresh();

  dictionary->registerEventListener(this);
}

void KasumiMainWindow::createWindow()
{
//  GtkWidget *mWindow;
  GtkWidget *vbox1;
//  GtkWidget *scrolledwindow1;
//  GtkWidget *mWordListView;
  GtkWidget *vbox2;
  GtkWidget *label1;
  GtkWidget *hbox1;
//  GtkWidget *mSpellingRadio;
  GSList *mSpellingRadio_group = NULL;
//  GtkWidget *mSoundRadio;
  GtkWidget *hbox2;
//  GtkWidget *mSearchEntry;
  GtkWidget *alignment6;
  GtkWidget *hbuttonbox1;
//  GtkWidget *mSaveButton;
//  GtkWidget *mAddButton;
//  GtkWidget *mRemoveButton;
//  GtkWidget *mChangeModeButton;
//  GtkWidget *mQuitButton;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  mWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (mWindow), _("Kasumi"));

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (mWindow), vbox1);

  mScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (mScrolledWindow);
  gtk_box_pack_start (GTK_BOX (vbox1), mScrolledWindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (mScrolledWindow), GTK_SHADOW_IN);

  mWordListView = gtk_tree_view_new ();
  gtk_widget_show (mWordListView);
  gtk_container_add (GTK_CONTAINER (mScrolledWindow), mWordListView);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("search"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox2), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label1), 10, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, TRUE, TRUE, 0);

  mSpellingRadio = gtk_radio_button_new_with_mnemonic (NULL, _("Search by Spelling"));
  gtk_widget_show (mSpellingRadio);
  gtk_box_pack_start (GTK_BOX (hbox1), mSpellingRadio, FALSE, FALSE, 20);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (mSpellingRadio), mSpellingRadio_group);
  mSpellingRadio_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (mSpellingRadio));

  mSoundRadio = gtk_radio_button_new_with_mnemonic (NULL, _("Search by Sound"));
  gtk_widget_show (mSoundRadio);
  gtk_box_pack_start (GTK_BOX (hbox1), mSoundRadio, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (mSoundRadio), mSpellingRadio_group);
  mSpellingRadio_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (mSoundRadio));

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, FALSE, 0);

  mSearchEntry = gtk_entry_new ();
  gtk_widget_show (mSearchEntry);
  gtk_box_pack_start (GTK_BOX (hbox2), mSearchEntry, TRUE, TRUE, 40);

  alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment6);
  gtk_box_pack_start (GTK_BOX (vbox1), alignment6, FALSE, FALSE, 8);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment6), 0, 0, 12, 12);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_container_add (GTK_CONTAINER (alignment6), hbuttonbox1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  mSaveButton = gtk_button_new_from_stock ("gtk-save");
  gtk_widget_show (mSaveButton);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), mSaveButton);
  GTK_WIDGET_SET_FLAGS (mSaveButton, GTK_CAN_DEFAULT);
  gtk_widget_add_accelerator (mSaveButton, "clicked", accel_group,
                              GDK_S, (GdkModifierType) GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mAddButton = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (mAddButton);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), mAddButton);
  GTK_WIDGET_SET_FLAGS (mAddButton, GTK_CAN_DEFAULT);
  gtk_widget_add_accelerator (mAddButton, "clicked", accel_group,
                              GDK_A, (GdkModifierType) GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mRemoveButton = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (mRemoveButton);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), mRemoveButton);
  GTK_WIDGET_SET_FLAGS (mRemoveButton, GTK_CAN_DEFAULT);
  gtk_widget_add_accelerator (mRemoveButton, "clicked", accel_group,
                              GDK_R, (GdkModifierType) GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
//  gtk_widget_add_accelerator (mRemoveButton, "clicked", accel_group,
//                              GDK_Delete, (GdkModifierType) 0,
//                              GTK_ACCEL_VISIBLE);

/* // This button is obsolete
  mChangeModeButton = gtk_button_new_with_mnemonic (_("Add mode"));
  gtk_widget_show (mChangeModeButton);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), mChangeModeButton);
  GTK_WIDGET_SET_FLAGS (mChangeModeButton, GTK_CAN_DEFAULT);
  gtk_widget_add_accelerator (mChangeModeButton, "clicked", accel_group,
                              GDK_M, (GdkModifierType) GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
*/

  mQuitButton = gtk_button_new_from_stock ("gtk-quit");
  gtk_widget_show (mQuitButton);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), mQuitButton);
  GTK_WIDGET_SET_FLAGS (mQuitButton, GTK_CAN_DEFAULT);
  gtk_widget_add_accelerator (mQuitButton, "clicked", accel_group,
                              GDK_Q, (GdkModifierType) GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  gtk_window_add_accel_group (GTK_WINDOW (mWindow), accel_group);
}

void KasumiMainWindow::createWordList()
{
    // renderer and column
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    g_signal_connect(G_OBJECT(renderer), "edited",
		     G_CALLBACK(_call_back_edited_spelling_column), this);
    g_signal_connect(G_OBJECT(renderer), "editing-started",
		     G_CALLBACK(_call_back_editing_started_spelling_column), this);
    g_object_set(renderer, "editable", TRUE, NULL);
    mSpellingColumn = gtk_tree_view_column_new_with_attributes(_("Spelling"),
							      renderer,
							      "text",
							      COL_WORD,
							      NULL);
    gtk_tree_view_column_set_resizable(mSpellingColumn, true);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(mWordListView),mSpellingColumn,-1);

    renderer = gtk_cell_renderer_text_new();
    g_signal_connect(G_OBJECT(renderer), "edited",
		     G_CALLBACK(_call_back_edited_sound_column), this);
    g_signal_connect(G_OBJECT(renderer), "editing-started",
		     G_CALLBACK(_call_back_editing_started_sound_column), this);
    g_object_set(renderer, "editable", TRUE, NULL);
    mSoundColumn = gtk_tree_view_column_new_with_attributes(_("Sound"),
							   renderer,
							   "text",
							   COL_YOMI,
							   NULL);
    gtk_tree_view_column_set_resizable(mSoundColumn, true);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(mWordListView),mSoundColumn,-1);
    gtk_tree_view_column_set_clickable(mSoundColumn,TRUE);
    g_signal_connect(G_OBJECT(mSoundColumn), "clicked",
		     G_CALLBACK(_call_back_clicked_column_header), this);

    const int FREQ_LBOUND = conf->getPropertyValueByInt("MinFrequency");  
    const int FREQ_UBOUND = conf->getPropertyValueByInt("MaxFrequency");
    renderer = gui_cell_renderer_spin_new(FREQ_LBOUND,
					  FREQ_UBOUND,
					  1,
					  FREQ_UBOUND / 100,
					  FREQ_UBOUND / 100,
					  10,
					  0);
    g_signal_connect(G_OBJECT(renderer), "edited",
		     G_CALLBACK(_call_back_edited_freq_column), this);
    g_signal_connect(G_OBJECT(renderer), "editing-started",
		     G_CALLBACK(_call_back_editing_started_freq_column), this);
    g_object_set(renderer, "editable", TRUE, NULL);
    mFreqColumn = gtk_tree_view_column_new_with_attributes(_("Frequency"),
							   renderer,
							   "text",
							   COL_FREQ,
							   NULL);
    gtk_tree_view_column_set_resizable(mFreqColumn, true);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(mWordListView),mFreqColumn,-1);
    gtk_tree_view_column_set_clickable(mFreqColumn, TRUE);
    g_signal_connect(G_OBJECT(mFreqColumn), "clicked",
		     G_CALLBACK(_call_back_clicked_column_header), this);

    // word type column - combo renderer
    GtkListStore *gWordTypeList = gtk_list_store_new(WORDTYPE_NUM_COLS,G_TYPE_STRING,G_TYPE_POINTER);
    GtkTreeIter iter;
    WordTypeList::iterator p = KasumiWordType::beginWordTypeList();
    while(p != KasumiWordType::endWordTypeList())
    {
	gtk_list_store_append(gWordTypeList,&iter);
	gtk_list_store_set(gWordTypeList,&iter,
			   COL_UI_STRING, (*p)->getUIString().c_str(),
			   COL_WORDTYPE_POINTER, (*p),
			   -1);    
	p++;
    }
    
    renderer = gtk_cell_renderer_combo_new();
    g_signal_connect(G_OBJECT(renderer), "editing-started",
		     G_CALLBACK(_call_back_editing_started_wordtype_column), this);
    g_object_set(renderer,
		 "model", gWordTypeList,
		 "text-column", COL_UI_STRING,
		 "has-entry", FALSE,
		 "editable", TRUE,
		 NULL);
    mWordTypeColumn = gtk_tree_view_column_new_with_attributes(_("WordClass"),
							       renderer,
							       "text",
							       COL_PART,
							       NULL);
    gtk_tree_view_column_set_resizable(mWordTypeColumn, true);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(mWordListView),mWordTypeColumn,-1);
    gtk_tree_view_column_set_clickable(mWordTypeColumn, TRUE);
    g_signal_connect(G_OBJECT(mWordTypeColumn), "clicked",
		     G_CALLBACK(_call_back_clicked_column_header), this);

    // model
    WordList = gtk_list_store_new(NUM_COLS,G_TYPE_UINT,
				  G_TYPE_STRING,G_TYPE_STRING,
				  G_TYPE_UINT,G_TYPE_STRING);
    SortList = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(WordList));
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(SortList), COL_YOMI,
				    sortFuncBySound, NULL,
				    NULL);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(SortList), COL_FREQ,
				    sortFuncByFreq, NULL,
				    NULL);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(SortList), COL_PART,
				    sortFuncByWordClass, NULL,
				    NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(mWordListView),
			    GTK_TREE_MODEL(SortList));

    SortListSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(mWordListView));
    gtk_tree_selection_set_mode(SortListSelection, GTK_SELECTION_SINGLE);

    // destroy model automatically with view
    g_object_unref(GTK_TREE_MODEL(WordList));
}

void KasumiMainWindow::registerCallbackFunctions()
{
    // window
    g_signal_connect(G_OBJECT(mWindow), "delete_event",
		     G_CALLBACK(_call_back_delete_event), this);

    // buttons
    g_signal_connect(G_OBJECT(mQuitButton), "clicked",
		     G_CALLBACK(_call_back_quit), this);
    g_signal_connect(G_OBJECT(mSaveButton), "clicked",
		     G_CALLBACK(_call_back_store), this);
    g_signal_connect(G_OBJECT(mAddButton), "clicked",
		     G_CALLBACK(_call_back_add), this);
    g_signal_connect(G_OBJECT(mRemoveButton), "clicked",
		     G_CALLBACK(_call_back_remove), this);
/* // This button is obsolete
    g_signal_connect(G_OBJECT(mChangeModeButton), "clicked",
		     G_CALLBACK(_call_back_adding_mode), this);
*/

    // search entry
    g_signal_connect(G_OBJECT(mSearchEntry),
		     "changed",
		     G_CALLBACK(_call_back_changed_search_entry),
		     this);
    g_signal_connect(G_OBJECT(mSearchEntry),
		     "activate",
		     G_CALLBACK(_call_back_activate_search_entry),
		     this); // called when Entry key is pressed

}

void KasumiMainWindow::refresh(){
    GtkTreeModel *model = GTK_TREE_MODEL(SortList);
    GtkTreeIter iter;

    gtk_list_store_clear(WordList);

    list<KasumiWord*>::iterator p = dictionary->firstWordIter();
    while(p != dictionary->endWordIter()){
	try{
	    KasumiWord *word = (*p);

	    if(word != NULL && word->getFrequency() != 0){
		gtk_list_store_append(WordList,&iter);
      
		gtk_list_store_set(WordList,&iter,
				   COL_ID,word->getID(),
				   COL_WORD,word->getSpellingByUTF8().c_str(),
				   COL_YOMI,word->getSoundByUTF8().c_str(),
				   COL_FREQ,word->getFrequency(),
				   COL_PART,word->getWordTypeUIString().c_str(),
				   -1);
	    }
	}catch(KasumiException e){
	    handleException(e);
	}
	p++;
    }

    // If no words, nothing to do more
    if(!gtk_tree_model_get_iter_first(model, &iter))
	return;

    // select first word
    gtk_tree_selection_select_iter(SortListSelection,&iter);  
}

KasumiMainWindow::~KasumiMainWindow(){
  dictionary->removeEventListener(this);
  destroy();
}

void KasumiMainWindow::destroy(){
  gtk_widget_destroy(mWindow);
}

void KasumiMainWindow::quit(){
  if(modificationFlag){
    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(mWindow),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_QUESTION,
                                     GTK_BUTTONS_YES_NO,
                                     _("Your dictionary was changed. Do you save these changes "
                                       "before Kasumi quits?"));
    switch(gtk_dialog_run (GTK_DIALOG (dialog))){
    case GTK_RESPONSE_YES:
      dictionary->store();
      break;
    case GTK_RESPONSE_NO:
      break;
    default:
      cerr << "Error: Unexcepted return value of dialog's run" << endl;
      exit(1);
    }
    gtk_widget_destroy (dialog);
  }
  delete dictionary;
  delete this;
  gtk_main_quit();
}

void KasumiMainWindow::ClickedStoreButton(){
  try{
    dictionary->store();
    modificationFlag = false;
  }catch(KasumiException e){
      cerr <<  e.getMessage() << endl;
      exit(1);
  }
}

void KasumiMainWindow::ClickedAddButton(){
    KasumiWord *word = KasumiWord::createNewWord(conf);
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

void KasumiMainWindow::SwitchToAddingMode(){
  new KasumiAddWindow(dictionary,conf);
  delete this;
}

void KasumiMainWindow::startedEditingTextColumn(GtkCellEditable *editable,
						string path,
						TextColumn col)
{
    editingPath = gtk_tree_path_new_from_string(path.c_str());

    g_signal_connect(G_OBJECT(editable), "key-press-event",
		     G_CALLBACK(_call_back_key_pressed_text_column), this);
}

void KasumiMainWindow::pressedKeyOnTextColumn(GtkWidget *widget,
					      GdkEventKey *event)
{
    lastKeyState = event->state;
    lastKeyVal = event->keyval;
}

void KasumiMainWindow::editedTextColumn(GtkCellRendererText *renderer,
					const string &newText,
					TextColumn col)
{
    try{
	if(editingPath == NULL)
	    throw KasumiException(string("internal error: editingPath is already freed."), STDERR, KILL);

	GtkTreeIter iter;
	gtk_tree_model_get_iter(SortList, &iter, editingPath);

	size_t id;
	gtk_tree_model_get(SortList, &iter, COL_ID, &id, -1);

	KasumiWord *word = KasumiWord::getWordFromID(id);
   
	if(col == SPELLING)
	{
	    word->setSpellingByUTF8(newText);
	}
	else if(col == SOUND)
	{
	    word->setSoundByUTF8(newText);
	}
	else if(col == FREQ)
	{
	    word->setFrequency(str2int(newText));
	}
	
	GtkTreePath *editedPath = gtk_tree_path_copy(editingPath);
	gtk_tree_path_free(editingPath);
	editingPath = NULL;

	// set cursor right or left
	GtkTreeViewColumn *postCol = NULL;
	if(lastKeyVal == GDK_Tab)
	{
	    if(lastKeyState & GDK_CONTROL_MASK)
	    { // set cursor left
		switch(col){
		case SPELLING:
		    postCol = NULL;
		    break;
		case SOUND:
		    postCol = mSpellingColumn;
		    break;
		case FREQ:
		    postCol = mSoundColumn;
		    break;
		}
	    }
	    else
	    { // set cursor right
		switch(col){
		case SPELLING:
		    postCol = mSoundColumn;
		    break;
		case SOUND:
		    postCol = mFreqColumn;
		    break;
		case FREQ:
		    postCol = mWordTypeColumn;
		    break;
		}
	    }
	}

	gtk_tree_view_set_cursor(GTK_TREE_VIEW(mWordListView),
				 editedPath,
				 postCol,
				 false);

	gtk_tree_path_free(editedPath);
    }catch(KasumiException e){
	handleException(e);
    }
}

void KasumiMainWindow::startedEditingWordTypeColumn(GtkCellEditable *editable,
						    string path)
{
    editingPath = gtk_tree_path_new_from_string(path.c_str());
    g_signal_connect(G_OBJECT(editable), "changed",
		     G_CALLBACK(_call_back_changed_wordtype_column), this);
}

void KasumiMainWindow::changedWordTypeColumn(GtkComboBox *combo)
{
    try{
	// get which word type was selected
	GtkTreeIter iter;
	KasumiWordType *type;

	GtkTreeModel *model = gtk_combo_box_get_model(combo);
	gtk_combo_box_get_active_iter(combo, &iter);
	gtk_tree_model_get(model, &iter,
			   COL_WORDTYPE_POINTER, &type,
			   -1);

	// get edited word
	if(editingPath == NULL)
	    throw KasumiException(string("internal error: editingPath is already freed."), STDERR, KILL);
	gtk_tree_model_get_iter(SortList, &iter, editingPath);
	size_t id;
	gtk_tree_model_get(SortList, &iter, COL_ID, &id, -1);
	KasumiWord *word = KasumiWord::getWordFromID(id);

	word->setWordType(type);
    
	gtk_tree_path_free(editingPath);
	editingPath = NULL;
    }catch(KasumiException e){
	handleException(e);
    }
}

void KasumiMainWindow::FindNext(bool fromCurrent){
  GtkTreeModel *model = GTK_TREE_MODEL(SortList);
  GtkTreeIter iter;
  KasumiWord *word;
  bool fromFirst = false;
  GtkTreeIter StartIter;
  int id;
  string searchString = string(gtk_entry_get_text(GTK_ENTRY(mSearchEntry)));
  string comparedString;
  GtkWidget *dialog;

  SearchBy by = SPELLING;
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mSoundRadio))){
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
    word = KasumiWord::getWordFromID(id);

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
    dialog = gtk_message_dialog_new (GTK_WINDOW(mWindow),
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
        word = KasumiWord::getWordFromID(id);

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

  dialog = gtk_message_dialog_new(GTK_WINDOW(mWindow),
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
  
  gtk_tree_view_column_set_sort_indicator(mSoundColumn,FALSE);
  gtk_tree_view_column_set_sort_indicator(mFreqColumn,FALSE);
  gtk_tree_view_column_set_sort_indicator(mWordTypeColumn,FALSE);

  if(column == mSoundColumn){
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(SortList),
                                         COL_YOMI, order);
  }else if(column == mFreqColumn){
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(SortList),
                                         COL_FREQ, order);
  }else if(column == mWordTypeColumn){
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(SortList),
                                         COL_PART, order);
  }
  gtk_tree_view_column_set_sort_indicator(column,TRUE);
  gtk_tree_view_column_set_sort_order(column,order);
}

void KasumiMainWindow::removedWord(size_t id){
  refresh();
  modificationFlag = true;  
}

void KasumiMainWindow::appendedWord(KasumiWord *word){
  GtkTreeIter iter;
 
  gtk_list_store_append(WordList,&iter);

  gtk_list_store_set(WordList, &iter,
                     COL_ID, word->getID(),
                     COL_WORD, word->getSpellingByUTF8().c_str(),
                     COL_YOMI, word->getSoundByUTF8().c_str(),
                     COL_FREQ, word->getFrequency(),
                     COL_PART, word->getWordTypeUIString().c_str(),
                     -1);
  GtkTreeIter sort_iter;
  gtk_tree_model_sort_convert_child_iter_to_iter(GTK_TREE_MODEL_SORT(SortList),
                                                 &sort_iter,&iter);
  gtk_tree_selection_select_iter(SortListSelection,&sort_iter);

  modificationFlag = true;

  // set the vertical scroll bar of the tree view undermost
  GtkAdjustment *adjustment =
    gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(mScrolledWindow));
  gtk_adjustment_set_value(adjustment, adjustment->upper);
  
  // start editing for spelling forcibly
  GtkTreePath *path = gtk_tree_model_get_path(SortList, &sort_iter);
  gtk_tree_view_set_cursor(GTK_TREE_VIEW(mWordListView),
			   path,
			   mSpellingColumn,
			   true);
  gtk_tree_path_free(path);

}

void KasumiMainWindow::modifiedWord(KasumiWord *word){
    size_t id = word->getID();
    GtkTreeIter *iter = findCorrespondingIter(id);

    if(iter != NULL)
	gtk_list_store_set(WordList,iter,
			   COL_ID,id,
			   COL_WORD,word->getSpellingByUTF8().c_str(),
			   COL_YOMI,word->getSoundByUTF8().c_str(),
			   COL_FREQ,word->getFrequency(),
			   COL_PART,word->getWordTypeUIString().c_str(),
			   -1);

    free(iter);

    modificationFlag = true;  
}

// Do not returns iter of SortList but WordList
GtkTreeIter *KasumiMainWindow::findCorrespondingIter(size_t id){
  size_t i;
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

/* // obsolete
void _call_back_adding_mode(GtkWidget *widget,
                            gpointer data){
  KasumiMainWindow *window = (KasumiMainWindow *)data;
  window->SwitchToAddingMode();
}
*/

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

gboolean _call_back_key_pressed_text_column(GtkWidget *widget,
					    GdkEventKey *event,
					    gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->pressedKeyOnTextColumn(widget, event);
    return FALSE;
}

void _call_back_editing_started_sound_column(GtkCellRenderer *render,
					     GtkCellEditable *editable,
					     gchar *path,
					     gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->startedEditingTextColumn(editable, string(path), SOUND);
}

void _call_back_edited_sound_column(GtkCellRendererText *renderer,
				    gchar *arg1,
				    gchar *arg2,
				    gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->editedTextColumn(renderer, string(arg2), SOUND);
}

void _call_back_editing_started_spelling_column(GtkCellRenderer *render,
						GtkCellEditable *editable,
						gchar *path,
						gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->startedEditingTextColumn(editable, string(path), SPELLING);
}

void _call_back_edited_spelling_column(GtkCellRendererText *renderer,
				       gchar *arg1,
				       gchar *arg2,
				       gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->editedTextColumn(renderer, string(arg2), SPELLING);
}


void _call_back_editing_started_freq_column(GtkCellRenderer *render,
					    GtkCellEditable *editable,
					    gchar *path,
					    gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->startedEditingTextColumn(editable, string(path), FREQ);
}

void _call_back_edited_freq_column(GtkCellRendererText *renderer,
				   gchar *arg1,
				   gchar *arg2,
				   gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->editedTextColumn(renderer, string(arg2), FREQ);
}

void _call_back_editing_started_wordtype_column(GtkCellRenderer *render,
						GtkCellEditable *editable,
						gchar *path,
						gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->startedEditingWordTypeColumn(editable, string(path));
}

void _call_back_changed_wordtype_column(GtkComboBox *combo,
					gpointer data)
{
    KasumiMainWindow *window = (KasumiMainWindow *)data;
    window->changedWordTypeColumn(combo);
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
  
  cerr << "Invalid shortcut key option: " << key << endl;
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

  cerr << "Invalid mask option: " << key << endl;
  exit(1);
}

gint sortFuncByFreq(GtkTreeModel *model,
                    GtkTreeIter *a,
                    GtkTreeIter *b,
                    gpointer user_data){
  int id_a, id_b;
  gtk_tree_model_get(model, a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, b, COL_ID, &id_b, -1);
  KasumiWord *word_a = KasumiWord::getWordFromID(id_a);
  KasumiWord *word_b = KasumiWord::getWordFromID(id_b);

  return word_a->getFrequency() - word_b->getFrequency();
}

gint sortFuncBySound(GtkTreeModel *model,
                     GtkTreeIter *iter_a,
                     GtkTreeIter *iter_b,
                     gpointer user_data){
  int id_a, id_b;
  gtk_tree_model_get(model, iter_a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, iter_b, COL_ID, &id_b, -1);
  KasumiWord *word_a = KasumiWord::getWordFromID(id_a);
  KasumiWord *word_b = KasumiWord::getWordFromID(id_b);
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
        cerr << "invalid character or not hiragana" << endl;
        cerr << str_a << endl;
        exit(1);
      }

      if((second_b == 0x81 && third_b >= 0x81 && third_b <= 0xbf) ||
         (second_b == 0x82 && third_b >= 0x80 && third_b <= 0x94) ||
         (second_b == 0x83 && third_b == 0xbc)){
        b = second_b * 256 + third_b;
      }else{
        cerr << "invalid character or not hiragana" << endl;
        cerr << str_b << endl;
        exit(1);
      }

      if(a != b){
        return a - b;
      }
    }else{
        cerr << "invalid character or not hiragana" << endl;
        cerr << str_a << endl;
        cerr << str_b << endl;
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
  int id_a, id_b;
  gtk_tree_model_get(model, iter_a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, iter_b, COL_ID, &id_b, -1);
  KasumiWord *word_a = KasumiWord::getWordFromID(id_a);
  KasumiWord *word_b = KasumiWord::getWordFromID(id_b);

  return word_a->getWordType()->comp(word_b->getWordType());
}
