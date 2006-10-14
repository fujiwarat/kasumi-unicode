/*********************************************************************
 * 
 * KasumiAddWindow.hxx
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

#ifndef __KASUMI_ADD_WINDOW_HXX__
#define __KASUMI_ADD_WINDOW_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include "KasumiDic.hxx"
#include "KasumiWord.hxx"
#include "KasumiConfiguration.hxx"

void _call_back_add_window_delete_event(GtkWidget *widget,
                                        GdkEvent *event,
                                        gpointer data);
void _call_back_add_window_quit(GtkWidget *widget,
                                gpointer data);
void _call_back_add_window_add(GtkWidget *widget,
                               gpointer data);
void _call_back_manage_mode(GtkWidget *widget,
                            gpointer data);
void _call_back_selection_data_received(GtkWidget *widget,
                                        GtkSelectionData *selection_data,
                                        gpointer data);
void _call_back_word_type_category_changed(GtkWidget *widget,
					   gpointer data);

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
    friend void _call_back_add_window_changed_word_type_combo(GtkWidget *widget,
							      gpointer data);
    friend void _call_back_selection_data_received(GtkWidget *widget,
						   GtkSelectionData *selection_data,
						   gpointer data) ;
    friend void _call_back_word_type_category_changed(GtkWidget *widget,
						      gpointer data);
private:
    KasumiDic *dictionary;
    KasumiConfiguration *conf;
    GtkWidget *window;
    GtkWidget *SpellingEntry;
    GtkWidget *SoundEntry;
    GtkWidget *FrequencySpin;
    GtkWidget *WordTypeCategoryCombo;
    GtkWidget *WordTypeCombo;
    GtkTooltips *Tooltips;

    GtkTreeIter defaultWordTypeCategoryIter;
  
    void destroy();
    void quit();
    void ClickedAddButton(GtkWidget *widget);

    void SwitchToManageMode();
    void ChangeWordTypeList( bool toDefault );
public:
    KasumiAddWindow(KasumiDic *aDictionary, KasumiConfiguration *conf);
    ~KasumiAddWindow();
};

void get_targets(GtkWidget *data);

#endif
