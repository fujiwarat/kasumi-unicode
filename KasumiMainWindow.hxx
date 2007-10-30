/*********************************************************************
 * 
 * KasumiMainWindow.hxx
 * 
 * Kasumi - a management tool for a private dictionary of anthy
 * 
 * Copyright (C) 2004-2006 Takashi Nakamoto
 * Copyright (C) 2006 Ikuya Awashiro
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

#ifndef __KASUMI_MAIN_WINDOW_HXX__
#define __KASUMI_MAIN_WINDOW_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <string>
#include "KasumiDic.hxx"
#include "KasumiDicEventListener.hxx"
#include "KasumiConfiguration.hxx"

enum column_name
{
  COL_ID = 0,
  COL_WORD,
  COL_YOMI,
  COL_FREQ,
  COL_PART,
  NUM_COLS
};

enum wordtype_column_name
{
    COL_UI_STRING = 0,
    COL_WORDTYPE_POINTER,
    WORDTYPE_NUM_COLS
};

enum _TextColumnEnum
{
    SPELLING = 0,
    SOUND,
    FREQ
};

typedef _TextColumnEnum SearchBy;
typedef _TextColumnEnum TextColumn;

void _call_back_changed_wordtype_column(GtkComboBox *combo,
                                        gpointer data);
gboolean _call_back_key_pressed_text_column(GtkWidget *widget,
                                            GdkEventKey *event,
                                            gpointer data);
void _call_back_activate_search_entry(GtkWidget *widget,
                                       gpointer data);
void _call_back_changed_search_entry(GtkWidget *widget,
                                       gpointer data);
void _call_back_remove(GtkWidget *widget,
                       gpointer data);
void _call_back_delete_event(GtkWidget *widget,
                             GdkEvent *event,
                             gpointer data);
void _call_back_quit(GtkWidget *widget,
                     gpointer data);
void _call_back_store(GtkWidget *widget,
                      gpointer data);
void _call_back_add(GtkWidget *widget,
                    gpointer data);
void _call_back_clicked_column_header(GtkTreeViewColumn *column,
                                             gpointer data);
gboolean _call_back_key_pressed_text_column(GtkWidget *widget,
					    GdkEventKey *event,
					    gpointer data);
void _call_back_editing_started_sound_column(GtkCellRenderer *render,
					     GtkCellEditable *editable,
					     gchar *path,
					     gpointer data);
void _call_back_edited_sound_column(GtkCellRendererText *renderer,
				    gchar *arg1,
				    gchar *arg2,
				    gpointer data);
void _call_back_editing_started_spelling_column(GtkCellRenderer *render,
						GtkCellEditable *editable,
						gchar *path,
						gpointer data);
void _call_back_edited_spelling_column(GtkCellRendererText *renderer,
				       gchar *arg1,
				       gchar *arg2,
				       gpointer data);
void _call_back_editing_started_freq_column(GtkCellRenderer *render,
					    GtkCellEditable *editable,
					    gchar *path,
					    gpointer data);
void _call_back_edited_freq_column(GtkCellRendererText *renderer,
				   gchar *arg1,
				   gchar *arg2,
				   gpointer data);
void _call_back_editing_started_wordtype_column(GtkCellRenderer *render,
						GtkCellEditable *editable,
						gchar *path,
						gpointer data);

class KasumiMainWindow : public KasumiDicEventListener{
    friend void _call_back_delete_event(GtkWidget *widget,
					GdkEvent *event,
					gpointer data);
    friend void _call_back_quit(GtkWidget *widget,
				gpointer data);
    friend void _call_back_store(GtkWidget *widget,
				 gpointer data);
    friend void _call_back_add(GtkWidget *widget,
			       gpointer data);
    friend void _call_back_remove(GtkWidget *widget,
				  gpointer data);
/* // obsolete
    friend void _call_back_adding_mode(GtkWidget *widget,
				       gpointer data);
*/
    friend void _call_back_changed_search_entry(GtkWidget *widget,
                                                gpointer data);
    friend void _call_back_activate_search_entry(GtkWidget *widget,
						 gpointer data);
    friend void _call_back_clicked_column_header(GtkTreeViewColumn *column,
						 gpointer data);
    friend gboolean _call_back_key_pressed_text_column(GtkWidget *widget,
						       GdkEventKey *event,
						       gpointer data);
    friend void _call_back_editing_started_sound_column(GtkCellRenderer *render,
							GtkCellEditable *editable,
							gchar *path,
							gpointer data);
    friend void _call_back_edited_sound_column(GtkCellRendererText *renderer,
					       gchar *arg1,
					       gchar *arg2,
					       gpointer data);
    friend void _call_back_editing_started_spelling_column(GtkCellRenderer *render,
							   GtkCellEditable *editable,
							   gchar *path,
							   gpointer data);
    friend void _call_back_edited_spelling_column(GtkCellRendererText *renderer,
						  gchar *arg1,
						  gchar *arg2,
						  gpointer data);
    friend void _call_back_editing_started_freq_column(GtkCellRenderer *render,
						       GtkCellEditable *editable,
						       gchar *path,
						       gpointer data);
    friend void _call_back_edited_freq_column(GtkCellRendererText *renderer,
					      gchar *arg1,
					      gchar *arg2,
					      gpointer data);
    friend void _call_back_editing_started_wordtype_column(GtkCellRenderer *render,
							   GtkCellEditable *editable,
							   gchar *path,
							   gpointer data);
    friend void _call_back_changed_wordtype_column(GtkComboBox *combo,
						   gpointer data);
private:
    KasumiDic *dictionary;
    KasumiConfiguration *conf;

    GtkWidget *mWindow;
    GtkWidget *mSpellingRadio;
    GtkWidget *mSoundRadio;
    GtkWidget *mSearchEntry;
    GtkWidget *mWordListView;
    GtkWidget *mScrolledWindow;
    GtkWidget *mQuitButton;
    GtkWidget *mSaveButton;
    GtkWidget *mAddButton;
    GtkWidget *mRemoveButton;
//    GtkWidget *mChangeModeButton;  // obsolete

    GtkListStore *WordList;
    GtkTreeModel *SortList;
    GtkTreeSelection *SortListSelection;
    GtkTreeViewColumn *mSpellingColumn;
    GtkTreeViewColumn *mSoundColumn;
    GtkTreeViewColumn *mFreqColumn;
    GtkTreeViewColumn *mWordTypeColumn;

    GtkTreePath *editingPath;
    guint lastKeyState;
    guint lastKeyVal;
  
    bool modificationFlag;
    string previousSoundEntryText;

    void createWindow();
    void createWordList();
    void registerCallbackFunctions();
      
    void destroy();
    void quit();
    void ClickedStoreButton();
    void ClickedAddButton();
    void ClickedRemoveButton();
    void SwitchToAddingMode();

    void startedEditingTextColumn(GtkCellEditable *editable,
				  string path,
				  TextColumn col);
    void pressedKeyOnTextColumn(GtkWidget *widget,
				GdkEventKey *event);
    void editedTextColumn(GtkCellRendererText *renderer,
			  const string &newSound,
			  TextColumn col);
    void startedEditingWordTypeColumn(GtkCellEditable *editable,
				      string path);
    void changedWordTypeColumn(GtkComboBox *combo);

    void FindNext(bool fromCurrent);
    GtkTreeIter *findCorrespondingIter(unsigned int id);
    
    void SortBy(GtkTreeViewColumn *column);

public:
  KasumiMainWindow(KasumiDic *aDictionary, KasumiConfiguration *conf);
  virtual ~KasumiMainWindow();
  void refresh();
  void removedWord(unsigned int id);
  void appendedWord(KasumiWord *word);
  void modifiedWord(KasumiWord *word);
};

guint getAccelKey(const string &key);
GdkModifierType getModifierType(const string &key);

gint sortFuncByFreq(GtkTreeModel *model,
                    GtkTreeIter *a,
                    GtkTreeIter *b,
                    gpointer user_data);
gint sortFuncBySound(GtkTreeModel *model,
                     GtkTreeIter *a,
                     GtkTreeIter *b,
                     gpointer user_data);
gint sortFuncByWordClass(GtkTreeModel *model,
                         GtkTreeIter *a,
                         GtkTreeIter *b,
                         gpointer user_data);

#endif
