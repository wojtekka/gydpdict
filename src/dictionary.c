#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <ydpdict/ydpdict.h>

#include <stdio.h>
#include <string.h>

#include "support.h"
#include "dictionary.h"
#include "interface.h"

ydpdict_t dict;
G_LOCK_DEFINE (dict);
int dict_loaded;
int dict_num;

const gchar *config_path = "/usr/local/share/ydpdict";


void
treeview1_init (GtkWidget *window1)
{
	GtkCellRenderer *renderer;
	GtkWidget *treeview1;

	treeview1 = lookup_widget(window1, "treeview1");

	if (!treeview1)
		return;

	renderer = gtk_cell_renderer_text_new();

	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "foo", renderer, "text", 0, NULL);
}


void
treeview_fill (GtkWidget *view, ydpdict_t *dict)
{
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	int i;

	store = gtk_list_store_new(1, G_TYPE_STRING);

	for (i = 0; i < dict->word_count; i++) {
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, dict->words[i], -1);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(store));

	g_object_unref(store);
}


gpointer
load_dictionary (gpointer data)
{
	GtkWidget *window1, *window2 = data, *treeview1;
	gchar *dat, *idx;
	gint num;

	G_LOCK (dict);

	treeview1 = g_object_get_data (G_OBJECT (window2), "treeview1");
	window1 = g_object_get_data (G_OBJECT (window2), "window1");
	num = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (window2), "dict_num"));

	g_print("treeview1 = %p\n", treeview1);

	if (dict_loaded) {
		gdk_threads_enter ();
		gtk_tree_view_set_model (GTK_TREE_VIEW (treeview1), NULL);
		gdk_threads_leave ();
		ydpdict_close (&dict);
		dict_loaded = 0;
		dict_num = 0;
	}

	memset (&dict, 0, sizeof(dict));

	dat = g_strdup_printf ("%s/dict%d.dat", config_path, num);
	idx = g_strdup_printf ("%s/dict%d.idx", config_path, num);

	g_print ("opening\n");
	if (ydpdict_open (&dict, dat, idx, YDPDICT_ENCODING_UTF8) == 0) {
		g_print ("opened\n");
		dict_loaded = 1;
		dict_num = num;

		g_print ("filling\n");
		gdk_threads_enter ();
		treeview_fill (treeview1, &dict);
		gdk_threads_leave ();
		g_print ("filled\n");
	} else {
		fprintf (stderr, _("Unable to open dictionary."));
	}

	g_free (dat);
	g_free (idx);

	G_UNLOCK (dict);

	gtk_widget_set_sensitive (window1, TRUE);
	g_print("destroying\n");
	g_source_remove (GPOINTER_TO_INT (g_object_get_data (G_OBJECT (data), "source_id")));
	gtk_widget_hide (GTK_WIDGET (data));
	gtk_widget_destroy (GTK_WIDGET (data));
	g_print("destroyed\n");
}


gboolean tick_progress (gpointer data)
{
	gdk_threads_enter ();
	gtk_progress_bar_pulse (GTK_PROGRESS_BAR (data));
	gdk_threads_leave ();

	return TRUE;
}


void
switch_dictionary (GtkWidget *any, int num)
{
	GtkWidget *window1, *window2;
	gchar *dat, *idx;
	guint sid;

	G_LOCK (dict);

	if (dict_loaded && dict_num == num) {
		G_UNLOCK (dict);
		return;
	}

	G_UNLOCK (dict);

	window1 = lookup_widget(any, "window1");
	gtk_widget_set_sensitive(window1, FALSE);

	window2 = create_window2 ();
	g_signal_connect (window2, "delete_event", G_CALLBACK (gtk_true), NULL);
	gtk_widget_show (window2);

	sid = g_timeout_add (100, tick_progress, lookup_widget (window2, "progressbar1"));
	g_object_set_data (G_OBJECT (window2), "source_id", GINT_TO_POINTER (sid));
	g_object_set_data (G_OBJECT (window2), "dict_num", GINT_TO_POINTER (num));
	g_object_set_data (G_OBJECT (window2), "window1", window1);
	g_object_set_data (G_OBJECT (window2), "treeview1", lookup_widget (any, "treeview1"));
	
	g_thread_create (load_dictionary, window2, FALSE, NULL);
}


