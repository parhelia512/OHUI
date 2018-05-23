#pragma once

#ifdef XP_WIN
#include <windows.h>
#endif



// FROZEN APIs
#include "stdafx.h"
#include "nsStringAPI.h"

#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserChromeFocus.h"

#include "nsIContextMenuListener.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIInterfaceRequestor.h"
#include "nsIObserver.h"
#include "nsISHistoryListener.h"
#include "nsITooltipListener.h"
#include "nsIWebProgressListener.h"
#include "nsIWebBrowser.h"
#include "nsIIOService.h"   //nsIIOService

// NON-FROZEN APIs!
#include "nsIBaseWindow.h"
#include "nsIWebNavigation.h"

// GLUE APIs (not frozen, but safe because we're statically linking them)
#include "nsCOMPtr.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsWeakReference.h"
#include "nsIWidget.h"

class nsIWebBrowserChrome;

//#include "OHUI.h"
class WebBrowserChromeUI
{ 
public:
	static nsCString escape(const char* p);
    static /*HWND*/nsIWidget* CreateNativeWindow(nsIWebBrowserChrome* chrome);
	static /*HWND*/nsIWidget* GetBrowserFromChrome(nsIWebBrowserChrome *aChrome);
	static /*HWND*/nsIWidget* GetBrowserDlgFromChrome(nsIWebBrowserChrome *aChrome);  
    
    static void Destroy(nsIWebBrowserChrome* chrome);
    static void Destroyed(nsIWebBrowserChrome* chrome);
    static void SetFocus(nsIWebBrowserChrome *chrome);
    static void UpdateStatusBarText(nsIWebBrowserChrome *aChrome, const PRUnichar* aStatusText);
    static void UpdateCurrentURI(nsIWebBrowserChrome *aChrome);
    static void UpdateBusyState(nsIWebBrowserChrome *aChrome, PRBool aBusy);
    static void UpdateProgress(nsIWebBrowserChrome *aChrome, PRInt32 aCurrent, PRInt32 aMax);
    static void GetResourceStringById(PRInt32 aID, char ** aReturn);
    static void ShowContextMenu(nsIWebBrowserChrome *aChrome, PRUint32 aContextFlags, nsIDOMEvent *aEvent, nsIDOMNode *aNode);
    static void ShowTooltip(nsIWebBrowserChrome *aChrome, PRInt32 aXCoords, PRInt32 aYCoords, const PRUnichar *aTipText);
    static void HideTooltip(nsIWebBrowserChrome *aChrome);
    static void ShowWindow(nsIWebBrowserChrome *aChrome, PRBool aShow);
    static void SizeTo(nsIWebBrowserChrome *aChrome, PRInt32 aWidth, PRInt32 aHeight);
	static already_AddRefed<nsIFile>
		GetRegDirectory(const char* basename, const char* dirname, const char* leafname);
    
	static nsresult GetIOService(nsIIOService **ioService);
	static nsresult NewURI (nsIURI **result,
							const nsAString &spec,
							const char *charset = nsnull,
							nsIURI *baseURI = nsnull);

	static nsresult NewURI (nsIURI **result,
							const nsACString &spec,
							const char *charset = nsnull,
							nsIURI *baseURI = nsnull);
};
