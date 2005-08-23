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

enum column_name{
  COL_ID = 0,
  COL_WORD,
  COL_YOMI,
  COL_FREQ,
  COL_PART,
  NUM_COLS
};

enum _SearchByEnum{
  SOUND = 0,
  SPELLING
};

typedef _SearchByEnum SearchBy;

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
  friend void _call_back_changed_list_cursor(GtkWidget *widget,
                                             gpointer data);
  friend void _call_back_changed_sound_entry(GtkWidget *widget,
                                             gpointer data);
  friend void _call_back_changed_spelling_entry(GtkWidget *widget,
                                                gpointer data);
  friend void _call_back_changed_frequency_spin(GtkWidget *widget,
                                                gpointer data);
  friend void _call_back_changed_word_class_combo(GtkWidget *widget,
                                                  gpointer data);
  friend void _call_back_changed_verb_type_combo(GtkWidget *widget,
                                                 gpointer data);
  friend void _call_back_toggled_check(GtkWidget *widget,
                                       gpointer data);
  friend void _call_back_changed_search_entry(GtkWidget *widget,
                                                gpointer data);
  friend void _call_back_activate_search_entry(GtkWidget *widget,
                                               gpointer data);
  friend void _call_back_clicked_column_header(GtkTreeViewColumn *column,
                                               gpointer data);

private:
  KasumiDic *dictionary;
  KasumiConfiguration *conf;
  GtkWidget *window;
  GtkWidget *SpellingEntry;
  GtkWidget *SoundEntry;
  GtkWidget *FrequencySpin;
  GtkWidget *WordClassCombo;
  GtkWidget *WordListView;
  GtkWidget *ScrolledWindow;

  GtkWidget *NounOptionPane;
  GtkWidget *NounOptionSaConnectionCheck;
  GtkWidget *NounOptionNaConnectionCheck;
  GtkWidget *NounOptionSuruConnectionCheck;
  GtkWidget *NounOptionGokanCheck;
  GtkWidget *NounOptionKakujoshiConnectionCheck;

  GtkWidget *AdvOptionPane;
  GtkWidget *AdvOptionToConnectionCheck;
  GtkWidget *AdvOptionTaruConnectionCheck;
  GtkWidget *AdvOptionSuruConnectionCheck;
  GtkWidget *AdvOptionGokanCheck;

  GtkWidget *VerbOptionPane;
  GtkWidget *VerbTypeCombo;
  GtkWidget *VerbOptionRentaiCheck;
  
  GtkListStore *WordList;
  GtkTreeModel *SortList;
  GtkTreeSelection *SortListSelection;
  GtkTreeViewColumn *FreqColumn;
  gulong HandlerIDOfSoundEntry;
  gulong HandlerIDOfSpellingEntry;
  gulong HandlerIDOfFrequencySpin;
  gulong HandlerIDOfWordClassCombo;
  gulong HandlerIDOfNounOptionSaConnectionCheck;
  gulong HandlerIDOfNounOptionNaConnectionCheck;
  gulong HandlerIDOfNounOptionSuruConnectionCheck;
  gulong HandlerIDOfNounOptionGokanCheck;
  gulong HandlerIDOfNounOptionKakujoshiConnectionCheck;
  gulong HandlerIDOfAdvOptionToConnectionCheck;
  gulong HandlerIDOfAdvOptionTaruConnectionCheck;
  gulong HandlerIDOfAdvOptionSuruConnectionCheck;
  gulong HandlerIDOfAdvOptionGokanCheck;
  gulong HandlerIDOfVerbTypeCombo;
  gulong HandlerIDOfVerbOptionRentaiCheck;

  GtkWidget *SearchBySpellingRadio;
  GtkWidget *SearchBySoundRadio;
  GtkWidget *SearchEntry;

  GtkWidget *SortCombo;
  
  bool modificationFlag;
  string previousSoundEntryText;
  
  void destroy();
  void quit();
  void ClickedStoreButton();
  void ClickedAddButton();
  void ClickedRemoveButton();
  void ChangedListCursor();
  void ChangedSoundEntry();
  void ChangedSpellingEntry();
  void ChangedFrequencySpin();
  void ChangedWordClassCombo();
  void ChangedVerbTypeCombo();
  void ChangedOption(GtkWidget *widget);
  void SwitchToAddingMode();

  void FindNext(bool fromCurrent);
  GtkTreeIter *findCorrespondingIter(int id);

  void SortByFreq();

  void synchronizeOptionCheckButton(KasumiWord *word);
  void setActiveVerbType(VerbType type);
  VerbType getActiveVerbType();
  void setActiveWordClass(WordClassType type);
  WordClassType getActiveWordClass();
  void flipOptionPane();
public:
  KasumiMainWindow(KasumiDic *aDictionary, KasumiConfiguration *conf);
  virtual ~KasumiMainWindow();
  void refresh();
  void removedWord(int id);
  void appendedWord(int id);
  void modifiedWord(int id);
};

guint getAccelKey(const string &key);
GdkModifierType getModifierType(const string &key);

gint sortFuncByFreq(GtkTreeModel *model,
                    GtkTreeIter *a,
                    GtkTreeIter *b,
                    gpointer user_data);

#endif
