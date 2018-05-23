
#include "ylxUtil.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

static GList *pixmaps_directories = NULL;

static
void get_app_realpath (char * argv0)
{
    char resolved_path[PATH_MAX];

    if (realpath (argv0, resolved_path) == 0) { 
	fprintf (stderr, "realpath failed: %s\n", strerror (errno));
    }
    else {
	printf ("Program's full path is '%s'\n", resolved_path);
    }
}

/* Use this function to set the directory containing installed pixmaps. */
void
add_pixmap_directory                   (const gchar     *directory)
{
  pixmaps_directories = g_list_prepend (pixmaps_directories,
                                        g_strdup (directory));

}


/* This is an internally used function to find pixmap files. */
static gchar*
find_pixmap_file                       (const gchar     *filename)
{
  GList *elem;

  /* We step through each of the pixmaps directory to find it. */
  elem = pixmaps_directories; 
  while (elem)
    {
      
      gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
                                         G_DIR_SEPARATOR_S, filename);
      if (g_file_test (pathname, G_FILE_TEST_EXISTS))
        return pathname;
      g_free (pathname);
      elem = elem->next;
    }
  return NULL;
}

/* This is an internally used function to create pixmaps. */
GdkPixbuf*
create_pixbuf                          (const gchar     *filename)
{
  gchar *pathname = NULL;
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  

  if (!filename || !filename[0])
      return NULL;

  pathname = find_pixmap_file (filename);
  
  if (!pathname)
    {
      //g_warning (_("Couldn't find pixmap file: %s"), filename);
      return NULL;
    }

  pixbuf = gdk_pixbuf_new_from_file (pathname, &error);
  if (!pixbuf)
    {
      fprintf (stderr, "Failed to load pixbuf file: %s: %s\n",
               pathname, error->message);
      g_error_free (error);
    }
  g_free (pathname);
  return pixbuf;
}
 

char *_T(char *string)
{
    return(g_locale_to_utf8(string, -1, NULL, NULL, NULL));
} 

char *__T(char *string)
{
    return(g_locale_from_utf8(string, -1, NULL, NULL, NULL));
}
