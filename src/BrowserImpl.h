#pragma once


#include "stdafx.h"
#include "nsCOMPtr.h"
#include "nsIInterfaceRequestor.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserChromeFocus.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIWebProgressListener.h"
#include "nsIContextMenuListener2.h"
#include "nsITooltipListener.h"
#include "nsWeakReference.h"
#include "nsIWindowProvider.h"
#include "nsIDOMEventListener.h"
#include "nsIDOMEvent.h"
#include "nsIPromptService.h" 
 
#ifdef XP_WIN
#include <windows.h>
#else
#include "MozEmbedSignal.h"

#endif

class CBrowserImpl: public nsIInterfaceRequestor
					,public nsIWebBrowserChrome
					,public nsIWebBrowserChromeFocus
					,public nsIEmbeddingSiteWindow
					,public nsIWebProgressListener
					,public nsSupportsWeakReference 
					,public nsIWindowProvider
					,public nsIDOMEventListener 
					,public nsIPromptService
{
 	~CBrowserImpl(void); 
public:
	CBrowserImpl(void);


	NS_DECL_ISUPPORTS
	NS_DECL_NSIINTERFACEREQUESTOR
	NS_DECL_NSIWEBBROWSERCHROME
	NS_DECL_NSIWEBBROWSERCHROMEFOCUS
	NS_DECL_NSIEMBEDDINGSITEWINDOW
	NS_DECL_NSIWEBPROGRESSLISTENER
	NS_DECL_NSIWINDOWPROVIDER
	NS_DECL_NSIDOMEVENTLISTENER
	NS_DECL_NSIPROMPTSERVICE

	NS_METHOD Init(class CBrowserWrapper*, nsIWebBrowser* aWebBrowser);

	void UpdateWidget(nativeWindow h);
	nativeWindow GetWidget();

	void     SetParent(nativeWindow aParent);
protected:
	nsCOMPtr<nsIWebBrowser> mWebBrowser;
	nativeWindow mParent;  // opener (for dependent windows only)
	class CBrowserWrapper* mWrapper;
	nativeWindow         mWidget;
	PRBool       mIsModal;
	PRBool       mChromeLoaded;
	PRBool       mSizeSet;
    PRUint32     mChromeFlags;
};

