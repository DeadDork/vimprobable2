/*
    (c) 2009 by Leon Winter
    (c) 2009-2012 by Hannes Schueller
    (c) 2009-2010 by Matto Fransen
    (c) 2010-2011 by Hans-Peter Deifel
    (c) 2010-2011 by Thomas Adam
    (c) 2011 by Albert Kim
    (c) 2013 Daniel Carl
    see LICENSE file
*/

/* Vimprobable version number */
#define VERSION "1.3.0"
#define INTERNAL_VERSION "Vimprobable2/"VERSION

extern Client client;

/* general settings */
char startpage[MAX_SETTING_SIZE]      = "http://www.vimprobable.org/";
char useragent[MAX_SETTING_SIZE]      = "Vimprobable2/" VERSION;
char acceptlanguage[MAX_SETTING_SIZE] = "";
static const gboolean enablePlugins     = FALSE; /* TRUE keeps plugins enabled */
static const gboolean enableJava        = FALSE; /* FALSE disables Java applets */
static const gboolean enablePagecache   = FALSE; /* TRUE turns on the page cache. */
static gboolean escape_input_on_load    = TRUE; /* TRUE will disable automatic focusing of input fields via Javascript*/
char temp_dir[MAX_SETTING_SIZE]         = "/tmp"; /* location of temporary files, default will be overridden if TEMPDIR is set */

/* appearance */
char statusbgcolor[MAX_SETTING_SIZE]    = "#000000";            /* background color for status bar */
char statuscolor[MAX_SETTING_SIZE]      = "#ffffff";            /* color for status bar */
char sslbgcolor[MAX_SETTING_SIZE]       = "#b0ff00";            /* background color for status bar with SSL url */
char sslinvalidbgcolor[MAX_SETTING_SIZE]= "#ff0000";            /* background color for status bar with unverified SSL url */
char sslcolor[MAX_SETTING_SIZE]         = "#000000";            /* color for status bar with SSL url */

                                        /*  normal,                 warning,                error       */
static const char *urlboxfont[]         = { "monospace normal 8",   "monospace normal 8",   "monospace bold 8"};
static const char *urlboxcolor[]        = { NULL,                   "#ff0000",              "#ffffff" };
static const char *urlboxbgcolor[]      = { NULL,                   NULL,                   "#ff0000" };

                                        /*  normal,                 error               */
static const char *completionfont[]     = { "monospace normal 8",   "monospace bold 8" };
                                                                                        /* topborder color */
static const char *completioncolor[]    = { "#000000",              "#ff00ff",              "#000000" };
                                                                                        /* current row background */
static const char *completionbgcolor[]  = { "#ffffff",              "#ffffff",              "#fff000" };
/* pango markup for prefix highliting:      opening,                closing             */
#define             COMPLETION_TAG_OPEN     "<b>"
#define             COMPLETION_TAG_CLOSE    "</b>"

static const char statusfont[]          = "monospace bold 8";   /* font for status bar */
#define             ENABLE_HISTORY_INDICATOR
#define             ENABLE_INCREMENTAL_SEARCH
#define             ENABLE_GTK_PROGRESS_BAR
#define             ENABLE_WGET_PROGRESS_BAR
static const int progressbartick        = 20;
static const char progressborderleft    = '[';
static const char progressbartickchar   = '=';
static const char progressbarcurrent    = '>';
static const char progressbarspacer     = ' ';
static const char progressborderright   = ']';

/* external handlers:
 * the handle (first string) contain what the handled links have to start with
 * the handlers (second string) contain the external applications which should be called for this sort of link
 *    %s can be used as a placeholder for the link argument after the handler
 *    e.g.: "mailto:user@example.org
 *       "handle" is "mailto:"
 *       "%s" will translate to "user@example.org"
 */
static URIHandler uri_handlers[] = {
    { "mailto:",          "x-terminal-emulator -e mutt %s" },
    { "vimprobableedit:", "x-terminal-emulator -e vim %s" },
    { "ftp://",           "x-terminal-emulator -e wget ftp://%s" },
};

/* cookies */
#define             ENABLE_COOKIE_SUPPORT
#define             COOKIES_STORAGE_FILENAME    "%s/vimprobable/cookies", client.config.config_base
#define             COOKIES_STORAGE_READONLY    TRUE   /* if TRUE new cookies will be lost if you quit */

/* downloads directory */
#define             DOWNLOADS_PATH              "%s/Downloads/vimprobable", getenv("HOME")

/* font size */
#define             DEFAULT_FONT_SIZE           12

/* user styles */
#define             USER_STYLESHEET             "%s/vimprobable/style.css", client.config.config_base

/* user javascript */
#define             ENABLE_USER_SCRIPTFILE
#define             USER_SCRIPTFILE             "%s/vimprobable/scripts.js", client.config.config_base

/* ssl */
static gboolean strict_ssl              = TRUE; /* FALSE will accept any SSL certificate at face value */
static char ca_bundle[MAX_SETTING_SIZE] = "/etc/ssl/certs/ca-bundle.crt";

/* proxy */
static const gboolean use_proxy         = TRUE; /* TRUE if you're going to use a proxy (whose address
                                                  is specified in http_proxy environment variable), false otherwise */
/* scrolling */
static unsigned int scrollstep          = 40;   /* cursor difference in pixel */
static unsigned int pagingkeep          = 40;   /* pixels kept when paging */
#define             DISABLE_SCROLLBAR

/* searching */
#define             ENABLE_MATCH_HIGHLITING
static const int searchoptions          = CaseInsensitive | Wrapping;
gboolean complete_case_sensitive        = TRUE;

/* search engines */
static Searchengine searchengines[] = {
    { "dd",         "https://duckduckgo.com/?q=%s" },
    { "dd",         "https://duckduckgo.com/?q=%s" },
    { "d",          "https://duckduckgo.com/html/?q=%s" },
    { "dic",        "http://dictionary.reference.com/browse/%s" },
    { "g",          "https://encrypted.google.com/#bav=on.2&q=%s" },
    { "is",         "https://ixquick.com/do/search?cat=web&cmd=process_search&language=english&engine0=v1all&query=%s&abp=-1&src=&prfh=lang_homepageEEEs%2Fwhite%2Feng%2FN1Nfont_sizeEEEmediumN1Nrecent_results_filterEEE1N1Nlanguage_uiEEEenglishN1Ndisable_open_in_new_windowEEE1N1NsslEEE1N1Ncolor_schemeEEEnewN1Ndisable_family_filterEEE1N1Nnum_of_resultsEEE100N1Npicture_privacyEEEonN1Nenable_exact_phraseEEE0N1Ndisable_video_family_filterEEE1N1NsuggestionsEEE0N1N&suggestOn=0" },
    { "ii",         "https://s1-us4.ixquick.com/do/search?nosteeraway=1&abp=-1&exrr=0&prfh=lang_homepageEEEs%2Fwhite%2Feng%2FN1Nfont_sizeEEEmediumN1Nrecent_results_filterEEE1N1Nlanguage_uiEEEenglishN1Ndisable_open_in_new_windowEEE1N1NsslEEE1N1Ncolor_schemeEEEnewN1Ndisable_family_filterEEE1N1Nnum_of_resultsEEE100N1Npicture_privacyEEEonN1Nenable_exact_phraseEEE0N1Ndisable_video_family_filterEEE1N1NsuggestionsEEE0N1N&cat=pics&suggeston=0&flag_ac=0&language=english&cmd=process_search&query=%s&enginecount=3&lui=english" },
    { "leo",        "http://dict.leo.org/#/search=%s" },
    { "mw",         "http://www.merriam-webster.com/dictionary/%s" },
    { "s",          "https://startpage.com/do/search?cat=web&cmd=process_search&language=english&engine0=v1all&query=%s&abp=-1&prfh=lang_homepageEEEs%2Fair%2Feng%2FN1Nenable_post_methodEEE0N1Nfont_sizeEEEmediumN1Nrecent_results_filterEEE0N1Nlanguage_uiEEEenglishN1Ndisable_open_in_new_windowEEE1N1NsslEEE1N1Ndisable_family_filterEEE1N1Nnum_of_resultsEEE100N1Npicture_privacyEEEonN1Nenable_exact_phraseEEE0N1Ndisable_video_family_filterEEE1N1NsuggestionsEEE0N1N&suggestOn=0" },
    { "ths",        "http://thesaurus.com/browse/%s" },
    { "w",          "https://secure.wikimedia.org/wikipedia/en/w/index.php?title=Special%%3ASearch&search=%s&go=Go" },
};

static char defaultsearch[MAX_SETTING_SIZE] = "d";

/* command mapping */
Command commands[COMMANDSIZE] = {
    /* command,                                        	function,         argument */
    { "ba",                                            	navigate,         {NavigationBack} },
    { "back",                                          	navigate,         {NavigationBack} },
    { "ec",                                            	script,           {Info} },
    { "echo",                                          	script,           {Info} },
    { "echoe",                                         	script,           {Error} },
    { "echoerr",                                       	script,           {Error} },
    { "fw",                                            	navigate,         {NavigationForward} },
    { "fo",                                            	navigate,         {NavigationForward} },
    { "forward",                                       	navigate,         {NavigationForward} },
    { "javascript",                                    	script,           {Silent} },
    { "o",                                             	open_arg,         {TargetCurrent} },
    { "open",                                          	open_arg,         {TargetCurrent} },
    { "q",                                             	quit,             {0} },
    { "quit",                                          	quit,             {0} },
    { "re",                                            	navigate,         {NavigationReload} },
    { "re!",                                           	navigate,         {NavigationForceReload} },
    { "reload",                                        	navigate,         {NavigationReload} },
    { "reload!",                                       	navigate,         {NavigationForceReload} },
    { "qt",                                             search_tag,       {0} },
    { "st",                                            	navigate,         {NavigationCancel} },
    { "stop",                                          	navigate,         {NavigationCancel} },
    { "t",                                             	open_arg,         {TargetNew} },
    { "tabopen",                                       	open_arg,         {TargetNew} },
    { "print",                                         	print_frame,      {0} },
    { "bma",                                           	bookmark,         {0} },
    { "bookmark",                                      	bookmark,         {0} },
    { "source",                                        	view_source,      {0} },
    { "openeditor",                                   	open_editor,      {0} },
    { "set",                                           	browser_settings, {0} },
    { "map",                                           	mappings,         {0} },
    { "inspect",                                        open_inspector,   {0} },
    { "jumpleft",                                       scroll,           {ScrollJumpTo   | DirectionLeft} },
    { "jumpright",                                      scroll,           {ScrollJumpTo   | DirectionRight} },
    { "jumptop",                                        scroll,           {ScrollJumpTo   | DirectionTop} },
    { "jumpbottom",                                     scroll,           {ScrollJumpTo   | DirectionBottom} },
    { "pageup",                                         scroll,           {ScrollMove     | DirectionTop      | UnitPage} },	
    { "pagedown",                                       scroll,           {ScrollMove     | DirectionBottom   | UnitPage} },
    { "navigationback",   	                            navigate,         {NavigationBack} },
    { "navigationforward",	                            navigate,         {NavigationForward} },
    { "scrollleft",                                     scroll,           {ScrollMove     | DirectionLeft     | UnitLine} },
    { "scrollright",                                    scroll,           {ScrollMove     | DirectionRight    | UnitLine} },
    { "scrollup",                                       scroll,           {ScrollMove     | DirectionTop      | UnitLine} },
    { "scrolldown",                                     scroll,           {ScrollMove     | DirectionBottom   | UnitLine} },
    { "zi",                                             zoom,             {ZoomIn         | ZoomText} },
    { "zo",                                             zoom,             {ZoomOut        | ZoomText} },
    { "pgzi",                                           zoom,             {ZoomIn         | ZoomFullContent} },
    { "pgzo",                                           zoom,             {ZoomOut        | ZoomFullContent} },
};

/* mouse bindings
   you can use MOUSE_BUTTON_1 to MOUSE_BUTTON_5
*/
static Mouse mouse[] = {
    /* modmask,             modkey,         button,            function,        argument */
    { 0,                    0,              MOUSE_BUTTON_2,    paste,           {TargetCurrent  | ClipboardPrimary  | ClipboardGTK, client.state.rememberedURI} },
    { GDK_CONTROL_MASK,     0,              MOUSE_BUTTON_2,    paste,           {TargetNew  | ClipboardPrimary  | ClipboardGTK} },
    { GDK_CONTROL_MASK,     0,              MOUSE_BUTTON_1,    open_remembered, {TargetNew} },
};

/* settings (arguments of :set command) */
static Setting browsersettings[] = {
    /* public name,      internal variable   webkit setting                 integer value?  boolean value?   colour value?   reload page? */
    { "useragent",       useragent,          "user-agent",                  FALSE,          FALSE,           FALSE,          FALSE  },
    { "scripts",         NULL,               "enable-scripts",              FALSE,          TRUE,            FALSE,          FALSE  },
    { "plugins",         NULL,               "enable-plugins",              FALSE,          TRUE,            FALSE,          FALSE  },
    { "pagecache",       NULL,               "enable-page-cache",           FALSE,          TRUE,            FALSE,          FALSE  },
    { "java",            NULL,               "enable-java-applet",          FALSE,          TRUE,            FALSE,          FALSE  },
    { "images",          NULL,               "auto-load-images",            FALSE,          TRUE,            FALSE,          FALSE  },
    { "shrinkimages",    NULL,               "auto-shrink-images",          FALSE,          TRUE,            FALSE,          FALSE  },
    { "cursivefont",     NULL,               "cursive-font-family",         FALSE,          FALSE,           FALSE,          FALSE  },
    { "defaultencoding", NULL,               "default-encoding",            FALSE,          FALSE,           FALSE,          FALSE  },
    { "defaultfont",     NULL,               "default-font-family",         FALSE,          FALSE,           FALSE,          FALSE  },
    { "fontsize",        NULL,               "default-font-size",           TRUE,           FALSE,           FALSE,          FALSE  },
    { "monofontsize",    NULL,               "default-monospace-font-size", TRUE,           FALSE,           FALSE,          FALSE  },
    { "caret",           NULL,               "enable-caret-browsing",       FALSE,          TRUE,            FALSE,          FALSE  },
    { "fantasyfont",     NULL,               "fantasy-font-family",         FALSE,          FALSE,           FALSE,          FALSE  },
    { "minimumfontsize", NULL,               "minimum-font-size",           TRUE,           FALSE,           FALSE,          FALSE  },
    { "monofont",        NULL,               "monospace-font-family",       FALSE,          FALSE,           FALSE,          FALSE  },
    { "backgrounds",     NULL,               "print-backgrounds",           FALSE,          TRUE,            FALSE,          FALSE  },
    { "sansfont",        NULL,               "sans-serif-font-family",      FALSE,          FALSE,           FALSE,          FALSE  },
    { "seriffont",       NULL,               "serif-font-family",           FALSE,          FALSE,           FALSE,          FALSE  },
    { "stylesheet",      NULL,               "user-stylesheet-uri",         FALSE,          FALSE,           FALSE,          FALSE  },
    { "resizetextareas", NULL,               "resizable-text-areas",        FALSE,          TRUE,            FALSE,          FALSE  },
    { "webinspector",    NULL,               "enable-developer-extras",     FALSE,          TRUE,            FALSE,          FALSE  },

    { "homepage",        startpage,          "",                            FALSE,          FALSE,           FALSE,          FALSE  },
    { "statusbgcolor",   statusbgcolor,      "",                            FALSE,          FALSE,           TRUE,           TRUE   },
    { "statuscolor",     statuscolor,        "",                            FALSE,          FALSE,           TRUE,           TRUE   },
    { "sslbgcolor",      sslbgcolor,         "",                            FALSE,          FALSE,           TRUE,           TRUE   },
    { "sslcolor",        sslcolor,           "",                            FALSE,          FALSE,           TRUE,           TRUE   },
    { "acceptlanguage",  acceptlanguage,     "",                            FALSE,          FALSE,           FALSE,          FALSE  },
    { "defaultsearch",   defaultsearch,      "",                            FALSE,          FALSE,           FALSE,          FALSE  },
    { "qmark",           NULL,               "",                            FALSE,          FALSE,           FALSE,          FALSE  },
    { "proxy",           NULL,               "",                            FALSE,          TRUE,            FALSE,          FALSE  },
    { "windowsize",      NULL,               "",                            FALSE,          FALSE,           FALSE,          FALSE  },
    { "scrollbars",      NULL,               "",                            FALSE,          TRUE,            FALSE,          FALSE  },
    { "statusbar",       NULL,               "",                            FALSE,          TRUE,            FALSE,          FALSE  },
    { "inputbox",        NULL,               "",                            FALSE,          TRUE,            FALSE,          FALSE  },
    { "completioncase",  NULL,               "",                            FALSE,          TRUE,            FALSE,          FALSE  },
    { "escapeinput",     NULL,               "",                           FALSE,          TRUE,            FALSE,          FALSE  },
    { "strictssl",       NULL,               "",                            FALSE,          TRUE,            FALSE,          FALSE  },
    { "cabundle",        ca_bundle,          "",                            FALSE,          FALSE,           FALSE,          FALSE  },
    { "tempdir",         temp_dir,           "",                            FALSE,          FALSE,           FALSE,          FALSE  },
};
