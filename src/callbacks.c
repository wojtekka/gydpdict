#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtkhtml/gtkhtml.h>
#include <ydpdict/ydpdict.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

#include "callbacks.h"
#include "dictionary.h"
#include "main.h"


const gchar xhtml_template[] = "<?xml version=\"1.0\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n<style>\nbody { background: white; margin-top: 0px; }\n</style>\n<body>\n%s\n</body>\n</html>\n";


GtkWidget*
create_gtkhtml1 (void)
{
	GtkWidget *gtk_html;
	gchar *text;
	
	text = g_strdup_printf(xhtml_template, _("<h1>ydpdict</h1><h3>wersja GTK+</h3><hr /><p>Witamy w wersji testowej.</p>"));

	gtk_html = gtk_html_new_from_string (text, -1);

	g_free(text);

	return gtk_html;
}


void
on_main_window_destroy                 (GtkObject       *object,
                                        gpointer         user_data)
{
	gtk_main_quit ();
}


void
entry_update                           (gint             index,
					const gchar     *word)
{
	gchar *def;
	int i = -1;

	if (html_view == NULL)
		return;

	printf ("entry_update(%d, %s%s%s)\n", index, (word != NULL) ? "\"" : "", (word != NULL) ? word : "NULL", (word != NULL) ? "\"" : "");
	
	G_LOCK (dict);

	if (dict == NULL) {
		G_UNLOCK (dict);
		return;
	}

	if (word != NULL) {
		i = ydpdict_find_word(dict, word);
		
		if (i != -1) {
			GtkTreePath *path;
			
			if ((path = gtk_tree_path_new_from_indices (i, -1))) {
				GtkTreeView *treeview;

				if ((treeview = GTK_TREE_VIEW (lookup_widget ("treeview1")))) {
					g_object_set_data (G_OBJECT (treeview), "dont-update", GINT_TO_POINTER (1));
					gtk_tree_view_set_cursor (treeview, path, NULL, 0);
				}

				gtk_tree_path_free (path);
			}
		}
	}

	if (index != -1)
		i = index;

	ydpdict_set_xhtml_header (dict, 1);
	ydpdict_set_xhtml_style (dict, "\nbody { background: white; margin-top: 0px; }\n");

	if (i != -1) {
		switch (dict_format) {
			case FORMAT_HTML:
				def = ydpdict_read_xhtml (dict, i);
				break;

			case FORMAT_RAW_RTF:
			{
				char *tmp;

				tmp = ydpdict_read_rtf (dict, i);
				def = g_markup_printf_escaped(xhtml_template, tmp);
				free(tmp);
				break;
			}

			case FORMAT_RAW_HTML:
			{
				char *tmp;

				tmp = ydpdict_read_xhtml (dict, i);
				def = g_markup_printf_escaped(xhtml_template, tmp);
				free(tmp);
				break;
			}

			case FORMAT_ALL:
			{
				char *tmp, *tmp_end, *tmp_rtf, *tmp_html;

				tmp = ydpdict_read_rtf (dict, i);
				tmp_rtf = g_markup_escape_text(tmp, -1);
				free(tmp);

				tmp = ydpdict_read_xhtml (dict, i);
				tmp_html = g_markup_escape_text(tmp, -1);

				tmp_end = strstr(tmp, "</body>");
				if (tmp_end != NULL)
					*tmp_end = 0;

				def = g_strdup_printf("%s<hr /><p>%s</p><hr /><p>%s</p></body></html>", tmp, tmp_rtf, tmp_html);

				free(tmp);
				free(tmp_html);
				free(tmp_rtf);

				break;
			}

			default:
				def = NULL;
				break;
		}
	} else {
		def = g_strdup_printf(xhtml_template, N_(""));
	}

	G_UNLOCK(dict);

	gtk_html_load_from_string (GTK_HTML (html_view), def, -1);

	free (def);
}


void
on_entry1_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
	g_print("on_entry1_changed\n");

	entry_update (-1, gtk_entry_get_text (GTK_ENTRY (editable)));
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
	entry_update (atoi (index), NULL);
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
		gtk_widget_event (lookup_widget ("entry1"), (GdkEvent*) event);
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
		switch_dictionary (YDPDICT_EN_PL);
}


void
on_toolbutton_pl_en_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_de_pl_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (YDPDICT_PL_EN);
}


void
on_toolbutton_de_pl_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_de_pl_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (YDPDICT_DE_PL);
}


void
on_toolbutton_pl_de_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_pl_de_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (YDPDICT_PL_DE);
}


void
on_toolbutton_test_toggled             (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data)
{
	g_print ("on_toolbutton_test_toggled()\n");

	if (gtk_toggle_tool_button_get_active (toggletoolbutton))
		switch_dictionary (YDPDICT_TEST);
}


void
on_combobox_format_changed             (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	dict_format = gtk_combo_box_get_active(combobox);
}

