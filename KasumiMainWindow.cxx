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
#include <cstdlib>
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
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *label1;
  GtkWidget *hbox1;
  GSList *mSpellingRadio_group = NULL;
  GtkWidget *alignment;
  GtkWidget *hbuttonbox1;

  mWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(mWindow, -1, 400);
  gtk_window_set_title (GTK_WINDOW (mWindow), _("Kasumi"));

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 6);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (mWindow), vbox1);

  mScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_set_border_width(GTK_CONTAINER(mScrolledWindow), 6);
  gtk_widget_show (mScrolledWindow);
  gtk_box_pack_start (GTK_BOX (vbox1), mScrolledWindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (mScrolledWindow), GTK_SHADOW_IN);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(mScrolledWindow),
				 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  mWordListView = gtk_tree_view_new ();
  gtk_widget_show (mWordListView);
  gtk_container_add (GTK_CONTAINER (mScrolledWindow), mWordListView);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox2), 6);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, FALSE, FALSE, 0);

  label1 = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label1), _("<b>Search:</b>"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox2), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label1), 4, 4);



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

  alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment);
  gtk_box_pack_start (GTK_BOX (vbox2), alignment, FALSE, FALSE, 6);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 18, 0);

  mSearchEntry = gtk_entry_new ();
  gtk_widget_show (mSearchEntry);
  gtk_container_add (GTK_CONTAINER (alignment), mSearchEntry);

  alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment);
  gtk_box_pack_start (GTK_BOX (vbox1), alignment, FALSE, FALSE, 6);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 6, 6);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_container_add (GTK_CONTAINER (alignment), hbuttonbox1);
  gtk_box_set_spacing(GTK_BOX(hbuttonbox1), 6);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);

  mSaveButton = gtk_button_new_from_stock ("gtk-save");
  gtk_widget_show (mSaveButton);
  gtk_box_pack_start (GTK_BOX (hbuttonbox1), mSaveButton, FALSE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (mSaveButton, GTK_CAN_DEFAULT);

  mAddButton = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (mAddButton);
  gtk_box_pack_start (GTK_BOX (hbuttonbox1), mAddButton, FALSE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (mAddButton, GTK_CAN_DEFAULT);

  mRemoveButton = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (mRemoveButton);
  gtk_box_pack_start (GTK_BOX (hbuttonbox1), mRemoveButton, FALSE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (mRemoveButton, GTK_CAN_DEFAULT);

  mQuitButton = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (mQuitButton);
  gtk_box_pack_end (GTK_BOX (hbuttonbox1), mQuitButton, FALSE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (mQuitButton, GTK_CAN_DEFAULT);
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
    gtk_tree_view_column_set_min_width(mSpellingColumn, 80);
    gtk_tree_view_column_set_resizable(mSpellingColumn, true);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(mWordListView),
				mSpellingColumn, -1);

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
    gtk_tree_view_column_set_min_width(mSoundColumn, 80);
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
    gtk_tree_view_column_set_min_width(mFreqColumn, 60);
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
  unsigned int id;
  
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

	unsigned int id;
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
	unsigned int id;
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
  GtkTreeIter StartIter;
  unsigned int id;
  string searchString = string(gtk_entry_get_text(GTK_ENTRY(mSearchEntry)));
  string comparedString;

  // undo the modified text color
  gtk_widget_modify_text(mSearchEntry, GTK_STATE_NORMAL, NULL);

  SearchBy by = SPELLING;
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mSoundRadio))){
    by = SOUND;
  }

  if(!gtk_tree_selection_get_selected(SortListSelection, &model, &iter)){
    if(!gtk_tree_model_get_iter_first(model, &iter)){
      // If no words, do nothing.
      return;
    }
  }
  StartIter = iter;

  if(fromCurrent){
    // Search from the selected word
    // nothing to do
    // to prevent from calling gtk_tree_model_iter_next )
  }else if(!gtk_tree_model_iter_next(model,&iter)){
    // Search from next word if a certain word is selected.
    // If the selected is the last word, seek from the first word.
    
    // If no words, do nothing;
    if(!gtk_tree_model_get_iter_first(model, &iter))
      return;
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

  // from first
  gtk_tree_model_get_iter_first(model, &iter);

  do{
      gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
      word = KasumiWord::getWordFromID(id);

      if(by == SPELLING){
	  comparedString = word->getSpellingByUTF8();
      }else{
	  comparedString = word->getSoundByUTF8();
      }

      if(comparedString.find(searchString,0) == 0){
	  gtk_tree_selection_select_iter(SortListSelection, &iter);
	  return; // succeed searching
      }

  }while(gtk_tree_model_iter_next(model, &iter) &&
	 (StartIter.user_data != iter.user_data ||
	  StartIter.user_data2 != iter.user_data2 ||
	  StartIter.user_data3 != iter.user_data3));

  // found nothing
  GdkColor red;
  red.red = 65535;
  red.green = red.blue = 0;
  gtk_widget_modify_text(mSearchEntry, GTK_STATE_NORMAL, &red);
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

void KasumiMainWindow::removedWord(unsigned int id){
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
  // removed: These codes doesn't work well. 
/*
  GtkAdjustment *adjustment =
    gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(mScrolledWindow));
  gtk_adjustment_set_value(adjustment, adjustment->upper);
*/
  
  // start editing for spelling forcibly
  GtkTreePath *path = gtk_tree_model_get_path(SortList, &sort_iter);
  gtk_tree_view_set_cursor(GTK_TREE_VIEW(mWordListView),
			   path,
			   mSpellingColumn,
			   true);
  gtk_tree_path_free(path);

}

void KasumiMainWindow::modifiedWord(KasumiWord *word){
    unsigned int id = word->getID();
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
GtkTreeIter *KasumiMainWindow::findCorrespondingIter(unsigned int id){
  unsigned int i;
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

  if(i == (unsigned int) key.npos){
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
  unsigned int id_a, id_b;
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
  unsigned int id_a, id_b;
  gtk_tree_model_get(model, iter_a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, iter_b, COL_ID, &id_b, -1);
  KasumiWord *word_a = KasumiWord::getWordFromID(id_a);
  KasumiWord *word_b = KasumiWord::getWordFromID(id_b);
  const char *str_a = word_a->getSoundByUTF8().c_str();
  const char *str_b = word_b->getSoundByUTF8().c_str();
  int size_a = word_a->getSound().size();
  int size_b = word_b->getSound().size();
  int size = (size_a < size_b) ? size_a : size_b;
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
    a = first_a * 65535 + second_a * 256 + third_a;
    b = first_b * 65535 + second_b * 256 + third_b;

    if(a != b){
      return a - b;
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
  unsigned int id_a, id_b;
  gtk_tree_model_get(model, iter_a, COL_ID, &id_a, -1);
  gtk_tree_model_get(model, iter_b, COL_ID, &id_b, -1);
  KasumiWord *word_a = KasumiWord::getWordFromID(id_a);
  KasumiWord *word_b = KasumiWord::getWordFromID(id_b);

  return word_a->getWordType()->comp(word_b->getWordType());
}
