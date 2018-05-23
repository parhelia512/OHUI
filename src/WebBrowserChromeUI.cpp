#include "stdafx.h"
#include "nsIFile.h"
#include "nsIDirectoryService.h"
#include "nsIWindowWatcher.h"

#include "WebBrowserChromeUI.h"
#include "nsINetUtil.h"
#include "nsNetCID.h" //NS_IOSERVICE_CONTRACTID
#include "nsServiceManagerUtils.h" //do_GetService

#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>

static already_AddRefed<nsIFile>
	CloneAndAppend(nsIFile* aFile, const char* name)
{
	nsCOMPtr<nsIFile> file;
	aFile->Clone(getter_AddRefs(file));
	file->AppendNative(nsDependentCString(name));
	return file.forget();
}

already_AddRefed<nsIFile>
WebBrowserChromeUI::GetRegDirectory(const char* basename, const char* dirname, const char* leafname)
{
	nsCOMPtr<nsIFile> f;
	nsresult rv = NS_NewNativeLocalFile(nsDependentCString(basename), true,
		getter_AddRefs(f));
	if (NS_FAILED(rv))
		return NULL;

	f->AppendNative(nsDependentCString(dirname));
	if (leafname)
		f->AppendNative(nsDependentCString(leafname));
	return f.forget();
}

nsCString WebBrowserChromeUI::escape(const  char* p)
{
	nsresult rv;
 	nsCString aCharset;
	nsCOMPtr<nsINetUtil> netUtil = do_GetService(NS_IOSERVICE_CONTRACTID, &rv);
	if (!NS_SUCCEEDED(rv)) return aCharset;
    
	int nLen = strlen(p);
	char *tmp= new char[nLen+1];
	strcpy(tmp, p);

	nsCString ac;
	ac.AssignASCII((tmp));
	delete[] tmp;

	//解决中文路径问题
	netUtil->EscapeString(ac, nsINetUtil::ESCAPE_URL_PATH, aCharset);
    
	//const char* pp= aCharset.BeginReading();
	return aCharset;
}
///////////////////////////////////////////////////////////////////////////////
// WebBrowserChromeUI

//
//  FUNCTION: CreateNativeWindow()
//
//  PURPOSE: Creates a new browser dialog.
//
//  COMMENTS:
//
//    This function loads the browser dialog from a resource template
//    and returns the HWND for the webbrowser container dialog item
//    to the caller.
//
nsIWidget* WebBrowserChromeUI::CreateNativeWindow(nsIWebBrowserChrome* chrome)
{
	// Load the browser dialog from resource
	nsIWidget* hwndDialog; 
	return 0/*hwndBrowser*/;
}

//
// FUNCTION: Destroy()
//
// PURPOSE: Destroy the window specified by the chrome
//
void WebBrowserChromeUI::Destroy(nsIWebBrowserChrome* chrome)
{
	nsCOMPtr<nsIWebBrowser> webBrowser;
	nsCOMPtr<nsIWebNavigation> webNavigation;

	chrome->GetWebBrowser(getter_AddRefs(webBrowser));
	if(!webBrowser) return;

	webNavigation = do_QueryInterface(webBrowser);
	if (webNavigation)
		webNavigation->Stop(nsIWebNavigation::STOP_ALL);

	chrome->ExitModalEventLoop(NS_OK);

	nsIWidget* hwndDlg = WebBrowserChromeUI::GetBrowserDlgFromChrome(chrome);
	if (hwndDlg == NULL)
		return;

	// Explicitly destroy the embedded browser and then the chrome

	// First the browser
	nsCOMPtr<nsIWebBrowser> browser = nsnull;
	chrome->GetWebBrowser(getter_AddRefs(browser));
	nsCOMPtr<nsIBaseWindow> browserAsWin = do_QueryInterface(browser);
	if (browserAsWin)
		browserAsWin->Destroy();

	// Now the chrome
	chrome->SetWebBrowser(nsnull);
	NS_RELEASE(chrome);
}


//
// FUNCTION: Called as the final act of a chrome object during its destructor
//
void WebBrowserChromeUI::Destroyed(nsIWebBrowserChrome* chrome)
{

}
 

//
//  FUNCTION: GetBrowserFromChrome()
//
//  PURPOSE: Returns the HWND for the webbrowser container associated
//           with the specified chrome.
//
/*HWND*/nsIWidget*  WebBrowserChromeUI::GetBrowserFromChrome(nsIWebBrowserChrome *aChrome)
{
	if (!aChrome)
	{
		return NULL;
	}
  
	nsCOMPtr<nsIEmbeddingSiteWindow> baseWindow = do_QueryInterface(aChrome);
	if (!baseWindow)
	{
		return NULL;
	}

    nsIWidget* hwnd=0;
    baseWindow->GetSiteWindow((void**)&hwnd);

    //printf(" WebBrowserChromeUI::GetBrowserFromChrome %p\n", (void*)hwnd);    
	return hwnd;
}

//  FUNCTION: GetBrowserDlgFromChrome()
//
//  PURPOSE: Returns the HWND for the browser dialog associated with
//           the specified chrome.
//
nsIWidget* WebBrowserChromeUI::GetBrowserDlgFromChrome(nsIWebBrowserChrome *aChrome)
{
    GtkWidget*  h = (GtkWidget*)WebBrowserChromeUI::GetBrowserFromChrome(aChrome);
    if(!h) return NULL;
    
	GtkWidget* hTop = h;
#ifdef XP_WIN	
    while (hTop = GetParent(h))
	{
		h = hTop;
	}  
#else
    while (h)
    {
        hTop  = h; 
        h = gtk_widget_get_parent(h);
    }    
#endif
    //printf("GetBrowserDlgFromChrome: %p \n",(void*)hTop);
	return (nsIWidget*)hTop;
}


//
// FUNCTION: Set the input focus onto the browser window
//
void WebBrowserChromeUI::SetFocus(nsIWebBrowserChrome *chrome)
{
	nsIWidget* hwndDlg = GetBrowserDlgFromChrome(chrome);
	if (hwndDlg == NULL)
	{
		return;
	}
#ifdef XP_WIN
	HWND hwndBrowser = GetDlgItem(hwndDlg, IDC_BROWSER);
	::SetFocus(hwndDlg);
#else

#endif
}

//
//  FUNCTION: UpdateStatusBarText()
//
//  PURPOSE: Set the status bar text.
//
void WebBrowserChromeUI::UpdateStatusBarText(nsIWebBrowserChrome *aChrome, const PRUnichar* aStatusText)
{
	nsIWidget* hwndDlg = GetBrowserDlgFromChrome(aChrome);
	nsCString status; 
	if (aStatusText) {
		nsString  wStatusText(aStatusText); 
		NS_UTF16ToCString(wStatusText, NS_CSTRING_ENCODING_NATIVE_FILESYSTEM,
			status);
	} 
}

 
//
//  FUNCTION: UpdateCurrentURI()
//
//  PURPOSE: Updates the URL address field
//
void WebBrowserChromeUI::UpdateCurrentURI(nsIWebBrowserChrome *aChrome)
{
	nsCOMPtr<nsIWebBrowser> webBrowser;
	nsCOMPtr<nsIWebNavigation> webNavigation;
	aChrome->GetWebBrowser(getter_AddRefs(webBrowser));
	webNavigation = do_QueryInterface(webBrowser);

	nsCOMPtr<nsIURI> currentURI;
	webNavigation->GetCurrentURI(getter_AddRefs(currentURI));
	if (currentURI)
	{ 
#ifdef XP_WIN        
		HWND hwndDlg = GetBrowserDlgFromChrome(aChrome);
		SetDlgItemText(hwndDlg, IDC_ADDRESS, uriString.get());
#else

#endif
	}//if
}


//
//  FUNCTION: UpdateBusyState()
//
//  PURPOSE: Refreshes the stop/go buttons in the browser dialog
//
void WebBrowserChromeUI::UpdateBusyState(nsIWebBrowserChrome *aChrome, PRBool aBusy)
{ 
}


//
//  FUNCTION: UpdateProgress()
//
//  PURPOSE: Refreshes the progress bar in the browser dialog
//
void WebBrowserChromeUI::UpdateProgress(nsIWebBrowserChrome *aChrome, PRInt32 aCurrent, PRInt32 aMax)
{ 
}

//
//  FUNCTION: ShowContextMenu()
//
//  PURPOSE: Display a context menu for the given node
//
void WebBrowserChromeUI::ShowContextMenu(nsIWebBrowserChrome *aChrome, PRUint32 aContextFlags, nsIDOMEvent *aEvent, nsIDOMNode *aNode)
{
	// TODO code to test context flags and display a popup menu should go here
}

//
//  FUNCTION: ShowTooltip()
//
//  PURPOSE: Show a tooltip
//
void WebBrowserChromeUI::ShowTooltip(nsIWebBrowserChrome *aChrome, PRInt32 aXCoords, PRInt32 aYCoords, const PRUnichar *aTipText)
{
	// TODO code to show a tooltip should go here
}

//
//  FUNCTION: HideTooltip()
//
//  PURPOSE: Hide the tooltip
//
void WebBrowserChromeUI::HideTooltip(nsIWebBrowserChrome *aChrome)
{
	// TODO code to hide a tooltip should go here
}

void WebBrowserChromeUI::ShowWindow(nsIWebBrowserChrome *aChrome, PRBool aShow)
{
    printf("WebBrowserChromeUI::ShowWindow \n");
	nsIWidget* win = GetBrowserDlgFromChrome(aChrome);
	//::ShowWindow(win, aShow ? SW_RESTORE : SW_HIDE);
}

void WebBrowserChromeUI::SizeTo(nsIWebBrowserChrome *aChrome, PRInt32 aWidth, PRInt32 aHeight)
{
	nsIWidget* hchrome  = GetBrowserDlgFromChrome(aChrome);
	nsIWidget* hbrowser = GetBrowserFromChrome(aChrome);
#ifdef XP_WIN
	//HWND hchrome  = GetBrowserDlgFromChrome(aChrome);
	//HWND hbrowser = GetBrowserFromChrome(aChrome);
	RECT chromeRect, browserRect;

	::GetWindowRect(hchrome,  &chromeRect);
	::GetWindowRect(hbrowser, &browserRect);

	PRInt32 decoration_x = (browserRect.left - chromeRect.left) + 
		(chromeRect.right - browserRect.right);
	PRInt32 decoration_y = (browserRect.top - chromeRect.top) + 
		(chromeRect.bottom - browserRect.bottom);

	::MoveWindow(hchrome, chromeRect.left, chromeRect.top,
    		aWidth+decoration_x,
    		aHeight+decoration_y, TRUE);
#else
    nsCOMPtr<nsIWebBrowser> webBrowser;
    aChrome->GetWebBrowser(getter_AddRefs(webBrowser));    
    nsCOMPtr<nsIBaseWindow> webBrowserAsWin = do_QueryInterface(webBrowser);
     printf("WebBrowserChromeUI::SizeTo haha \n");
    if(webBrowserAsWin)
    {
        gint w=0;
        gint h=0;
        gtk_window_get_size((GtkWindow*)hbrowser,&w, &h);
       
        webBrowserAsWin->SetPositionAndSize(0, 0, w, h, PR_TRUE);
        webBrowserAsWin->SetVisibility(PR_TRUE);
    }//if(webBrowserAsWin)
#endif
}

//
//  FUNCTION: GetResourceStringByID()
//
//  PURPOSE: Get the resource string for the ID
//
void WebBrowserChromeUI::GetResourceStringById(PRInt32 aID, char ** aReturn)
{
 
	return;
}

nsresult WebBrowserChromeUI::GetIOService(nsIIOService **ioService)
{
	nsresult rv;

	nsCOMPtr<nsIServiceManager> mgr; 
	NS_GetServiceManager (getter_AddRefs (mgr));
	if (!mgr) return NS_ERROR_FAILURE;

	rv = mgr->GetServiceByContractID ("@mozilla.org/network/io-service;1",
		NS_GET_IID (nsIIOService),
		(void **)ioService);
	return rv;
}


nsresult WebBrowserChromeUI::NewURI (nsIURI **result,
	const nsAString &spec,
	const char *charset,
	nsIURI *baseURI)
{
	nsCString/*nsEmbedCString*/ cSpec;
	NS_UTF16ToCString (spec, NS_CSTRING_ENCODING_UTF8, cSpec);

	return NewURI (result, cSpec, charset, baseURI);
}

nsresult WebBrowserChromeUI::NewURI (nsIURI **result,
	const nsACString &spec,
	const char *charset,
	nsIURI *baseURI)
{
	nsresult rv;

	nsCOMPtr<nsIIOService> ioService;
	rv = WebBrowserChromeUI::GetIOService (getter_AddRefs (ioService));
	NS_ENSURE_SUCCESS (rv, rv);

	rv = ioService->NewURI (spec, charset, baseURI, result);

	return rv;
}
