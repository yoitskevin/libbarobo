#ifndef _GTKIMOBOTCONTROLLER_H_
#define _GTKIMOBOTCONTROLLER_H_

#include <gtk/gtk.h>
#include "gait.h"
#define MOTION_POSE 0
#define MOTION_MOVE 1

/* These function prototypes need to be in "C" so that gmodule can find them */
extern "C" {

int initialize();

/* Main Dialog */
void on_menuitem_connect_activate(GtkWidget* widget, gpointer data);

/* Connect Dialog */
void on_button_connect_clicked(GtkWidget* widget, gpointer data);
void on_button_connect_cancel_clicked(GtkWidget* widget, gpointer data);

/* Movement Handlers */
void on_button_forward_clicked(GtkWidget* widget, gpointer data);
void on_button_backward_clicked(GtkWidget* widget, gpointer data);
void on_button_rotateLeft_clicked(GtkWidget* widget, gpointer data);
void on_button_rotateRight_clicked(GtkWidget* widget, gpointer data);
void on_button_lfaceForward_clicked(GtkWidget* widget, gpointer data);
void on_button_lfaceBackward_clicked(GtkWidget* widget, gpointer data);
void on_button_rfaceForward_clicked(GtkWidget* widget, gpointer data);
void on_button_rfaceBackward_clicked(GtkWidget* widget, gpointer data);
void on_button_stop_clicked(GtkWidget* widget, gpointer data);
void on_button_home_clicked(GtkWidget* widget, gpointer data);
gboolean on_vscale_motorspeed0_change_value(
    GtkRange* range,
    GtkScrollType scroll,
    gdouble value,
    gpointer user_data);
void on_vscale_motorspeed0_value_changed(GtkRange* range, gpointer data);
gboolean on_vscale_motorspeed0_button_release_event(GtkRange* range, GdkEvent* event, gpointer data);
gboolean on_vscale_motorspeed1_button_release_event(GtkRange* range, GdkEvent* event, gpointer data);
gboolean on_vscale_motorspeed2_button_release_event(GtkRange* range, GdkEvent* event, gpointer data);
gboolean on_vscale_motorspeed3_button_release_event(GtkRange* range, GdkEvent* event, gpointer data);

/* Misc. */
void quick_message (GtkWidget* parent, gchar *message);
int getIterModelFromTreeSelection(GtkTreeView *treeView, GtkTreeModel **model, GtkTreeIter *iter);
}

extern GtkBuilder *builder;
extern br_comms_t *imobotComms;
extern GtkWidget  *window;
extern GtkWidget  *dialog_connect;
extern Gait* g_gaits[100];
extern int g_numGaits;
extern GtkVScale* scale_motorSpeeds[4];

#endif
