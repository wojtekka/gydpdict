#include <gtk/gtk.h>
#include <ydpdict/ydpdict.h>


GtkWidget*
create_gtkhtml1 (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data);

void
on_entry1_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_treeview1_cursor_changed            (GtkTreeView     *treeview,
                                        gpointer         user_data);

gboolean
on_treeview1_key_press_event           (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_toolbutton_en_pl_clicked            (GtkToggleToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbutton_pl_en_clicked            (GtkToggleToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbutton_de_pl_clicked            (GtkToggleToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbutton_pl_de_clicked            (GtkToggleToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbutton_copy_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

gboolean
on_entry1_key_press_event              (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_toolbutton_en_pl_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data);

void
on_toolbutton_pl_en_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data);

void
on_toolbutton_de_pl_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data);

void
on_toolbutton_pl_de_toggled            (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data);
