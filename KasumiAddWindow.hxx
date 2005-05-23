#ifndef __KASUMI_ADD_WINDOW_HXX__
#define __KASUMI_ADD_WINDOW_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include "KasumiDic.hxx"
#include "KasumiWord.hxx"
#include "KasumiConfiguration.hxx"

class KasumiAddWindow{
  friend void _call_back_add_window_delete_event(GtkWidget *widget,
                                                     GdkEvent *event,
                                                     gpointer data);
  friend void _call_back_add_window_quit(GtkWidget *widget,
                                         gpointer data);
  friend void _call_back_add_window_add(GtkWidget *widget,
                                        gpointer data);
  friend void _call_back_manage_mode(GtkWidget *widget,
                                     gpointer data);
  friend void _call_back_add_window_changed_word_class_combo(GtkWidget *widget,
                                                             gpointer data);
  friend void _call_back_selection_data_received(GtkWidget *widget,
                                                 GtkSelectionData *selection_data,
                                                 gpointer data) ;
private:
  KasumiDic *dictionary;
  KasumiConfiguration *conf;
  GtkWidget *window;
  GtkWidget *SpellingEntry;
  GtkWidget *SoundEntry;
  GtkWidget *FrequencySpin;
  GtkWidget *WordClassCombo;

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
  
  void destroy();
  void quit();
  void ClickedAddButton(GtkWidget *widget);
  void ChangedWordClassCombo(GtkWidget *widget);
  WordClassType getActiveWordClass();
  VerbType getActiveVerbType();

  void SwitchToManageMode();
public:
  KasumiAddWindow(KasumiDic *aDictionary, KasumiConfiguration *conf);
  ~KasumiAddWindow();
};

void get_targets(GtkWidget *data);

#endif
