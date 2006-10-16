#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <libgtkhtml/gtkhtml.h>
#include <ydpdict/ydpdict.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "dictionary.h"


GtkWidget*
create_gtkhtml1 (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
	GtkWidget *view;
	HtmlDocument *doc;
	char *text = "<?xml version=\"1.0\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\"><head><style>\nbody { background: white; margin-top: 0px; }\n</style></head><body><h1>ydpdict</h1><h3>wersja GTK+</h3><hr /><p>Witamy w wersji testowej.</p></body></html>";

	doc = html_document_new ();

	html_document_open_stream (doc, "text/html");
	html_document_write_stream (doc, text, strlen (text));
	html_document_close_stream (doc);

	view = html_view_new ();
	html_view_set_document (HTML_VIEW (view), doc);

	return view;
}


void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data)
{
	gtk_main_quit ();
}


void
entry_update                           (GtkWidget       *html,
                                        gint             index,
					const gchar     *word)
{
	GtkWidget *window1;
	HtmlDocument *doc;
	uint32_t i = (uint32_t) -1;
	gchar *def;
	int staticdef;

	if (!html)
		return;

	printf ("entry_update(%p, %d, %s%s%s)\n", html, index, (word) ? "\"" : "", (word) ? word : "NULL", (word) ? "\"" : "");
	
	G_LOCK (dict);

	if (!dict_loaded) {
		G_UNLOCK (dict);
		return;
	}

	window1 = lookup_widget (GTK_WIDGET (html), "window1");

	if (word) {
		i = ydpdict_find(&dict, word);
		
		if (i == (uint32_t) -1) {
			for (i = 0; i < dict.word_count; i++) {
				if (!strncasecmp (dict.words[i], word, strlen (word)))
					break;
			}
			
			if (i == dict.word_count)
				i = (uint32_t) -1;
		}

		if (i != (uint32_t) -1) {
			GtkTreePath *path;
			
			if ((path = gtk_tree_path_new_from_indices (i, -1))) {
				GtkTreeView *treeview;

				if ((treeview = GTK_TREE_VIEW (lookup_widget (GTK_WIDGET (html), "treeview1")))) {
					g_object_set_data (G_OBJECT (treeview), "dont-update", GINT_TO_POINTER (1));
					gtk_tree_view_set_cursor (treeview, path, NULL, 0);
				}

				gtk_tree_path_free (path);
			}
		}
	}

	if (index != -1)
		i = index;

	ydpdict_xhtml_set_header (&dict, 1);
	ydpdict_xhtml_set_style (&dict, "\nbody { background: white; margin-top: 0px; }\n");

	if (i == (uint32_t) -1) {
		def = "<?xml version=\"1.0\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\"><head><style>\nbody { background: white; margin-top: 0px; }\n</style><body>Nie znaleziono.</body></html>";
		staticdef = 1;
	} else {
		def = ydpdict_read_xhtml (&dict, i);
		staticdef = 0;
	}

	G_UNLOCK(dict);

	doc = html_document_new ();

	html_document_open_stream (doc, "text/html");
	html_document_write_stream (doc, def, strlen (def));
	html_document_close_stream (doc);

	html_view_set_document (HTML_VIEW (html), doc);

	if (!staticdef)
		free (def);
}


void
on_entry1_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
	g_print("on_entry1_changed\n");

	entry_update (lookup_widget (GTK_WIDGET (editable), "gtkhtml1"), -1, gtk_entry_get_text (GTK_ENTRY (editable)));
}



void
on_treeview1_cursor_changed            (GtkTreeView     *treeview,
                                        gpointer         user_data)
{
	GtkTreePath *path;
	gchar *index;

	g_print ("on_treeview1_cursor_changed\n");

	if (g_object_get_data (G_OBJECT (treeview), "dont-update")) {
		g_object_set_data (G_OBJECT (treeview), "dont-update", NULL);
		return;
	}

	gtk_tree_view_get_cursor (treeview, &path, NULL);

	if (!path)
		return;

	index = gtk_tree_path_to_string (path);
	entry_update (lookup_widget (GTK_WIDGET (treeview), "gtkhtml1"), atoi (index), NULL);
	g_free (index);

	gtk_tree_path_free (path);
}


gboolean
on_treeview1_key_press_event           (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
	g_print( "on_treeview1_key_press_event\n");

	if (event && !(event->keyval == GDK_Up || event->keyval == GDK_Down || event->keyval == GDK_Page_Up || event->keyval == GDK_Page_Down || event->keyval == GDK_Home || event->keyval == GDK_End)) {
		gtk_widget_event (lookup_widget (widget, "entry1"), (GdkEvent*) event);
		return TRUE;
	}

	return FALSE;
}


void
on_toolbutton_copy_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_copy_clicked()\n");

}


gboolean
on_entry1_key_press_event              (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
	g_print ("on_entry1_key_press_event\n");

	return FALSE;
}


void
on_toolbutton_en_pl_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_de_pl_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (GTK_WIDGET(toggletoolbutton), YDPDICT_EN_PL);
}


void
on_toolbutton_pl_en_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_de_pl_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (GTK_WIDGET (toggletoolbutton), YDPDICT_PL_EN);
}


void
on_toolbutton_de_pl_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_de_pl_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (GTK_WIDGET (toggletoolbutton), YDPDICT_DE_PL);
}


void
on_toolbutton_pl_de_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_pl_de_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (GTK_WIDGET (toggletoolbutton), YDPDICT_PL_DE);
}

