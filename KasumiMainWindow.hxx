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
    SOUND = 0,
    SPELLING
};

typedef _TextColumnEnum SearchBy;
typedef _TextColumnEnum TextColumn;

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
    friend void _call_back_adding_mode(GtkWidget *widget,
				       gpointer data);
    friend void _call_back_changed_search_entry(GtkWidget *widget,
                                                gpointer data);
    friend void _call_back_activate_search_entry(GtkWidget *widget,
						 gpointer data);
    friend void _call_back_clicked_column_header(GtkTreeViewColumn *column,
						 gpointer data);
    friend void _call_back_edited_sound_column(GtkCellRendererText *renderer,
					       gchar *arg1,
					       gchar *arg2,
					       gpointer data);
    friend void _call_back_edited_spelling_column(GtkCellRendererText *renderer,
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
    GtkWidget *mChangeModeButton;

    GtkListStore *WordList;
    GtkTreeModel *SortList;
    GtkTreeSelection *SortListSelection;
    GtkTreeViewColumn *mSpellingColumn;
    GtkTreeViewColumn *mSoundColumn;
    GtkTreeViewColumn *mFreqColumn;
    GtkTreeViewColumn *mWordTypeColumn;

    GtkTreePath *editingPath;
  
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

    void editedTextColumn(GtkCellRendererText *renderer,
			  const string &pathStr,
			  const string &newSound,
			  TextColumn col);
    void changedWordTypeColumn(GtkComboBox *combo);

    void FindNext(bool fromCurrent);
    GtkTreeIter *findCorrespondingIter(size_t id);
    
    void SortBy(GtkTreeViewColumn *column);

public:
  KasumiMainWindow(KasumiDic *aDictionary, KasumiConfiguration *conf);
  virtual ~KasumiMainWindow();
  void refresh();
  void removedWord(size_t id);
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
