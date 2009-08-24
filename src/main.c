#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <ydpdict/ydpdict.h>

#include "dictionary.h"
#include "main.h"

#define UI_FILE "gydpdict.ui"

GtkBuilder *builder;
GtkWidget *main_window;
GtkWidget *progress_window;
GtkWidget *html_view;

GtkWidget* lookup_widget (const char *name)
{
	return GTK_WIDGET (gtk_builder_get_object (builder, name));
}

int main (int argc, char *argv[])
{
	ydpdict_t *dict;
	gboolean debug = FALSE;
	GError* error = NULL;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_set_locale ();

	g_thread_init (NULL);
	gdk_threads_init ();
	gdk_threads_enter ();

	gtk_init (&argc, &argv);

	// XXX dodać prawdziwe getopt()
	if (argc > 1 && strcmp(argv[1], "--debug") == 0)
		debug = 1;

//	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
//	add_pixmap_directory ("../pixmaps");

	builder = gtk_builder_new ();

	if (!gtk_builder_add_from_file (builder, UI_FILE, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	/* This is important */
	gtk_builder_connect_signals (builder, NULL);

	main_window = lookup_widget("main_window");
	progress_window = lookup_widget("progress_window");

	if (debug) {
		GtkWidget *tmp;

		tmp = lookup_widget("toolbutton_test");
		gtk_widget_show(tmp);

		tmp = lookup_widget("separatortoolitem_format");
		gtk_widget_show(tmp);

		tmp = lookup_widget("combobox_format");
		gtk_combo_box_set_active(GTK_COMBO_BOX(tmp), FORMAT_ALL);
		gtk_widget_show(tmp);
	}

	widgets_init ();

	gtk_widget_show (main_window);

	switch_dictionary (YDPDICT_EN_PL);

	gtk_main ();

	g_object_unref (builder);
	
	gdk_threads_leave ();

	return 0;
}

