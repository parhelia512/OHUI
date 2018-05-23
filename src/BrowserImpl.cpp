
#include "nsIWebBrowser.h"
#include "nsIDOMWindow.h"
#include "nsIBaseWindow.h" // nativeWindow
#include "BrowserImpl.h"
#include "../include/common.h"   //WM_BROWSERBUILD_COMPLETE
#include "nsEmbedString.h"
#include "BrowserWrapper.h"
#include "nsIAppShell.h"
#include "nsComponentManagerUtils.h"
#include "nsIInterfaceRequestorUtils.h"  //do_GetInterface()

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "gtkmozembedmarshal.h"
#include "MozEmbedSignal.h"
#include "mozIDOMWindow.h"
 
//*****************************************************************************
// CBrowserImpl::nsISupports
//*****************************************************************************   
NS_IMPL_ADDREF(CBrowserImpl)
NS_IMPL_RELEASE(CBrowserImpl)

NS_INTERFACE_MAP_BEGIN(CBrowserImpl)
   NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIWebBrowserChrome)
   NS_INTERFACE_MAP_ENTRY(nsIInterfaceRequestor)
   NS_INTERFACE_MAP_ENTRY(nsIWebBrowserChrome)
   NS_INTERFACE_MAP_ENTRY(nsIEmbeddingSiteWindow)
   NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
   NS_INTERFACE_MAP_ENTRY(nsIWindowProvider)
   NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference)
   NS_INTERFACE_MAP_ENTRY(nsIDOMEventListener)
   NS_INTERFACE_MAP_ENTRY(nsIPromptService)
NS_INTERFACE_MAP_END


CBrowserImpl::CBrowserImpl(void)
{ 
	mWidget = 0;
	mIsModal = PR_FALSE;
	mChromeLoaded = PR_FALSE;
	mSizeSet = PR_FALSE;
	mChromeFlags = 0;
	mParent=0;
}


CBrowserImpl::~CBrowserImpl(void)
{ 
}

nsresult CBrowserImpl::Init(CBrowserWrapper* pWrapper, nsIWebBrowser* aWebBrowser)
{
	mWebBrowser = aWebBrowser; 
	mWrapper = pWrapper;
	return NS_OK;
}

void CBrowserImpl::UpdateWidget(nativeWindow h)
{
	mWidget = h;
}

nativeWindow CBrowserImpl::GetWidget()
{
	if(mWidget==0)
	{
		nsCOMPtr<nsIEmbeddingSiteWindow> baseWindow = do_QueryInterface((nsIWebBrowserChrome*)this);
		baseWindow->GetSiteWindow((void **) & mWidget);
	}
	return (nativeWindow)mWidget;
}

//*****************************************************************************
// CBrowserImpl::nsIInterfaceRequestor
//*****************************************************************************   
NS_IMETHODIMP CBrowserImpl::GetInterface(const nsIID &aIID, void** aInstancePtr)
{
	// Note that we're wrapping our nsIPrompt impl. with the 
	// nsISingleSignOnPrompt - if USE_SINGLE_SIGN_ON is defined
	// (and if single sign-on support dll's are present and enabled) 
	// This allows the embedding app to use the password save
	// feature. When signing on to a host which needs authentication
	// the Single sign-on service will check to see if the auth. info
	// is already saved and if so uses it to pre-fill the sign-on form
	// If not, our nsIPrompt impl will be called
	// to present the auth UI and return the required auth info.
	// If we do not compile with single sign-on support or if that
	// service is missing we just fall back to the regular 
	// implementation of nsIPrompt
	if(aIID.Equals(NS_GET_IID(nsIDOMWindow)))
	{
	  
		if (mWebBrowser)
		{		  
		    nsCOMPtr<mozIDOMWindowProxy> domWindowProxy;
		    mWebBrowser->GetContentDOMWindow(getter_AddRefs(domWindowProxy));
		    nsCOMPtr<nsIDOMWindow> contentWin = do_QueryInterface(domWindowProxy);
		    *aInstancePtr = contentWin; 
		    return NS_OK;
		}
			 
		return NS_ERROR_NOT_INITIALIZED;
	}
	nsresult rv = QueryInterface(aIID, aInstancePtr);

	return rv;
}
 
//*****************************************************************************
// CBrowserImpl::nsIWebBrowserChrome
//***************************************************************************** 
NS_IMETHODIMP CBrowserImpl::SetStatus(uint32_t/*PRUint32*/ aType, const char16_t* aStatus)
{
 
	return NS_OK;
}
/* attribute nsIWebBrowser webBrowser; */
NS_IMETHODIMP CBrowserImpl::GetWebBrowser(nsIWebBrowser** aWebBrowser)
{
	NS_ENSURE_ARG_POINTER(aWebBrowser);
	*aWebBrowser = mWebBrowser;
	NS_IF_ADDREF(*aWebBrowser);
	return NS_OK;
} 

NS_IMETHODIMP CBrowserImpl::SetWebBrowser(nsIWebBrowser* aWebBrowser)
{
	mWebBrowser = aWebBrowser;
	return NS_OK;
}

NS_IMETHODIMP CBrowserImpl::GetChromeFlags(PRUint32* aChromeMask)
{
	*aChromeMask = mChromeFlags;
	return NS_OK;
}

NS_IMETHODIMP CBrowserImpl::SetChromeFlags(PRUint32 aChromeMask)
{
	mChromeFlags = aChromeMask;
	return NS_OK;
}

// Will get called in response to JavaScript window.close()
NS_IMETHODIMP CBrowserImpl::DestroyBrowserWindow(void)
{

	return NS_OK;
} 

// Gets called in response to set the size of a window
// Ex: In response to a JavaScript Window.Open() call of
// the form 
//
//		window.open("http://www.mozilla.org", "theWin", "width=200, height=400");
//
// First the CreateBrowserWindow() call will be made followed by a 
// call to this method to resize the window
//
NS_IMETHODIMP CBrowserImpl::SizeBrowserTo(PRInt32 aCX, PRInt32 aCY)
{ 
	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;
#ifdef XP_WIN
	WINDOWPLACEMENT  wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(hWidget, &wndpl);
	if (wndpl.showCmd != SW_SHOWNORMAL)
		return NS_ERROR_FAILURE;

	::SetWindowPos(hWidget,NULL, 0, 0, aCX, aCY,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER
		);
#else
    gtk_window_set_default_size(GTK_WINDOW (hWidget),aCX, aCY);

#endif
	mSizeSet = PR_TRUE;

	return NS_OK;
}

void   CBrowserImpl::SetParent(nativeWindow aParent)
{
	mParent = aParent; 
}

NS_IMETHODIMP CBrowserImpl::ShowAsModal(void)
{
	mIsModal = PR_TRUE;
#ifdef XP_WIN
	if (mParent)
		::EnableWindow(mParent, PR_FALSE);

	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;
	nativeWindow hParent = ::GetParent(hWidget); 


	OHUI::CBaseDlg::DoModal(hWidget, hParent, 0);	
 
	if (mParent)
		::EnableWindow(mParent, PR_TRUE);    
#else
    GtkWidget *toplevel;
    toplevel = gtk_widget_get_toplevel(GTK_WIDGET(mWidget));
    gtk_grab_add(toplevel);
    gtk_main();
#endif
  
	mIsModal = PR_FALSE;
	return NS_OK;
}
 

NS_IMETHODIMP CBrowserImpl::IsWindowModal(/*PRBool*/bool *_retval)
{
	*_retval = mIsModal;
	return NS_OK;
}
 
NS_IMETHODIMP
CBrowserImpl::ExitModalEventLoop(nsresult aStatus)
{
	if (mIsModal) 
	{
#ifdef XP_WIN
		HWND  hWidget = (HWND)GetWidget();
		if(!hWidget) return NS_ERROR_FAILURE;
		::PostMessage(hWidget,WM_CLOSE, 0, 0);
#else
        GtkWidget *toplevel;
        toplevel = gtk_widget_get_toplevel(GTK_WIDGET(mWidget));
        gtk_grab_remove(toplevel);
        mIsModal = PR_FALSE;
        gtk_main_quit();
#endif        
	}
	return NS_OK;
}

//*****************************************************************************
// CBrowserImpl::nsIEmbeddingSiteWindow
//*****************************************************************************   
NS_IMETHODIMP CBrowserImpl::SetDimensions(PRUint32 aFlags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy)
{
	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;
#ifdef XP_WIN    
	UINT flags = SWP_NOACTIVATE | SWP_NOZORDER;

	if (!(aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_POSITION)) 
		flags |= SWP_NOMOVE;


	if ((aFlags & (nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_OUTER|nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_INNER)))
	{
		::SetWindowPos(hWidget, NULL, x, y, cx, cy, flags);
	}
	else
	{
		flags |= SWP_NOSIZE;
		::SetWindowPos(hWidget, NULL, x, y, 0, 0, flags);
	}
#else
    gtk_window_move(GTK_WINDOW(mWidget),    x,  y);
    gtk_window_resize(GTK_WINDOW(mWidget), cx, cy);    
#endif    
	return NS_OK;
}

NS_IMETHODIMP 
CBrowserImpl::GetDimensions(PRUint32 aFlags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy)
{ 
	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;
    
#ifdef XP_WIN
	RECT wndRect;    
	if (aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_POSITION)
	{
		::GetWindowRect(hWidget, &wndRect);
		*x = wndRect.left;
		*y = wndRect.top;
	}

	if (aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_INNER)
	{
		::GetClientRect(hWidget, &wndRect);
		*cx = wndRect.right;
		*cy = wndRect.bottom;
	}
	else if (aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_OUTER)
	{
		::GetWindowRect(hWidget, &wndRect);
		*cx = wndRect.right - wndRect.left;
		*cy = wndRect.bottom - wndRect.top;
	}
#else
    gint gt_x,gt_y,gt_cx,gt_cy;
    gtk_window_get_default_size(GTK_WINDOW(mWidget), &gt_cx, &gt_cy);
    gtk_window_get_position(GTK_WINDOW(mWidget), &gt_x, &gt_y);

    *x  = (PRInt32)gt_x;
    *y  = (PRInt32)gt_y;
    *cx = (PRInt32)gt_cx;
    *cy = (PRInt32)gt_cy;
#endif
	return NS_OK;
}

NS_IMETHODIMP CBrowserImpl::Blur()
{
	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;

	nsresult rv;

	nsCOMPtr<nsIWebBrowserFocus> focus(do_GetInterface(mWebBrowser));
	if(focus)
	{
	    rv = focus->Deactivate();
	}
	//printf("CBrowserImpl::Blur() \n");
	return rv;
}


NS_IMETHODIMP CBrowserImpl::SetFocus()
{
	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;

	nsresult rv;
	nsCOMPtr<nsIBaseWindow> baseWindow = do_QueryInterface(mWebBrowser, &rv);
	NS_ENSURE_SUCCESS(rv, rv);
 	rv = baseWindow->SetFocus();   
	
#ifdef XP_WIN
	::SetFocus(hWidget);    
#else
        gtk_widget_grab_focus((GtkWidget*)(hWidget));
	nsCOMPtr<nsIWebBrowserFocus> focus(do_GetInterface(mWebBrowser));
	if(focus)
	{
	    rv = focus->Activate();
	}
#endif    

        //printf("CBrowserImpl::SetFocus() \n");
	return rv;
}

NS_IMETHODIMP CBrowserImpl::GetVisibility(bool *aVisibility)
{
	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;
#ifdef XP_WIN
	*aVisibility = ::IsIconic(hWidget) || ::IsWindowVisible(hWidget);
#else
    *aVisibility = gtk_widget_get_visible((GtkWidget*)hWidget);

#endif 
	return NS_OK;
}

NS_IMETHODIMP CBrowserImpl::SetVisibility(bool aVisibility)
{
	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;

	if (!mChromeLoaded && (mChromeFlags & nsIWebBrowserChrome::CHROME_OPENAS_CHROME))
		return NS_OK; // Not yet, waiting for resize in UpdateBusyState
#ifdef XP_WIN
	if(aVisibility)
	{
	    if (::IsIconic(hWidget))
		return NS_ERROR_FAILURE;

	    ::ShowWindow(hWidget, SW_SHOW);
	    ::UpdateWindow(hWidget);
	}
	else
	{
	    ::ShowWindow(hWidget, SW_HIDE);
	} 
#else
      if ( aVisibility == PR_TRUE ) {
	      gtk_widget_show_all( (GtkWidget*)hWidget );
	} else
	      gtk_widget_hide( (GtkWidget*)hWidget );
#endif 
	return NS_OK;
}

NS_IMETHODIMP CBrowserImpl::GetTitle(char16_t * *aTitle)
{
	NS_ENSURE_ARG_POINTER(aTitle);
#ifdef XP_WIN
	*aTitle = L"";
#else

#endif 
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::SetTitle(const char16_t * aTitle)
{
	if(!aTitle) return NS_ERROR_FAILURE;

	nativeWindow  hWidget = GetWidget();
	if(!hWidget) return NS_ERROR_FAILURE;
#ifdef XP_WIN
#ifdef _UNICODE
	::SetWindowText(hWidget, aTitle);
#else
	nsCString status; 
	if (aTitle) 
	{
		nsString  wStatusText(aTitle); 
		NS_UTF16ToCString(wStatusText, NS_CSTRING_ENCODING_NATIVE_FILESYSTEM,
			status);
	}
	::SetWindowText(hWidget, status.get());
#endif
#else
	nsCString status; 
	if (aTitle) 
	{
		nsString  wStatusText(aTitle); 
		NS_UTF16ToCString(wStatusText, NS_CSTRING_ENCODING_NATIVE_FILESYSTEM,
			status);
	}
	gtk_window_set_title( (GtkWindow*)hWidget,  status.get() );
#endif
	return NS_OK;
}

NS_IMETHODIMP CBrowserImpl::GetSiteWindow(void * *aSiteWindow)
{
	NS_ENSURE_ARG_POINTER(aSiteWindow);

	*aSiteWindow = GetWidget();
	return NS_OK;
}


//*****************************************************************************
// CBrowserImpl::nsIWebProgressListener Implementation
//*****************************************************************************   
NS_IMETHODIMP CBrowserImpl::OnStateChange(nsIWebProgress *progress, nsIRequest *request,
	PRUint32 progressStateFlags, nsresult status)
{
	if ((progressStateFlags & STATE_START) && (progressStateFlags & STATE_IS_DOCUMENT))
	{
		;
	}

	if ((progressStateFlags & STATE_STOP) && (progressStateFlags & STATE_IS_DOCUMENT))
	{
		nativeWindow  hWidget = GetWidget();
		// if it was a chrome window and no one has already specified a size,
		// size to content
		if (mWebBrowser && !mSizeSet &&
			(mChromeFlags & nsIWebBrowserChrome::CHROME_OPENAS_CHROME)) 
		{
			nsCOMPtr<mozIDOMWindowProxy> domWindowProxy;
			mWebBrowser->GetContentDOMWindow(getter_AddRefs(domWindowProxy));

			nsCOMPtr<nsIDOMWindow> domWin = do_QueryInterface(domWindowProxy);
			if (domWin)
			{
				domWin->SizeToContent();
			} 
			 
				
			if(hWidget)
			{
			#ifdef XP_WIN
			    ::ShowWindow(hWidget, PR_TRUE);
			#else
			    gtk_widget_show((GtkWidget*)(hWidget));
			#endif
			}
		}

		if(hWidget)
		{
		    #ifdef XP_WIN
			::SendMessage(hWidget, WM_BROWSERBUILD_COMPLETE,0,0);
		    #else
  
			g_signal_emit_by_name (G_OBJECT(hWidget),
					       "dom_loadcompleted",
					       "This is the second param",
					       G_TYPE_NONE);
		    #endif
		}
	}

	return NS_OK;
}

NS_IMETHODIMP CBrowserImpl::OnProgressChange(nsIWebProgress *progress, nsIRequest *request,
	PRInt32 curSelfProgress, PRInt32 maxSelfProgress,
	PRInt32 curTotalProgress, PRInt32 maxTotalProgress)
{

	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP 
CBrowserImpl::OnLocationChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsIURI *aLocation, uint32_t aFlags)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP 
CBrowserImpl::OnStatusChange(nsIWebProgress* aWebProgress,
							nsIRequest* aRequest,
							nsresult aStatus,
							const char16_t* aMessage)
{ 
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP 
CBrowserImpl::OnSecurityChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, uint32_t aState)
{ 
	return NS_ERROR_NOT_IMPLEMENTED;
}


//*****************************************************************************
// CBrowserImpl::nsIDOMEventListener
//***************************************************************************** 

NS_IMETHODIMP CBrowserImpl::HandleEvent(nsIDOMEvent *event)
{
	nsresult rv;
	nsEmbedString type;
	event->GetType(type);


	return NS_ERROR_NOT_IMPLEMENTED;
}


//*****************************************************************************
// CBrowserImpl::nsIWindowProvider
//***************************************************************************** 
NS_IMETHODIMP CBrowserImpl::ProvideWindow(mozIDOMWindowProxy *aParent
	, uint32_t aChromeFlags
	, bool aCalledFromJS
	, bool aPositionSpecified
	, bool aSizeSpecified
	, nsIURI *aURI
	, const nsAString & aName
	, const nsACString & aFeatures
	, bool aForceNoOpener
	, bool *aWindowIsNew
	, mozIDOMWindowProxy * *_retval)
{
	NS_ENSURE_ARG_POINTER(aParent);
	*_retval = 0; 
 
	CBrowserWrapper* window = mWrapper; 
	if (!window) return NS_OK;

	nsCOMPtr<nsIWebBrowser> browser = window->GetWebBrowser();
	NS_ENSURE_TRUE(browser, NS_OK);

	nsCOMPtr<mozIDOMWindowProxy> domWindow;
	browser->GetContentDOMWindow(getter_AddRefs(domWindow));
	NS_ENSURE_TRUE(domWindow, NS_OK);
	
	nsCOMPtr<mozIDOMWindowProxy> currentWindow;
	mWebBrowser->GetContentDOMWindow(getter_AddRefs(currentWindow));
	NS_ENSURE_TRUE(currentWindow, NS_OK);
	
	*aWindowIsNew = (currentWindow != domWindow);

	NS_ADDREF(*_retval = domWindow);
	return NS_OK;
}


//*****************************************************************************
// CBrowserImpl::nsIWebBrowserChromeFocus
//*****************************************************************************

NS_IMETHODIMP CBrowserImpl::FocusNextElement(bool aForDocumentNavigation)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::FocusPrevElement(bool aForDocumentNavigation)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

//*****************************************************************************
// CBrowserImpl::nsIPromptService
//*****************************************************************************   
NS_IMETHODIMP CBrowserImpl::Alert(mozIDOMWindowProxy *aParent, const char16_t * aDialogTitle, const char16_t  * aText)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::AlertCheck(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, const char16_t  * aCheckMsg, bool *aCheckState)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::Confirm(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, bool *_retval)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::ConfirmCheck(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, const char16_t  * aCheckMsg, bool *aCheckState, bool *_retval)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::ConfirmEx(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, uint32_t aButtonFlags, const char16_t  * aButton0Title, const char16_t  * aButton1Title, const char16_t  * aButton2Title, const char16_t  * aCheckMsg, bool *aCheckState, int32_t *_retval)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::Prompt(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, char16_t * *aValue, const char16_t * aCheckMsg, bool *aCheckState, bool *_retval)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::PromptUsernameAndPassword(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, char16_t  * *aUsername, char16_t  * *aPassword, const char16_t  * aCheckMsg, bool *aCheckState, bool *_retval)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}


NS_IMETHODIMP CBrowserImpl::PromptPassword(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, char16_t  * *aPassword, const char16_t  * aCheckMsg, bool *aCheckState, bool *_retval)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CBrowserImpl::Select(mozIDOMWindowProxy *aParent, const char16_t  * aDialogTitle, const char16_t  * aText, uint32_t aCount, const char16_t  * *aSelectList, int32_t *aOutSelection, bool *_retval)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}
