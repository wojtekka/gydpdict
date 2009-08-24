#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <ydpdict/ydpdict.h>
#include <libgtkhtml/gtkhtml.h>

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "dictionary.h"
#include "callbacks.h"
#include "main.h"

ydpdict_t *dict;
G_LOCK_DEFINE (dict);

dict_type_t dict_type;
dict_format_t dict_format;

const gchar *config_path = "/usr/local/share/ydpdict";
static guint progress_source_id;


void widgets_init (void)
{
	GtkWidget *treeview1;
	GtkWidget *viewport1;
	
	treeview1 = lookup_widget("treeview1");

	if (treeview1 != NULL) {
		GtkCellRenderer *renderer;

		renderer = gtk_cell_renderer_text_new();

		gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview1), -1, "foo", renderer, "text", 0, NULL);
	}

	html_view = create_gtkhtml1();
	viewport1 = lookup_widget("viewport1");
	gtk_container_add(GTK_CONTAINER(viewport1), html_view);
	gtk_widget_show(html_view);
}


void treeview_fill (GtkWidget *view, ydpdict_t *dict)
{
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	int i, count;

	store = gtk_list_store_new (1, G_TYPE_STRING);

	count = ydpdict_get_count(dict);

	for (i = 0; i < count; i++) {
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, ydpdict_get_word(dict, i), -1);
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(store));

	g_object_unref(store);
}


gpointer
load_dictionary (gpointer data)
{
	GtkWidget *treeview1;
	gchar *dat, *idx;
	gint num;
	struct timeval tv1, tv2;
	gint td;

	G_LOCK (dict);

	treeview1 = lookup_widget ("treeview1");
	num = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (progress_window), "dict_type"));

	g_print("treeview1 = %p\n", treeview1);

	if (dict) {
		gdk_threads_enter ();
		gtk_tree_view_set_model (GTK_TREE_VIEW (treeview1), NULL);
		gdk_threads_leave ();
		ydpdict_close (dict);
		dict = NULL;
		dict_type = 0;
	}

	dat = g_strdup_printf ("%s/dict%d.dat", config_path, num);
	idx = g_strdup_printf ("%s/dict%d.idx", config_path, num);

	g_print ("opening %s\n", dat);

	dict = ydpdict_open (dat, idx, YDPDICT_ENCODING_UTF8);

	if (dict != NULL) {
		g_print ("opened\n");
		dict_type = num;

		g_print ("filling\n");
		gdk_threads_enter ();
		treeview_fill (treeview1, dict);
		gdk_threads_leave ();
		g_print ("filled\n");
	} else {
		fprintf (stderr, _("Unable to open dictionary.\n"));
	}

	g_free (dat);
	g_free (idx);

	G_UNLOCK (dict);

	gtk_widget_set_sensitive (main_window, TRUE);
	g_print("destroying\n");
	g_source_remove (GPOINTER_TO_INT (g_object_get_data (G_OBJECT (progress_window), "source_id")));
	gtk_widget_hide (GTK_WIDGET (progress_window));
	g_print("destroyed\n");
}


gboolean progress_tick (gpointer data)
{
	gdk_threads_enter ();
	gtk_progress_bar_pulse (GTK_PROGRESS_BAR (data));
	gdk_threads_leave ();

	return TRUE;
}


void switch_dictionary (dict_type_t type)
{
	gchar *dat, *idx;
	guint sid;

	G_LOCK (dict);

	if (dict != NULL && dict_type == type) {
		G_UNLOCK (dict);
		return;
	}

	G_UNLOCK (dict);

	gtk_widget_set_sensitive(main_window, FALSE);

	gtk_widget_show (progress_window);

	sid = g_timeout_add (100, progress_tick, lookup_widget ("progressbar1"));
	g_object_set_data (G_OBJECT (progress_window), "source_id", GINT_TO_POINTER (sid));
	g_object_set_data (G_OBJECT (progress_window), "dict_type", GINT_TO_POINTER (type));
	
	g_thread_create (load_dictionary, NULL, FALSE, NULL);
}


