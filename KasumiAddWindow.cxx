/*********************************************************************
 * 
 * KasumiAddWindow.cxx
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
#include <iostream>
#include <algorithm>
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
    bool IsEUCJP = conf->getPropertyValueByBool("UseEUCJP");
  
    dictionary = aDictionary;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
    gtk_window_set_title(GTK_WINDOW(window), _("Kasumi (Register words)"));
    g_signal_connect(G_OBJECT(window), "delete_event",
		     G_CALLBACK(_call_back_add_window_delete_event), this);

    // tooltips for every widget
    Tooltips = gtk_tooltips_new();

    // creating vbox for text entries, spin button and so on.
    GtkWidget *vbox = gtk_vbox_new(FALSE,0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);
    gtk_container_add(GTK_CONTAINER(window),vbox);  

    // creating text entries for "Spelling"
    GtkWidget *label = gtk_label_new(_("Spelling"));
    GtkWidget *alignment = gtk_alignment_new(0, 0.5, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    SpellingEntry = gtk_entry_new();
    string spelling = conf->getPropertyValue("DefaultAddingSpelling");
    if (IsEUCJP) spelling = KasumiWord::convertEUCJPToUTF8(spelling);
    gtk_entry_set_text(GTK_ENTRY(SpellingEntry),
		       spelling.c_str());
    alignment = gtk_alignment_new(0, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 6, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment), SpellingEntry);
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    // creating text entries for "Sound"
    label = gtk_label_new(_("Sound"));
    alignment = gtk_alignment_new(0, 0.5, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    SoundEntry = gtk_entry_new();
    string sound = conf->getPropertyValue("DefaultAddingSound");
    if (IsEUCJP) sound = KasumiWord::convertEUCJPToUTF8(sound);
    gtk_entry_set_text(GTK_ENTRY(SoundEntry),
		       sound.c_str());
    alignment = gtk_alignment_new(0, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 6, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment), SoundEntry);
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    // creating spin button for "Frequency"
    label = gtk_label_new(_("Frequency"));
    alignment = gtk_alignment_new(0, 0.5, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    const int FREQ_DEFAULT = conf->getPropertyValueByInt("DefaultFrequency");
    const int FREQ_LBOUND = conf->getPropertyValueByInt("MinFrequency");  
    const int FREQ_UBOUND = conf->getPropertyValueByInt("MaxFrequency");
    GtkObject *adjustment = gtk_adjustment_new(FREQ_DEFAULT,
					       FREQ_LBOUND,
					       FREQ_UBOUND,
					       1,
					       FREQ_UBOUND / 100
					       ,0);
    FrequencySpin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment),1.0,0);
    alignment = gtk_alignment_new(0, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 6, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment), FrequencySpin);
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    // creating combo box for "Word Type"
    label = gtk_label_new(_("Word Type"));
    alignment = gtk_alignment_new(0, 0.5, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    string defaultCannaTab = conf->getPropertyValue("DefaultAddingWordType");

    GtkListStore *gWordTypeCategoryList = gtk_list_store_new(WORDTYPE_NUM_COLS,G_TYPE_STRING,G_TYPE_POINTER);
    GtkTreeIter iter;
    WordTypeList::iterator p = KasumiWordType::beginWordTypeList();
    list<string> wordCategoryList;
    while(p != KasumiWordType::endWordTypeList())
    {
	string category = (*p)->getCategory();
	if( find( wordCategoryList.begin(), wordCategoryList.end(), category )
	    == wordCategoryList.end() )
	{
	    wordCategoryList.push_back( category );

	    gtk_list_store_append(gWordTypeCategoryList,&iter);
	    gtk_list_store_set(gWordTypeCategoryList,&iter,
			       COL_UI_STRING, category.c_str(),
			       COL_WORDTYPE_POINTER, (*p),
			       -1);

	    if((*p)->getCannaTab() == defaultCannaTab)
		defaultWordTypeCategoryIter = iter;
	}

	p++;
    }
    WordTypeCategoryCombo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(gWordTypeCategoryList));
    GtkCellRenderer *renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(WordTypeCategoryCombo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(WordTypeCategoryCombo), renderer,
				   "text", COL_UI_STRING,
				   NULL);
    gtk_combo_box_set_active_iter(GTK_COMBO_BOX(WordTypeCategoryCombo), &defaultWordTypeCategoryIter);
    alignment = gtk_alignment_new(0, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 6, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment), WordTypeCategoryCombo);
    gtk_box_pack_start(GTK_BOX(vbox),
		       GTK_WIDGET(alignment),FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(WordTypeCategoryCombo),"changed",
		     G_CALLBACK(_call_back_word_type_category_changed),this);


    // creating combo box for "Subcategory of Word Type"
    label = gtk_label_new(_("Subcategory of Word Type"));
    alignment = gtk_alignment_new(0, 0.5, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment),GTK_WIDGET(label));
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(alignment),FALSE,FALSE,0);

    GtkListStore *gWordTypeList = gtk_list_store_new(WORDTYPE_NUM_COLS,G_TYPE_STRING,G_TYPE_POINTER); // dummy model
    WordTypeCombo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(gWordTypeList));
    renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(WordTypeCombo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(WordTypeCombo), renderer,
				   "text", COL_UI_STRING,
				   NULL);
    alignment = gtk_alignment_new(0, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 12, 6, 6);
    gtk_container_add(GTK_CONTAINER(alignment), WordTypeCombo);
    gtk_box_pack_start(GTK_BOX(vbox),
		       GTK_WIDGET(alignment),FALSE,FALSE,0);
    ChangeWordTypeList(true);

    // creating box for buttons
    GtkWidget *hbutton_box = gtk_hbutton_box_new();
    gtk_box_set_spacing(GTK_BOX(hbutton_box),6);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbutton_box),GTK_BUTTONBOX_SPREAD);
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbutton_box),FALSE,FALSE,6);

    if( conf->getPropertyValue("StartupMode") == "EXCLUSIVE" )
    {
	GtkWidget *button = gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
	g_signal_connect(G_OBJECT(button),"clicked",
			 G_CALLBACK(_call_back_add_window_add),this);
	gtk_tooltips_set_tip(Tooltips, button,
			     _("Add entered word and quit registration."),
			     _("If all the necessary items are filled in, add entered word and quit registration."));

	button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
	g_signal_connect(G_OBJECT(button),"clicked",
			 G_CALLBACK(_call_back_add_window_quit),this);
	gtk_tooltips_set_tip(Tooltips, button,
			     _("Cancel registration and quit."),
			     _("Cancel registration and quit."));

	gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    }
    else
    {
	// ADD mode
	// creating buttons and configure shortcut key
	GtkWidget *button = gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
	g_signal_connect(G_OBJECT(button),"clicked",
			 G_CALLBACK(_call_back_add_window_add),this);
	gtk_tooltips_set_tip(Tooltips, button,
			     _("Add entered word"),
			     _("If all the necessary items are filled in, add entered word."));

	button = gtk_button_new_from_stock(GTK_STOCK_EDIT);
	gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
	g_signal_connect(G_OBJECT(button),"clicked",
			 G_CALLBACK(_call_back_manage_mode),this);
	gtk_tooltips_set_tip(Tooltips, button,
			     _("Manage mode"),
			     _("Make the shift to manage mode to modify and remove registered words."));

	button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_box_pack_start(GTK_BOX(hbutton_box),GTK_WIDGET(button),TRUE,TRUE,0);
	g_signal_connect(G_OBJECT(button),"clicked",
			 G_CALLBACK(_call_back_add_window_quit),this);
	gtk_tooltips_set_tip(Tooltips, button,
			     _("Quit this application"),
			     _("Save dictionary and quit this application."));

	// get selection at the time of launching
	g_signal_connect(G_OBJECT(SpellingEntry),"selection_received",
			 G_CALLBACK(_call_back_selection_data_received),NULL);
    }
  
    gtk_widget_show_all(window);

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
//  anthy_dic_util_quit();
    delete dictionary;
    delete this;
    gtk_main_quit();
}

void KasumiAddWindow::ClickedAddButton(GtkWidget *widget){
    KasumiWord *word = KasumiWord::createNewWord(conf);
    try{
	// register entered word
	if(string(gtk_entry_get_text(GTK_ENTRY(SoundEntry))) == ""){
	    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							_("Invalid entry for Sound."));
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
							_("Invalid entry for Spelling.")
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

	GtkTreeIter iter;
	KasumiWordType *type;
	GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(WordTypeCombo));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(WordTypeCombo), &iter);
	gtk_tree_model_get(model, &iter,
			   COL_WORDTYPE_POINTER, &type,
			   -1);
	word->setWordType(type);

	dictionary->appendWord(word);

	string mode = conf->getPropertyValue("StartupMode");
	if( mode == "EXCLUSIVE" )
	{
	    cout << word->getSpelling() << ","
                 << word->getSound() << ","
                 << word->getFrequency() << ","
                 << word->getWordType()->getCannaTab() << endl;
	    quit();
	}
	else if( mode == "ADD" )
	{
	    gtk_entry_set_text(GTK_ENTRY(SoundEntry), "");
	    gtk_entry_set_text(GTK_ENTRY(SpellingEntry), "");
	    const int FREQ_DEFAULT = conf->getPropertyValueByInt("DefaultFrequency");
	    gtk_spin_button_set_value(GTK_SPIN_BUTTON(FrequencySpin),FREQ_DEFAULT);
	    gtk_combo_box_set_active_iter(GTK_COMBO_BOX(WordTypeCategoryCombo), &defaultWordTypeCategoryIter);
	    ChangeWordTypeList(true);
	}
    }catch(KasumiException e){
	handleException(e);
    }
}

void KasumiAddWindow::SwitchToManageMode(){
    new KasumiMainWindow(dictionary,conf);
    delete this;
}

void KasumiAddWindow::ChangeWordTypeList(bool toDefault = false){
    GtkTreeIter iter;
    GtkTreeIter defaultIter;
    bool defaultFlag = false;
    KasumiWordType *tmp;
    string category;
    string defaultCannaTab = conf->getPropertyValue("DefaultAddingWordType");
    
    gtk_combo_box_get_active_iter(GTK_COMBO_BOX(WordTypeCategoryCombo),
				  &iter);

    gtk_tree_model_get(gtk_combo_box_get_model( GTK_COMBO_BOX(WordTypeCategoryCombo)),
		       &iter,
		       COL_WORDTYPE_POINTER, &tmp,
		       -1 );
    category = tmp->getCategory();
    
    GtkListStore *gWordTypeList = gtk_list_store_new(WORDTYPE_NUM_COLS,G_TYPE_STRING,G_TYPE_POINTER);
    WordTypeList::iterator p = KasumiWordType::beginWordTypeList();
    while(p != KasumiWordType::endWordTypeList())
    {
	if( (*p)->getCategory() == category )
	{
	    gtk_list_store_append(gWordTypeList,&iter);
	    if( (*p)->getPos().size() != 0 )
		gtk_list_store_set(gWordTypeList,&iter,
				   COL_UI_STRING, (*p)->getPos().c_str(),
				   COL_WORDTYPE_POINTER, (*p),
				   -1);
	    else
		gtk_list_store_set(gWordTypeList,&iter,
				   COL_UI_STRING, (*p)->getCategory().c_str(),
				   COL_WORDTYPE_POINTER, (*p),
				   -1);

	    if( (*p)->getCannaTab() == defaultCannaTab )
	    {
		defaultFlag = true;
		defaultIter = iter;
	    }
	}

	p++;
    }

    gtk_combo_box_set_model(GTK_COMBO_BOX(WordTypeCombo),
			    GTK_TREE_MODEL(gWordTypeList));
    if( defaultFlag && toDefault )
	gtk_combo_box_set_active_iter(GTK_COMBO_BOX(WordTypeCombo),
				      &defaultIter);
    else
	gtk_combo_box_set_active(GTK_COMBO_BOX(WordTypeCombo), 0);
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

void _call_back_word_type_category_changed(GtkWidget *widget,
					   gpointer data)
{
    KasumiAddWindow *window = (KasumiAddWindow *)data;
    window->ChangeWordTypeList();
}
