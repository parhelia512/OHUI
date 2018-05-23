
#pragma once
#include <nsIBaseWindow.h>
#include "../include/common.h"
#include "nsIWebProgressListener.h"
#include "BrowserWrapper.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gtk/gtkdialog.h>
#include <gdk/gdkkeysyms-compat.h>   //GDK_F5

class CMainFrame
{
public:
	CMainFrame();
	virtual ~CMainFrame();

	nativeWindow CreateWindow();
	CBrowserWrapper*  m_pBrowserWrapper;
};
