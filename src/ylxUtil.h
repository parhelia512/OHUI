
/* For printf. */
#include <stdio.h>
/* For realpath. */
#include <stdlib.h>
/* For PATH_MAX (the maximum length of a path). */
#include <sys/param.h>
/* For strerror. */
#include <string.h>
/* For errno. */
#include <errno.h>

#include <gtk/gtk.h>

char *_T(char *string);
char *__T(char *string);

/* 
 * Standard gettext macros.
/run/media/lixing/20170318_4TB/src/common/searchmonkey-0.8.1/src/support.h
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  define Q_(String) g_strip_context ((String), gettext (String))
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define Q_(String) g_strip_context ((String), (String))
#  define N_(String) (String)
#endif



/*
 * Private Functions.
 */

/* This is used to create the pixmaps used in the interface. */
GdkPixbuf* create_pixbuf (GtkWidget       *widget,
                          const gchar     *filename);


