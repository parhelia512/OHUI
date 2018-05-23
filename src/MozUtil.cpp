
#include "../include/common.h"
#include "nsIWindowWatcher.h"
#include "nsIIOService.h"
#include "nsDirectoryServiceUtils.h"
#include "nsIDOMHTMLImageElement.h"
#include "nsIDOMHTMLInputElement.h"
#include "nsIDOMHTMLObjectElement.h"
#include "nsIDOMCSSPrimitiveValue.h"
#include "nsIDOMCSSStyleDeclaration.h"
#include "nsIDOMCharacterData.h"
#include "nsIDOMHTMLAreaElement.h"
#include "nsIDOMHTMLLinkElement.h"
#include "nsIDOMHTMLAnchorElement.h"
#include "nsIDOMDocument.h"
#include "nsIDOMLocation.h"
#include "nsIDOMWindow.h"
#include "nsIWebBrowserChrome.h"
#include "nsIImageLoadingContent.h"
#include "nsIConsoleService.h"
#include "nsIScriptError.h"
#include "nsIWebBrowser.h"
#include "nsIDOMEventTarget.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIBaseWindow.h"
#include "nsAlgorithm.h"  //XPCOM_MAX
#include <stdio.h>
#include <climits>
#include <algorithm>
#include "stdafx.h"
#include "MozUtil.h"
#include "macrodef.h"
#include "nsINetUtil.h"
#include "nsNetCID.h"

#ifdef XP_WIN
#include <Shlwapi.h> //StrStrIA
#include <atlexcept.h>
#include <atlconv.h>
#include "nsIWindowsRegKey.h"
#endif


#include "nsServiceManagerUtils.h"
#include "nsStringAPI.h"
#include "nsIWebNavigation.h"

#include "nsIFile.h"
#include "nsIDirectoryService.h"
#include "nsINetUtil.h"
#include "nsNetCID.h" //NS_IOSERVICE_CONTRACTID
#include "nsServiceManagerUtils.h" //do_GetService
#include "nsComponentManagerUtils.h" //do_CreateInstance
#include <plstr.h>  //PL_strcasecmp
#include "../3rdParty/ylxUtil/Util.h"


CMozUtil::CMozUtil(void)
{
}


CMozUtil::~CMozUtil(void)
{
}


nsCString CMozUtil::escape(const char* p)
{
	nsresult rv;
	nsCString aCharset;
	nsCOMPtr<nsINetUtil> netUtil = do_GetService(NS_IOSERVICE_CONTRACTID, &rv);
	if (!NS_SUCCEEDED(rv)) return aCharset;

	nsCString ac;
	ac.AssignASCII(p);

	netUtil->EscapeString(ac, nsINetUtil::ESCAPE_URL_PATH, aCharset);

	return aCharset;
}


nsresult CMozUtil::GetIOService(nsIIOService **ioService)
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

nsresult CMozUtil::NewURI (nsIURI **result,
	const nsAString &spec,
	const char *charset,
	nsIURI *baseURI)
{
	nsCString/*nsEmbedCString*/ cSpec;
	NS_UTF16ToCString (spec, NS_CSTRING_ENCODING_UTF8, cSpec);

	return NewURI (result, cSpec, charset, baseURI);
}

nsresult CMozUtil::NewURI (nsIURI **result,
	const nsACString &spec,
	const char *charset,
	nsIURI *baseURI)
{
	nsresult rv;

	nsCOMPtr<nsIIOService> ioService;
	rv = CMozUtil::GetIOService(getter_AddRefs (ioService));
	NS_ENSURE_SUCCESS (rv, rv);

	rv = ioService->NewURI (spec, charset, baseURI, result);

	return rv;
}
 

GtkWindow* CMozUtil::GetGtkWindowForDOMWindow(mozIDOMWindowProxy* aDOMWindow)
{
    nsCOMPtr<nsIWindowWatcher> wwatch = do_GetService("@mozilla.org/embedcomp/window-watcher;1");

    if (!aDOMWindow)
        return NULL;

    nsCOMPtr<nsIWebBrowserChrome> chrome;
    wwatch->GetChromeForWindow(aDOMWindow, getter_AddRefs(chrome));
    nsCOMPtr<nsIEmbeddingSiteWindow> siteWindow = do_QueryInterface(chrome);

    if (!siteWindow)
        return NULL;

    GtkWidget* parentWidget;
    siteWindow->GetSiteWindow((void**)&parentWidget);

    if (!parentWidget)
        return NULL;
 
    GtkWidget* gtkWit = gtk_widget_get_toplevel(parentWidget);
    GtkWindow* gtkWin = GTK_WINDOW(gtkWit);
    //if (GTK_WIDGET_TOPLEVEL(gtkWin))
    if (gtk_window_get_window_type(gtkWin)==GTK_WINDOW_TOPLEVEL)
        return (gtkWin);

    return NULL;
}

nsIWidget*  CMozUtil::GetHwndFromChrome(nsIWebBrowserChrome *aChrome)
{
	if (!aChrome)
	{
		return NULL;
	}
	nsCOMPtr<nsIEmbeddingSiteWindow> baseWindow = do_QueryInterface(aChrome);

    nsIWidget* hwnd=0;
	baseWindow->GetSiteWindow((void **) & hwnd);
	return hwnd;
}

nsIWidget* CMozUtil::GetTopWidgetFromChrome(nsIWebBrowserChrome *aChrome)
{
    GtkWidget*  h = (GtkWidget*)CMozUtil::GetHwndFromChrome(aChrome);
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

nsresult CMozUtil::ResizeEmbedding(nsIWebBrowserChrome* chrome)
{
	if (!chrome)
		return NS_ERROR_FAILURE;

	nsCOMPtr<nsIEmbeddingSiteWindow> embeddingSite = do_QueryInterface(chrome);
    nativeWindow hWnd;
	embeddingSite->GetSiteWindow((void **) & hWnd);

	if (!hWnd)
		return NS_ERROR_NULL_POINTER;

	// Make sure the browser is visible and sized
	nsCOMPtr<nsIWebBrowser> webBrowser;
	chrome->GetWebBrowser(getter_AddRefs(webBrowser));
	nsCOMPtr<nsIBaseWindow> webBrowserAsWin = do_QueryInterface(webBrowser);
	if (webBrowserAsWin)
	{
        gint w=0;
        gint h=0;
        gtk_window_get_size((GtkWindow*)hWnd,&w, &h);

        gint l=0,t=0;
        gtk_window_get_position((GtkWindow*)hWnd, &l, &t);
 
		webBrowserAsWin->SetPositionAndSize(l, 
			t, 
		    w, 
		    h,
			PR_TRUE);
		webBrowserAsWin->SetVisibility(PR_TRUE);
	}

	return NS_OK;
}

void CMozUtil::ShowWindow(nsIWebBrowserChrome *aChrome, PRBool aShow)
{
	nativeWindow win = CMozUtil::GetHwndFromChrome(aChrome);
	//::ShowWindow(win, aShow ? SW_RESTORE : SW_HIDE);
}

void CMozUtil::SizeTo(nsIWebBrowserChrome *aChrome, PRInt32 aWidth, PRInt32 aHeight)
{
	nsIWidget* hbrowser = CMozUtil::GetHwndFromChrome(aChrome);
    
#ifdef XP_WIN
	HWND hchrome  = ::GetParent(hbrowser);
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
	  gint x=0,y=0;
	  gint w=0;
	  gint h=0;
	  gtk_window_get_position((GtkWindow*)hbrowser,&x, &y);
	  gtk_window_get_size((GtkWindow*)hbrowser,&w, &h);
	  webBrowserAsWin->SetPositionAndSize(x,y,w,h,PR_TRUE);

	  webBrowserAsWin->SetVisibility(PR_TRUE);
    }//if(webBrowserAsWin)
#endif    
}

nsresult CMozUtil::GetDOMEventTarget (nsIWebBrowser* aWebBrowser, nsIDOMEventTarget** aTarget)
{
	NS_ENSURE_ARG(aWebBrowser);
	nsCOMPtr<mozIDOMWindowProxy> domWindowProxy;
	aWebBrowser->GetContentDOMWindow(getter_AddRefs(domWindowProxy));
	nsCOMPtr<nsIDOMWindow> domWin = do_QueryInterface(domWindowProxy);
			
	NS_ENSURE_TRUE (domWin, NS_ERROR_FAILURE);

#if GECKO_VERSION > GECKO_1_9_2_VER
	nsresult rs = domWin->GetWindowRoot (aTarget);
#else
	nsCOMPtr<nsIDOMWindow2> domWin2 (do_QueryInterface (domWin));
	NS_ENSURE_TRUE (domWin2, NS_ERROR_FAILURE);

	nsresult rs = domWin2->GetWindowRoot (aTarget);
#endif

	return rs;
}

char* CMozUtil::NS_strndup(const char *aString, uint32_t aLen)
{
  char *newBuf = (char*) NS_Alloc((aLen + 1) * sizeof(char));
  if (newBuf)
  {
    memcpy(newBuf, aString, aLen * sizeof(char));
    newBuf[aLen] = '\0';
  }
  return newBuf;
}


PRUnichar* CMozUtil::NS_strcpy(OUT PRUnichar* pDes, IN const PRUnichar* pSrc)
{
    if(!pSrc || !pDes) return pDes;
    
    uint32_t aLen = NS_strlen((const char16_t*)pSrc) +1;
    memcpy(pDes, pSrc, aLen * sizeof(PRUnichar));
    return pDes;
}


static already_AddRefed<nsIFile> CloneAndAppend(nsIFile* aFile, const char* name)
{
	nsCOMPtr<nsIFile> file;
	aFile->Clone(getter_AddRefs(file));
	file->AppendNative(nsDependentCString(name));
	return file.forget();
}

already_AddRefed<nsIFile>
CMozUtil::GenericIFile(const char* basename, const char* dirname, const char* leafname)
{
	nsCOMPtr<nsIFile> f;
	nsresult rv = NS_NewNativeLocalFile(nsDependentCString(basename), true,
		getter_AddRefs(f));
	if (NS_FAILED(rv))
		return NULL;

    if (dirname)
        f->AppendNative(nsDependentCString(dirname));

	if (leafname)
		f->AppendNative(nsDependentCString(leafname));
	return f.forget();
}

uint32_t CMozUtil::GetValFromIni(nsIINIParser* iniFile, const char* appSection, const char* key, char*& val, const char* defaultVal)
{
    if(val) val[0]=0;
    
    if(!iniFile || !appSection || !key) return 0;
    nsresult rv = NS_ERROR_FAILURE;
    uint32_t len=0;
    do
    {
        nsAutoCString rtval;
        rv = iniFile->GetString(nsDependentCString(appSection),
                                nsDependentCString(key),
                                rtval);
        if(NS_FAILED(rv))
        {
            printf("iniFile->GetString error %x \n", rv);
            break;
        }
        len = rtval.Length();
        
        if(val)
            memcpy(val, rtval.get(), len + 1);
        else
            val = CMozUtil::NS_strndup((const char*)rtval.get(), len + 1);
    }while(0);
    
    return len;
}

uint32_t CMozUtil::GetValFromIni(const char* fn, const char* appSection, const char* key, char*& val, const char* defaultVal)
{
    uint32_t len = 0;
    nsresult rv  = NS_ERROR_FAILURE;
    nsCOMPtr<nsIFile> nsCfgIni = CMozUtil::GenericIFile(fn, 0, 0);
    do{
        if(val) val[0]=0;
        if(!fn || !appSection || !key) break;
        
        if(!nsCfgIni) break;
        
        bool exists=false;
        rv = nsCfgIni->Exists(&exists);
        if(!exists) break;

        nsCOMPtr<nsIINIParserFactory> iniFactory = do_GetService("@mozilla.org/xpcom/ini-processor-factory;1", &rv);
        NS_ENSURE_SUCCESS(rv, len);

        nsCOMPtr<nsIINIParser> iniParser;
        rv = iniFactory->CreateINIParser(nsCfgIni, getter_AddRefs(iniParser));
        NS_ENSURE_SUCCESS(rv, len);
        if(!iniParser) break;
        nsAutoCString rtval;
        #if 0
        //why faild? sizeof() ahaha....
        rv = iniParser->GetString(NS_LITERAL_CSTRING(appSection),
                                  NS_LITERAL_CSTRING(key),
                                  rtval);
        #endif
        rv = iniParser->GetString(nsDependentCString(appSection),
                                  nsDependentCString(key),
                                  rtval);        
        if(NS_FAILED(rv))
        {
            printf("iniFile->GetString error %x \n", rv);
            break;
        }
        len = rtval.Length();
        
        if(val)
            memcpy(val, rtval.get(), len + 1);
        else
            val = CMozUtil::NS_strndup((const char*)rtval.get(), len + 1);        
    
    }while(0);

    return len;    
}

bool CMozUtil::GetWebUIPath(OUT PRUnichar* ptchPath, OUT char* path)
{
	if(!ptchPath && !path) return false;

    nsresult rv;
    const char* pchCurrFullPath = g_pchCurrFullPath;
    uint32_t nCurrPathLen = strlen(pchCurrFullPath);
    char* chTmp = new char[nCurrPathLen+1000];// (char*)malloc(nCurrPathLen+1000);
    strcpy(chTmp, pchCurrFullPath);
    strcat(chTmp, INICFGFILE);

    do
    {
        //at first, get url from cfg.ini
        char *url=0;
        CMozUtil::GetValFromIni(chTmp, "Setting", "url", url, "");        
        if(!url) break;
        
        int len = strlen(url);
        char* chOutPath =0;
        if(!url[0] 
         || (!PL_strcasestr(url, ":/") && !PL_strcasestr(url, "://") && !PL_strcasestr(url, ":\\")))
        {
            //empty string or
            //it's a related path 
            chOutPath = new char[nCurrPathLen+100];
            strcpy(chOutPath,"file://");
            strcat(chOutPath, pchCurrFullPath);
            strcat(chOutPath, "WebUI/"); 
        }else
        {
            chOutPath = new char[len + 200];
            chOutPath[0]=0;
            if (!PL_strcasestr(url, "://"))
            {
               strcpy(chOutPath, "file://");
            }
            nsYLX::CUtil::GetParentPath(url,chOutPath+strlen(chOutPath), len,0);
        }
        
        if(url)
        {
            NS_Free(url);url=0;
        }
        nsCOMPtr<nsINetUtil> netUtil = do_GetService(NS_IOSERVICE_CONTRACTID, &rv);
        if (NS_SUCCEEDED(rv)) 
        {
            nsCString ac;
            ac.AssignLiteral(chOutPath);
            //ac.AssignASCII(T2A(tchOutPath));
        
            nsCString aCharset;
            //解决中文路径问题
            netUtil->EscapeURL(ac, nsINetUtil::ESCAPE_URL_DIRECTORY, aCharset);
            //netUtil->EscapeString(ac, nsINetUtil::ESCAPE_URL_PATH, aCharset);

            strcpy(chOutPath, aCharset.BeginReading());
        }//
    
        if(path)
        {
            strcpy(path, chOutPath);
        }
        
        if(ptchPath)
        {
            nsCString cs;
            cs.Assign(chOutPath);
        
            nsString aDest;
            NS_CStringToUTF16(cs, NS_CSTRING_ENCODING_UTF8, aDest);

            CMozUtil::NS_strcpy(ptchPath, (const PRUnichar*)aDest.get()); 
        }
        
    }while(0);
    
	//free(chTmp);    
	delete[] chTmp;
	return true;
}

bool CMozUtil::GetReadmePath(OUT PRUnichar* ptchPath, OUT char* path)
{
	if(!ptchPath && !path) return false;

    uint32_t len = strlen(g_pchCurrFullPath);
    char* chTmp = new char[len+100];

	//use for introducing to everyone
#if 1
	chTmp[0]=0;
	strcpy(chTmp,"file:///");
	strcat(chTmp, g_pchCurrFullPath);
    strcat(chTmp, "WebUI/");
#endif
    
	nsresult rv;
	nsCOMPtr<nsINetUtil> netUtil = do_GetService(NS_IOSERVICE_CONTRACTID, &rv);
	if (NS_SUCCEEDED(rv)) 
	{
		nsCString ac;
		ac.AssignLiteral(chTmp);
        
		nsCString aCharset;

		netUtil->EscapeURL(ac, nsINetUtil::ESCAPE_URL_DIRECTORY, aCharset);

        strcpy(chTmp, aCharset.BeginReading());
    }//

    if(path)
    {
        strcpy(path, chTmp);
    }
    
	if(ptchPath)
	{
        
        nsCString cs;
        cs.Assign(chTmp);
        
        nsString aDest;
        NS_CStringToUTF16(cs, NS_CSTRING_ENCODING_UTF8, aDest);

        NS_strcpy(ptchPath, (const PRUnichar*)aDest.get()); 
	}	

    delete[] chTmp;
	return true;
}

#ifdef XP_WIN
PRUint32 CMozUtil::RunEventLoop(PRBool &aRunCondition)
{
	MSG msg;
	HANDLE hFakeEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	while (aRunCondition ) {
		// Process pending messages
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!::GetMessage(&msg, NULL, 0, 0)) {
				// WM_QUIT
				aRunCondition = PR_FALSE;
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		// Do idle stuff
		::MsgWaitForMultipleObjects(1, &hFakeEvent, FALSE, 100, QS_ALLEVENTS);
	}
	::CloseHandle(hFakeEvent);
	return msg.wParam;
}
#endif

