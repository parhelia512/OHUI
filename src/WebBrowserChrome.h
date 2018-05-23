/* ***** BEGIN LICENSE BLOCK *****
 * Version: Mozilla-sample-code 1.0
 *
 * Copyright (c) 2002 Netscape Communications Corporation and
 * other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this Mozilla sample software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __WebBrowserChrome__
#define __WebBrowserChrome__

// OS headers
#ifdef XP_WIN
#include <windows.h>
#else

#endif

// FROZEN APIs

#include "nsStringAPI.h"

#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserChromeFocus.h"

#include "nsIContextMenuListener.h"
#include "nsIContextMenuListener2.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIInterfaceRequestor.h"
#include "nsIObserver.h"
#include "nsISHistoryListener.h"
#include "nsITooltipListener.h"
#include "nsIWebProgressListener.h"
#include "nsIWebBrowser.h"
#include "nsIPromptService.h"

// GLUE APIs (not frozen, but safe because we're statically linking them)
#include "nsCOMPtr.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsWeakReference.h"
#include "WebBrowserChromeUI.h"
#include "nsIWebBrowserSetup.h"
#include "nsIDOMEventListener.h"

class WebBrowserChrome   : public nsIWebBrowserChrome
                           ,public nsIWebProgressListener
                           ,public nsIEmbeddingSiteWindow
                           ,public nsIInterfaceRequestor
                           ,public nsISHistoryListener
                           ,public nsIObserver
                           ,public nsITooltipListener
                           ,public nsSupportsWeakReference
                           ,public nsIPromptService
{
public:
    WebBrowserChrome(/*HWND*/nsIWidget*);
    virtual ~WebBrowserChrome();

    NS_DECL_ISUPPORTS
    NS_DECL_NSIWEBBROWSERCHROME
    NS_DECL_NSIWEBPROGRESSLISTENER
    NS_DECL_NSIEMBEDDINGSITEWINDOW
    NS_DECL_NSIINTERFACEREQUESTOR
    NS_DECL_NSISHISTORYLISTENER
    NS_DECL_NSIOBSERVER
    NS_DECL_NSITOOLTIPLISTENER
    NS_DECL_NSIPROMPTSERVICE

    nsresult CreateBrowser(PRInt32 aX, PRInt32 aY, PRInt32 aCX, PRInt32 aCY,
                           nsIWebBrowser **aBrowser);

    void     SetParent(nsIWebBrowserChrome *aParent)
               { mDependentParent = aParent; }
   
	static nsresult	CreateBrowserWindow(PRUint32 aChromeFlags,
                                        nsIWebBrowserChrome *aParent,
                                        nsIWebBrowserChrome **aNewWindow, /*HWND*/nsIWidget* h);

	static nsresult OpenWebPage(const char *url, /*HWND*/nsIWidget* h);
	static void SetIWBrwChrm(nsIWebBrowserChrome* aChrome, /*HWND*/nsIWidget* hWnd);
	static nsIWebBrowserChrome* GetIWBrwChrm(/*HWND*/nsIWidget*);
	static nsresult ResizeEmbedding(nsIWebBrowserChrome* chrome);
	static void EnableChromeWindow(nsIWebBrowserChrome *aWindow, PRBool aEnabled);
	static PRUint32 RunEventLoop(PRBool &aRunCondition);
	static nsresult SetupFlashPluginPath(const nsAutoString&  nsKey, const nsAutoString& nsFlashPluginPath);


    nsCOMPtr<nsIBaseWindow>  mBaseWindow; // [OWNER]
protected:
    nsresult SendHistoryStatusMessage(nsIURI * aURI, char * operation, PRInt32 info1=0, PRUint32 info2=0);

    void ContentFinishedLoading();

    /*
      on windows, nativeWindow is HWND
      on linux,   nativeWindow is nsIWidget*
    */
    nativeWindow mNativeWindow;
    PRUint32     mChromeFlags;
    PRBool       mContinueModalLoop;
    PRBool       mSizeSet;

    nsCOMPtr<nsIWebBrowser> mWebBrowser;
    nsCOMPtr<nsIWebBrowserChrome> mDependentParent; // opener (for dependent windows only)
};

#endif /* __WebBrowserChrome__ */
