#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <ydpdict/ydpdict.h>

ydpdict_t dict;
int dict_loaded;
int dict_num;
G_LOCK_EXTERN (dict);

enum {
	YDPDICT_EN_PL = 100,
	YDPDICT_PL_EN = 101,
	YDPDICT_DE_PL = 200,
	YDPDICT_PL_DE = 201
};

void
treeview1_init (GtkWidget *window1);

void
switch_dictionary(GtkWidget *any, int dict);

