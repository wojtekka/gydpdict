#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <ydpdict/ydpdict.h>

typedef enum {
	YDPDICT_EN_PL = 100,
	YDPDICT_PL_EN = 101,
	YDPDICT_DE_PL = 200,
	YDPDICT_PL_DE = 201,
	YDPDICT_TEST = 999
} dict_type_t;

typedef enum {
	FORMAT_HTML,
	FORMAT_RAW_RTF,
	FORMAT_RAW_HTML,
	FORMAT_ALL,
} dict_format_t;

ydpdict_t *dict;
G_LOCK_EXTERN (dict);

dict_type_t dict_type;
dict_format_t dict_format;

void
treeview1_init (GtkWidget *window1);

void
switch_dictionary(GtkWidget *any, dict_type_t type);

