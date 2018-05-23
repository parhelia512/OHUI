
#include "stdafx.h"
#include "nsAppDirectoryServiceDefs.h"
#include "nsDirectoryServiceDefs.h" 
#include "nsStringAPI.h"
#include "nsXPCOMGlue.h"

#include "nsIClipboardCommands.h"
#include "nsIInterfaceRequestor.h"
#include "nsIObserverService.h"
#include "nsIObserver.h"
#include "nsIURI.h"
#include "nsIWebBrowserFocus.h"
#include "nsIWindowWatcher.h"
// NON-FROZEN APIs!
#include "nsIBaseWindow.h"
#include "nsIWebNavigation.h"
#include "nsDocShellLoadTypes.h"

#include "MozUtil.h"
#include "BrowserWrapper.h"


#include "../images/48x48.xpm"
#include "MainFrame.h"

CMainFrame::CMainFrame()
{
    m_pBrowserWrapper = new  CBrowserWrapper();
}

CMainFrame::~CMainFrame()
{
	if(m_pBrowserWrapper)
	{
		m_pBrowserWrapper->DestroyBrowser();
		delete m_pBrowserWrapper;
		m_pBrowserWrapper=0;
	}
}

/* This callback quits the program */
static 
gint wnd_del_callback( GtkWidget *widget,
                   GdkEvent *event,
                   gpointer data )
{
  gtk_main_quit ();
  
  return FALSE;
}

nativeWindow CMainFrame::CreateWindow()
{
    GtkWidget *wnd;
    GdkPixbuf *app_icon_pixbuf=0;
    nsresult rv;
    
    wnd=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app_icon_pixbuf =  gdk_pixbuf_new_from_xpm_data (_8x48);
    if (app_icon_pixbuf)
    {
        gtk_window_set_icon (GTK_WINDOW (wnd), app_icon_pixbuf);
        g_object_unref (app_icon_pixbuf);
    } 
    gtk_window_set_default_size(GTK_WINDOW (wnd),800,600);
    gtk_window_set_position(GTK_WINDOW (wnd), GTK_WIN_POS_CENTER);
    
    /* NULL window variable when window is closed */    
    g_signal_connect(G_OBJECT (wnd), "delete_event",
                     G_CALLBACK (wnd_del_callback),
                     NULL);    
    return wnd;
}
