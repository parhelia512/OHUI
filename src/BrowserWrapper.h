#pragma once

#include "nsCOMPtr.h"
#include "nsIWebBrowser.h"
#include "nsIBaseWindow.h"
#include "nsIWebNavigation.h"
#include "nsIWebBrowserFocus.h"
#include "nsIDOMEventTarget.h"
#include "nsIPrintSettings.h"

#include "BrowserImpl.h"

#ifdef XP_WIN
#include <windows.h>
#else

#endif


class CBrowserWrapper
{
private:
	nsCOMPtr<nsIBaseWindow> mBaseWindow;
	nsCOMPtr<nsIWebNavigation> mWebNav;
	nsCOMPtr<nsIWebBrowserFocus> mWebBrowserFocus;
	nsCOMPtr<nsIDOMEventTarget> mEventTarget;
	nsCOMPtr<nsIPrintSettings> mPrintSettings;

public:
	nsCOMPtr<nsIWebBrowser> mWebBrowser;
	CBrowserWrapper(void);
	~CBrowserWrapper(void);
	PRBool InitPrintSettings();
	PRBool Print();
	PRBool IsPrintPreview();
	PRBool PrintPreview(nsIPrintSettings* printSettings=0);
	PRBool LoadURL(const char* url, bool allowFixup=false);

	void SetActive(bool aActive)
	{
		NS_ENSURE_TRUE(mWebBrowserFocus, );
		if (aActive) mWebBrowserFocus->Activate(); else mWebBrowserFocus->Deactivate();
	}

	void FocusFirstElement() 
	{
		NS_ENSURE_TRUE(mWebBrowserFocus, );
		mWebBrowserFocus->Activate();
		mWebBrowserFocus->SetFocusAtFirstElement();
	}

	void FocusLastElement() 
	{
		NS_ENSURE_TRUE(mWebBrowserFocus, );
		mWebBrowserFocus->Activate();
		mWebBrowserFocus->SetFocusAtLastElement();
	}
	
	already_AddRefed<nsIWebBrowser> GetWebBrowser()
	{
		nsIWebBrowser* browser;
		NS_IF_ADDREF(browser = mWebBrowser);
		return already_AddRefed<nsIWebBrowser>(browser);//.forget();
	}
	

	PRBool CreateBrowser(nativeWindow hNativeWindow, PRBool chromeContent=PR_FALSE );
	PRBool DestroyBrowser();

	PRBool AddListeners(void);
	void RemoveListeners(void);

	CBrowserImpl* mpBrowserImpl;
	nativeWindow mNativeWindow;
	
	std::string mTest;
};

