/*
    (c) 2009 by Leon Winter, see LICENSE file
*/

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>
#include <libsoup/soup.h>

/* macros */
#define LENGTH(x)   (sizeof(x)/sizeof(x[0]))

/* enums */
enum { ModeNormal, ModeInsert, ModeSearch, ModeWebsearch, ModeHints };                  /* modes */
enum { TargetCurrent, TargetNew };                                                      /* target */
/* bitmask,
    1 << 0:  0 = jumpTo,            1 = scroll
    1 << 1:  0 = top/down,          1 = left/right
    1 << 2:  0 = top/left,          1 = bottom/right
    1 << 3:  0 = paging/halfpage,   1 = line
    1 << 4:  0 = paging,            1 = halfpage aka buffer
*/
enum { ScrollJumpTo, ScrollMove };
enum { OrientationVert, OrientationHoriz = (1 << 1) };
enum { DirectionTop,
        DirectionBottom = (1 << 2),
        DirectionLeft = OrientationHoriz,
        DirectionRight = OrientationHoriz | (1 << 2) };
enum { UnitPage,
        UnitLine = (1 << 3),
        UnitBuffer = (1 << 4) };

/* structs here */
typedef union {
    int i;
    void *v;
} Arg;

typedef struct {
    guint mask;
    guint modkey;
    guint key;
    gboolean (*func)(const Arg *arg);
    const Arg arg;
} Key;

/* callbacks here */
static void window_destroyed_cb(GtkWidget* window, gpointer func_data);
static void webview_title_changed_cb(WebKitWebView* webview, WebKitWebFrame* frame, char* title, gpointer user_data);
static void webview_load_committed_cb(WebKitWebView* webview, WebKitWebFrame* frame, gpointer user_data);
static void webview_title_changed_cb(WebKitWebView* webview, WebKitWebFrame* frame, char* title, gpointer user_data);
static void webview_progress_changed_cb(WebKitWebView* webview, int progress, gpointer user_data);
static void webview_load_committed_cb(WebKitWebView* webview, WebKitWebFrame* frame, gpointer user_data);
static void webview_load_finished_cb(WebKitWebView* webview, WebKitWebFrame* frame, gpointer user_data);
static gboolean webview_navigation_cb(WebKitWebView* webview, WebKitWebFrame* frame, WebKitNetworkRequest* request,
                        WebKitWebPolicyDecision* decision, gpointer user_data);
static gboolean webview_new_window_cb(WebKitWebView* webview, WebKitWebFrame* frame, WebKitNetworkRequest* request,
                        WebKitWebNavigationAction *action, WebKitWebPolicyDecision *decision, gpointer user_data);
static gboolean webview_mimetype_cb(WebKitWebView* webview, WebKitWebFrame* frame, WebKitNetworkRequest* request,
                        char* mime_type, WebKitWebPolicyDecision* decision, gpointer user_data);
static gboolean webview_download_cb(WebKitWebView* webview, GObject* download, gpointer user_data);
static gboolean webview_keypress_cb(WebKitWebView* webview, GdkEventKey* event);
static void webview_hoverlink_cb(WebKitWebView* webview, char* title, char* link, gpointer data);
static gboolean webview_console_cb(WebKitWebView* webview, char* message, int line, char* source, gpointer user_data);

/* functions */
static gboolean scroll(const Arg* arg);

static void setup_modkeys();
static void setup_gui();
static void setup_signals(GObject* window, GObject* webview);

/* variables */

static GtkAdjustment* adjust_h;
static GtkAdjustment* adjust_v;
static GtkWidget* input;
static WebKitWebView* webview;

static unsigned int mode = ModeNormal;
static unsigned int count = 0;
static char* modkeys;
static char current_modkey;

#include "config.h"

/* callbacks */
void
window_destroyed_cb(GtkWidget* window, gpointer func_data) {
    gtk_main_quit();
}

void
webview_title_changed_cb(WebKitWebView* webview, WebKitWebFrame* frame, char* title, gpointer user_data) {

}

void
webview_progress_changed_cb(WebKitWebView* webview, int progress, gpointer user_data) {

}

void
webview_load_committed_cb(WebKitWebView* webview, WebKitWebFrame* frame, gpointer user_data) {

}

void
webview_load_finished_cb(WebKitWebView* webview, WebKitWebFrame* frame, gpointer user_data) {

}

gboolean
webview_navigation_cb(WebKitWebView* webview, WebKitWebFrame* frame, WebKitNetworkRequest* request,
                        WebKitWebPolicyDecision* decision, gpointer user_data) {

}

gboolean
webview_new_window_cb(WebKitWebView* webview, WebKitWebFrame* frame, WebKitNetworkRequest* request,
                        WebKitWebNavigationAction *action, WebKitWebPolicyDecision *decision, gpointer user_data) {

}

gboolean
webview_mimetype_cb(WebKitWebView* webview, WebKitWebFrame* frame, WebKitNetworkRequest* request,
                        char* mime_type, WebKitWebPolicyDecision* decision, gpointer user_data) {

}

gboolean
webview_download_cb(WebKitWebView* webview, GObject* download, gpointer user_data) {

}

gboolean
webview_keypress_cb(WebKitWebView* webview, GdkEventKey* event) {
    unsigned int i;

    switch (mode) {
    case ModeNormal:
        if(event->state == 0)
            if((event->keyval >= GDK_1 && event->keyval <= GDK_9)
            ||  (event->keyval == GDK_0 && count)) {
                count = (count ? count * 10 : 0) + (event->keyval - GDK_0);
                return TRUE;
            } else if(strchr(modkeys, event->keyval) && current_modkey != event->keyval) {
                current_modkey = event->keyval;
                return TRUE;
            }
        /* keybindings */
        for(i = 0; i < LENGTH(keys); i++)
            if(keys[i].mask == event->state && (!keys[i].modkey || keys[i].modkey == current_modkey) && keys[i].key == event->keyval && keys[i].func)
                if(keys[i].func(&keys[i].arg)) {
                    current_modkey = count = 0;
                    return TRUE;
                }
        return FALSE;
        break;
    }
}

void
webview_hoverlink_cb(WebKitWebView* webview, char* title, char* link, gpointer data) {

}

gboolean
webview_console_cb(WebKitWebView* webview, char* message, int line, char* source, gpointer user_data) {

}

/* funcs here */
gboolean
scroll(const Arg* arg) {
    GtkAdjustment* adjust = (arg->i & OrientationHoriz) ? adjust_h : adjust_v;

    if(arg->i & ScrollMove)
        gtk_adjustment_set_value(adjust, gtk_adjustment_get_value(adjust) +
            (arg->i & (1 << 2) ? 1 : -1) *      /* direction */
            ((arg->i & UnitLine || (arg->i & UnitBuffer && count)) ? (scrollstep * (count ? count : 1)) : (
                arg->i & UnitBuffer ? gtk_adjustment_get_page_size(adjust) / 2 :
                (count ? count : 1) * (gtk_adjustment_get_page_size(adjust) -
                    (gtk_adjustment_get_page_size(adjust) > pagingkeep ? pagingkeep : 0)))));
    else
        gtk_adjustment_set_value(adjust,
            ((arg->i & (1 << 2)) ?  gtk_adjustment_get_upper : gtk_adjustment_get_lower)(adjust));
    return TRUE;
}

void
setup_modkeys() {
    unsigned int i;
    modkeys = calloc(LENGTH(keys) + 1, sizeof(char));
    char* ptr = modkeys;

    for(i = 0; i < LENGTH(keys); i++)
        if(keys[i].modkey && !strchr(modkeys, keys[i].modkey))
            *(ptr++) = keys[i].modkey;
    modkeys = realloc(modkeys, &ptr[0] - &modkeys[0] + 1);
}

void
setup_gui() {
    GtkScrollbar* scroll_h = (GtkScrollbar*)gtk_hscrollbar_new(NULL);
    GtkScrollbar* scroll_v = (GtkScrollbar*)gtk_vscrollbar_new(NULL);
    adjust_h = gtk_range_get_adjustment((GtkRange*)scroll_h);
    adjust_v = gtk_range_get_adjustment((GtkRange*)scroll_v);
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkBox* box = (GtkBox*)gtk_vbox_new(FALSE, 0);
    input = gtk_entry_new();
    GtkWidget* viewport = gtk_scrolled_window_new(adjust_h, adjust_v);
    webview = (WebKitWebView*)webkit_web_view_new();
    WebKitWebSettings* settings = (WebKitWebSettings*)webkit_web_settings_new();

    // let config.h set up WebKitWebSettings
    gtk_widget_set_name(window, "WebKitBrowser");
#ifdef DISABLE_SCROLLBAR
    gtk_scrolled_window_set_policy((GtkScrolledWindow*)viewport, GTK_POLICY_NEVER, GTK_POLICY_NEVER);
#endif
    setup_signals((GObject*)window, (GObject*)webview);
    gtk_container_add((GtkContainer*)viewport, (GtkWidget*)webview);
    gtk_box_pack_start(box, input, FALSE, FALSE, 0);
    gtk_box_pack_start(box, viewport, TRUE, TRUE, 0);
    gtk_container_add((GtkContainer*)window, (GtkWidget*)box);
    gtk_widget_grab_focus((GtkWidget*)webview);
    gtk_widget_show_all(window);
    webkit_web_view_load_uri(webview, startpage);
    gtk_main();
}

void
setup_signals(GObject* window, GObject* webview) {
    /* window */
    g_signal_connect((GObject*)window,  "destroy", (GCallback)window_destroyed_cb, NULL);
    /* webview */
    g_signal_connect((GObject*)webview, "title-changed", (GCallback)webview_title_changed_cb, NULL);
    g_signal_connect((GObject*)webview, "load-progress-changed", (GCallback)webview_progress_changed_cb, NULL);
    g_signal_connect((GObject*)webview, "load-committed", (GCallback)webview_load_committed_cb, NULL);
    g_signal_connect((GObject*)webview, "load-finished", (GCallback)webview_load_finished_cb, NULL);
    g_signal_connect((GObject*)webview, "navigation-policy-decision-requested", (GCallback)webview_navigation_cb, NULL);
    g_signal_connect((GObject*)webview, "new-window-policy-decision-requested", (GCallback)webview_new_window_cb, NULL);
    g_signal_connect((GObject*)webview, "mime-type-policy-decision-requested", (GCallback)webview_mimetype_cb, NULL);
    g_signal_connect((GObject*)webview, "download-requested", (GCallback)webview_download_cb, NULL);
    g_signal_connect((GObject*)webview, "key-press-event", (GCallback)webview_keypress_cb, NULL);
    g_signal_connect((GObject*)webview, "hovering-over-link", (GCallback)webview_hoverlink_cb, NULL);
    g_signal_connect((GObject*)webview, "console-message", (GCallback)webview_console_cb, NULL);
}

int
main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    if(!g_thread_supported())
        g_thread_init(NULL);
    setup_modkeys();
    setup_gui();

    return EXIT_SUCCESS;
}
