#pragma once

#ifdef XP_WIN
#include <Windows.h>
#else
#include <gtk/gtk.h>
#include <glib-unix.h>  //gint
#include <gtk/gtkwindow.h>
#endif

#include "macrodef.h"

#include "nsCRTGlue.h"  //NS_strlen
#include "nsStringAPI.h"
#include "nsIWebBrowser.h"
#include "nsIDOMWindow.h"
#include "nsIDOMEventTarget.h"
#include "nsIIOService.h"   //nsIIOService
#include "nsIBaseWindow.h"
#include "mozIDOMWindow.h" //mozIDOMWindowProxy
#include "nsIWebNavigation.h"
#include <nsIINIParser.h>
#include "nspr/prtypes.h"

class CMozUtil
{
public:
	CMozUtil(void);
	~CMozUtil(void);

	static nsCString  escape(const char* p);
    static char*      NS_strndup(const char *aString, uint32_t aLen);
    static PRUnichar* NS_strcpy(OUT PRUnichar* pDes, IN const PRUnichar* pSrc);
    
	static nsIWidget* GetHwndFromChrome(nsIWebBrowserChrome *aChrome);
    static nsIWidget* GetTopWidgetFromChrome(nsIWebBrowserChrome *aChrome);    
    static GtkWindow* GetGtkWindowForDOMWindow(mozIDOMWindowProxy* aDOMWindow);

	static bool GetWebUIPath(OUT  PRUnichar* ptchPath, OUT char* path=0);
	static bool GetReadmePath(OUT PRUnichar* ptchPath, OUT char* path=0);	
	static nsresult GetDOMEventTarget (nsIWebBrowser* aWebBrowser, nsIDOMEventTarget** aTarget);

#ifdef XP_WIN
	static PRUint32 RunEventLoop(PRBool &aRunCondition);
	static nsresult SetupFlashPluginPath(const nsAutoString&  nsKey, const nsAutoString& nsFlashPluginPath);    
#endif
    
	static void ShowWindow(nsIWebBrowserChrome *aChrome, PRBool aShow);
	static void SizeTo(nsIWebBrowserChrome *aChrome, PRInt32 aWidth, PRInt32 aHeight);
	static nsresult ResizeEmbedding(nsIWebBrowserChrome* chrome);
    static uint32_t GetValFromIni(nsIINIParser* iniFile, const char* appSection, const char* key, char*& val, const char* defaultVal="");
    static uint32_t GetValFromIni(const char* fn, const char* appSection, const char* key, char*& val, const char* defaultVal="");
	static nsresult GetIOService(nsIIOService **ioService);
	static already_AddRefed<nsIFile> GenericIFile(const char* basename, const char* dirname, const char* leafname);
    
	static nsresult NewURI (nsIURI **result,
		const nsAString &spec,
		const char *charset = nsnull,
		nsIURI *baseURI = nsnull);

	static nsresult NewURI (nsIURI **result,
		const nsACString &spec,
		const char *charset = nsnull,
		nsIURI *baseURI = nsnull);

	protected:
	void NS_strcpy(PRUnichar* ptchPath, char16ptr_t get);
};

