
#include "stdafx.h"
#include "nsIWindowWatcher.h"
#include "nsIWebBrowserChrome.h"
#include "nsServiceManagerUtils.h"  //do_GetService
#include "MozUtil.h"   

#include "MainFrame.h"
#include "OhuiApp.h"
#include "nsISupportsImpl.h"
NS_IMPL_ADDREF(COhuiApp)
NS_IMPL_RELEASE(COhuiApp)

NS_INTERFACE_MAP_BEGIN(COhuiApp) 
NS_INTERFACE_MAP_ENTRY(nsIObserver)
NS_INTERFACE_MAP_ENTRY(nsIWindowCreator)
NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference) 
NS_INTERFACE_MAP_END

COhuiApp::COhuiApp(void)
{
}


COhuiApp::~COhuiApp(void)
{
}

PRBool  COhuiApp::InitInstance()
{
	//OverrideComponents();
	InitializeWindowCreator();
	return 0;
}

int  COhuiApp::ExitInstance()
{


	return 0;
}

/* InitializeWindowCreator creates and hands off an object with a callback
to a window creation function. This will be used by Gecko C++ code
(never JS) to create new windows when no previous window is handy
to begin with. This is done in a few exceptional cases, like PSM code.
Failure to set this callback will only disable the ability to create
new windows under these circumstances.
*/
nsresult COhuiApp::InitializeWindowCreator()
{
	nsCOMPtr<nsIWindowCreator> windowCreator(static_cast<nsIWindowCreator *>(this));
	if (windowCreator) 
	{
		nsCOMPtr<nsIWindowWatcher> wwatch(do_GetService("@mozilla.org/embedcomp/window-watcher;1"));
		if (wwatch) 
		{
			wwatch->SetWindowCreator(windowCreator);
			return NS_OK;
		}
	}
	return NS_ERROR_FAILURE;
}

NS_IMETHODIMP COhuiApp::Observe(nsISupports *aSubject, const char *aTopic, const char16_t *someData)
{
	nsresult rv = NS_ERROR_NOT_IMPLEMENTED;
	return rv;
}

NS_IMETHODIMP
COhuiApp::CreateChromeWindow(nsIWebBrowserChrome *parent,
uint32_t/*PRUint32*/ chromeMask,
nsIWebBrowserChrome **_retval)
{
	NS_ENSURE_ARG_POINTER(_retval);
	CMainFrame* pMain = new CMainFrame(); 
	

	nativeWindow hParent = CMozUtil::GetHwndFromChrome(parent);
	nativeWindow hWnd = 0;//
#ifdef XP_WIN
	LONG style, styleEx = 0L;
	if (chromeMask & nsIWebBrowserChrome::CHROME_WINDOW_POPUP) {
		style = WS_POPUPWINDOW | WS_CAPTION;
		styleEx = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	}
	else if (chromeMask & nsIWebBrowserChrome::CHROME_OPENAS_DIALOG) {
		style =  WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_DLGFRAME;
		//| DS_3DLOOK | DS_MODALFRAME;
		styleEx = WS_EX_DLGMODALFRAME;
	}
	else {
		style = WS_OVERLAPPEDWINDOW;
		if (hParent && (chromeMask & nsIWebBrowserChrome::CHROME_DEPENDENT))
			style |= WS_POPUP;
	}

	if ( !(chromeMask & nsIWebBrowserChrome::CHROME_DEFAULT) &&
		((chromeMask & nsIWebBrowserChrome::CHROME_ALL) != nsIWebBrowserChrome::CHROME_ALL)) {

			if( !(chromeMask & nsIWebBrowserChrome::CHROME_TITLEBAR) )
				style &= ~WS_CAPTION; // No caption      

		
				if( !(chromeMask & nsIWebBrowserChrome::CHROME_WINDOW_RESIZE) ) {
					// Can't resize this window
					style &= ~WS_THICKFRAME;
					style &= ~WS_MAXIMIZEBOX;
				}

			if ( !(chromeMask & nsIWebBrowserChrome::CHROME_WINDOW_CLOSE) &&
				!(chromeMask & nsIWebBrowserChrome::CHROME_MENUBAR) )
				style &= ~WS_SYSMENU;

			if (chromeMask & nsIWebBrowserChrome::CHROME_WINDOW_MIN)  
				style |= WS_MINIMIZEBOX;
	}

	if ( (chromeMask & (nsIWebBrowserChrome::CHROME_OPENAS_CHROME)) ||
		(!(chromeMask & nsIWebBrowserChrome::CHROME_DEFAULT) &&
		!(chromeMask & nsIWebBrowserChrome::CHROME_TOOLBAR)) ) {
			 
			style |= WS_POPUP; // XXX
	}

	if (
		((chromeMask & nsIWebBrowserChrome::CHROME_WINDOW_RESIZE) ||
		(chromeMask & nsIWebBrowserChrome::CHROME_ALL)))
		style |= WS_MAXIMIZE;
	
	hWnd = pMain->Create((HWND)hParent, 0, ghInstanceApp, /*WS_VISIBLE|*/style);
#else
	hWnd = pMain->CreateWindow();
#endif
	
	pMain->m_pBrowserWrapper->CreateBrowser(hWnd); 
	pMain->m_pBrowserWrapper->mpBrowserImpl->SetChromeFlags(chromeMask); 
 

	*_retval = static_cast<nsIWebBrowserChrome *>(pMain->m_pBrowserWrapper->mpBrowserImpl);

	return *_retval ? NS_OK : NS_ERROR_FAILURE;
}
