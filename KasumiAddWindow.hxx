#ifndef __KASUMI_ADD_WINDOW_HXX__
#define __KASUMI_ADD_WINDOW_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include "KasumiDic.hxx"
#include "KasumiWord.hxx"

class KasumiAddWindow{
  friend void _call_back_add_window_destroy(GtkWidget *widget, gpointer data);
  friend gboolean _call_back_add_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
  friend void _call_back_add_window_quit(GtkWidget *widget, gpointer data);
  friend void _call_back_add_window_add(GtkWidget *widget, gpointer data);
  friend void _call_back_add_window_changed_word_class_combo(GtkWidget *widget, gpointer data);
private:
  KasumiDic *dictionary;
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
  
  void destroy(GtkWidget *widget);
  gboolean delete_event(GtkWidget *widget, GdkEvent *event);
  void ClickedQuitButton(GtkWidget *widget);
  void ClickedAddButton(GtkWidget *widget);
  void ChangedWordClassCombo(GtkWidget *widget);
  WordClassType getActiveWordClass();
public:
  KasumiAddWindow(KasumiDic *aDictionary);
};

#endif
