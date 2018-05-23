
#include "nsCOMPtr.h"
#include "nsEmbedCID.h" //NS_WEBBROWSER_CONTRACTID
#include "nsComponentManagerUtils.h"  //do_CreateInstance
#include "nsServiceManagerUtils.h" //do_GetService
#include "nsIInterfaceRequestorUtils.h"

#include "nsIDocShellTreeItem.h"
#include "MozUtil.h"
#include "BrowserWrapper.h"
#include "nsStringAPI.h"
#include "nsIPrintSettingsService.h"
#include "nsIWebBrowserPrint.h"
#include "nsIDOMWindow.h"
#include "nsIWebBrowserSetup.h"
#include "nsIBaseWindow.h"
#include "nsWidgetInitData.h"
#include "nsIWidget.h"


CBrowserWrapper::CBrowserWrapper(void)
{
	mNativeWindow=0;
	mWebBrowser=(nsIWebBrowser*)nullptr;
	mBaseWindow=(nsIBaseWindow*)nullptr;
	mWebNav=(nsIWebNavigation*)nullptr;
	mWebBrowserFocus=(nsIWebBrowserFocus*)nullptr;
	mEventTarget=(nsIDOMEventTarget*)nullptr;
	mPrintSettings=(nsIPrintSettings*)nullptr; 
	mTest="asdfsdf CBrowserWrapper";
}


CBrowserWrapper::~CBrowserWrapper(void)
{
}

PRBool CBrowserWrapper::CreateBrowser(nativeWindow h, PRBool chromeContent)
{
	PRBool bRtn = PR_TRUE;
	mNativeWindow = h;
	// Create web shell
	nsresult rv;
	mWebBrowser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID, &rv);
	NS_ENSURE_SUCCESS(rv, FALSE);

	mWebNav = do_QueryInterface(mWebBrowser, &rv);
	NS_ENSURE_SUCCESS(rv, FALSE);

	mWebBrowserFocus = do_QueryInterface(mWebBrowser, &rv);
	NS_ENSURE_SUCCESS(rv, FALSE);

	mpBrowserImpl = new CBrowserImpl();
	NS_ENSURE_TRUE(mpBrowserImpl, FALSE);

	mpBrowserImpl->Init(this, mWebBrowser);
	mpBrowserImpl->AddRef();
	mpBrowserImpl->UpdateWidget(h);

	nativeWindow hParent;
#ifdef XP_WIN
	hParent = ::GetParent(h);
#else
	hParent = gtk_widget_get_parent((GtkWidget*)h);
#endif
	mpBrowserImpl->SetParent(hParent);

	mWebBrowser->SetContainerWindow(static_cast<nsIWebBrowserChrome*>(mpBrowserImpl));
	nsCOMPtr<nsIWebBrowserSetup>setup(do_QueryInterface(mWebBrowser));  
	if (setup) 
		setup->SetProperty(nsIWebBrowserSetup::SETUP_IS_CHROME_WRAPPER,PR_TRUE);

	nsCOMPtr<nsIDocShellTreeItem> dsti = do_QueryInterface(mWebBrowser, &rv);
	NS_ENSURE_SUCCESS(rv, FALSE);


	dsti->SetItemType( chromeContent ? 
		nsIDocShellTreeItem::typeChromeWrapper :
		nsIDocShellTreeItem::typeContentWrapper);


	mBaseWindow = do_QueryInterface(mWebBrowser, &rv);
	if (NS_FAILED(rv)) {
		DestroyBrowser();
		return FALSE;
	}

	PRInt32 aX=0,aY=0,CX=0,CY=0;
#ifdef XP_WIN
	RECT rect;
	::GetClientRect(h, &rect);
	if(::IsRectEmpty(&rect)) 
	{
		rect.bottom++;
		rect.top++;
	}
	CX = rect.right-rect.left;
	CY = rect.bottom-rect.top;
	::GetWindowRect(h,&rect);
	aX = rect.left;
	aY = rect.top;
#else
	mpBrowserImpl->GetDimensions(nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_OUTER, &aX, &aY, &CX, &CY);

#endif
	rv = mBaseWindow->InitWindow(nativeWindow(mNativeWindow), (nsIWidget*)0, aX, aY, CX, CY);
	rv = mBaseWindow->Create();
 
	// Register the BrowserImpl object to receive progress messages
	// These callbacks will be used to update the status/progress bars
	nsCOMPtr<nsIWeakReference> weak = do_GetWeakReference(static_cast<nsIWebProgressListener*>(mpBrowserImpl));
	mWebBrowser->AddWebBrowserListener(weak, NS_GET_IID(nsIWebProgressListener));
 
	// Add listeners for various events like popup-blocking, link added ... 
	if (!chromeContent) AddListeners();

	// Place it where we want it.
	CMozUtil::ResizeEmbedding(static_cast<nsIWebBrowserChrome*>(mpBrowserImpl));
#ifdef XP_WIN
	// Activate the window
	PostMessage(mNativeWindow, WM_ACTIVATE, WA_ACTIVE, 0);
#else
	gtk_widget_activate((GtkWidget*)mNativeWindow);
#endif
	// Finally, show the web browser window
	mBaseWindow->SetVisibility(PR_TRUE);
	return bRtn;
}

PRBool CBrowserWrapper::DestroyBrowser()
{

	return PR_TRUE;
}


PRBool CBrowserWrapper::AddListeners(void)
{
	nsresult rv;
	rv = CMozUtil::GetDOMEventTarget(mWebBrowser, (getter_AddRefs(mEventTarget)));
	NS_ENSURE_SUCCESS(rv, FALSE);

	mWebNav = do_QueryInterface(mWebBrowser);
	NS_ENSURE_TRUE(mWebNav, FALSE);
    
	mWebBrowserFocus = do_QueryInterface(mWebBrowser);
	NS_ENSURE_TRUE(mWebBrowserFocus, FALSE);

	nsString nsS;
	nsS.AssignLiteral("click");
	rv = mEventTarget->AddEventListener(/*NS_LITERAL_STRING*/nsS,mpBrowserImpl, PR_TRUE);

	rv = mEventTarget->AddEventListener(NS_LITERAL_STRING("mousedown"),
		mpBrowserImpl, PR_TRUE);
	rv = mEventTarget->AddEventListener(NS_LITERAL_STRING("DOMPopupBlocked"),
		mpBrowserImpl, PR_FALSE);
	rv = mEventTarget->AddEventListener(NS_LITERAL_STRING("DOMLinkAdded"),
		mpBrowserImpl, PR_FALSE);
	rv = mEventTarget->AddEventListener(NS_LITERAL_STRING("DOMContentLoaded"),
		mpBrowserImpl, PR_FALSE);

	return PR_TRUE;
}

void CBrowserWrapper::RemoveListeners(void)
{
	mEventTarget->RemoveEventListener(NS_LITERAL_STRING("click"),
		mpBrowserImpl, PR_FALSE);
	mEventTarget->RemoveEventListener(NS_LITERAL_STRING("mousedown"),
		mpBrowserImpl, PR_TRUE);
	mEventTarget->RemoveEventListener(NS_LITERAL_STRING("DOMPopupBlocked"),
		mpBrowserImpl, PR_FALSE);
	mEventTarget->RemoveEventListener(NS_LITERAL_STRING("DOMLinkAdded"),
		mpBrowserImpl, PR_FALSE);
	mEventTarget->RemoveEventListener(NS_LITERAL_STRING("DOMContentLoaded"),
		mpBrowserImpl, PR_FALSE);
}

PRBool  CBrowserWrapper::InitPrintSettings()
{
	nsCOMPtr<nsIPrintSettingsService> psService =
		do_GetService("@mozilla.org/gfx/printsettings-service;1");
	NS_ENSURE_TRUE(psService, FALSE);

	psService->GetGlobalPrintSettings(getter_AddRefs(mPrintSettings));
	NS_ENSURE_TRUE(mPrintSettings, FALSE);

	nsresult rv = psService->InitPrintSettingsFromPrefs(mPrintSettings, PR_FALSE, nsIPrintSettings::kInitSaveAll);
	return NS_SUCCEEDED(rv);
}

PRBool CBrowserWrapper::Print()
{
	NS_ENSURE_TRUE(mWebBrowser, FALSE);

	nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface(mWebBrowser));
	NS_ENSURE_TRUE(print, FALSE);

	//if (!mPrintSettings) 
		InitPrintSettings();
 
	nsresult rv = print->Print(mPrintSettings, nullptr);
	NS_ENSURE_SUCCESS(rv, FALSE);
 
	return TRUE;
}

PRBool  CBrowserWrapper::IsPrintPreview()
{
	nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface(mWebBrowser));
	NS_ENSURE_TRUE(print, FALSE);

	bool isPreview = PR_FALSE;
	nsresult rv = print->GetDoingPrintPreview(&isPreview);
	NS_ENSURE_SUCCESS(rv, FALSE);

	return isPreview;
}

PRBool  CBrowserWrapper::PrintPreview(nsIPrintSettings* printSettings)
{
	nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface(mWebBrowser));
	NS_ENSURE_TRUE(print, FALSE);

	if (!mPrintSettings) InitPrintSettings();

	if(printSettings)
		printSettings->Clone(getter_AddRefs(mPrintSettings));

	bool isPreview = PR_FALSE;
	nsresult rv = print->GetDoingPrintPreview(&isPreview);
	NS_ENSURE_SUCCESS(rv, FALSE);

	if (isPreview) 
		rv = print->ExitPrintPreview();
	else {

		rv = print->PrintPreview(mPrintSettings, nullptr, nullptr);

	}

	return NS_SUCCEEDED(rv);
}


PRBool  CBrowserWrapper::LoadURL(const char* url, bool allowFixup)
{
	NS_ENSURE_TRUE(url, FALSE);
	NS_ENSURE_TRUE(mWebNav, FALSE);

	nsresult rv = mWebNav->LoadURI(NS_ConvertASCIItoUTF16(url).get(),
		allowFixup ?            
		nsIWebNavigation::LOAD_FLAGS_ALLOW_THIRD_PARTY_FIXUP : nsIWebNavigation::LOAD_FLAGS_NONE, 
		nullptr,                 
		nullptr,                 
		nullptr);   

	return NS_SUCCEEDED(rv);
}
