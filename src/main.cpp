


#include <libgen.h>

#include "stdafx.h"
#include "ylxUtil.h"
#include "macrodef.h"

#include "nsAppDirectoryServiceDefs.h"
#include "nsDirectoryServiceDefs.h"
#include "nsProfileDirServiceProvider.h"
#include "nsXPCOMGlue.h"

#include "nsIClipboardCommands.h"
#include "nsIInterfaceRequestor.h"
#include "nsIObserverService.h"
#include "nsIObserver.h"
#include "nsIURI.h"
#include "nsIWebBrowserFocus.h"
#include "nsIWindowWatcher.h"

#include "nsXPCOMCIDInternal.h"

#include "mozilla/AppData.h"
#include "nsIXULAppInfo.h"
#include "nsIXPConnect.h"
#include "jspubtd.h"
// NON-FROZEN APIs!
#include "nsIBaseWindow.h"
#include "nsIWebNavigation.h"
#include "nsDocShellLoadTypes.h"
#include "nsIScriptSecurityManager.h"  //nsIScriptSecurityManager
#include "nsIPermissionManager.h"
#include "nsIComponentRegistrar.h"
#include "nsIPromptFactory.h"
#include "nsIPromptService.h"
#include "nsIFile.h"

#include "WebBrowserChrome.h"
#include "WindowCreator.h"
#include "nsIAppStartup.h"
#include "nsToolkitCompsCID.h"

#include "WebBrowserChromeUI.h"
#include "nsXREAppData.h" 
#include "nsITextToSubURI.h"   //nsITextToSubURI
#include "nsINetUtil.h"
#include "nsNetCID.h" //NS_IOSERVICE_CONTRACTID

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gtk/gtkdialog.h>

#include "../images/48x48.xpm"
#include "../3rdParty/ylxUtil/Util.h"
#include "nsIDirectoryService.h"
#include "mozilla/RefCountType.h"


#include "nsDirectoryServiceUtils.h"
#include "gtkmozembedmarshal.h"
#include "MozEmbedSignal.h"
#include "MozUtil.h"
#include "OhuiApp.h"
#include "MainFrame.h"
#include <plstr.h>  //PL_strcasecmp

#include  "mainSignalCallback.cpp"
# include <X11/Xlib.h>
# include <X11/Xlibint.h>

/* This callback quits the program */
static
gint wnd_del_callback( GtkWidget *widget,
                   GdkEvent *event,
                   gpointer data )
{
  gtk_main_quit ();
  
  return FALSE;
}



#include "nsXULAppAPI.h"
XRE_API(int,
	XRE_mainInit, (bool* aExitFlag))

XRE_API(int,
	XRE_mainStartup, (bool* aExitFlag))

XRE_NotifyProfileType XRE_NotifyProfile;  
XRE_InitEmbedding2Type XRE_InitEmbedding;
XRE_TermEmbeddingType XRE_TermEmbedding;
XRE_GetFileFromPathType  XRE_GetFileFromPath;
XRE_AddManifestLocationType XRE_AddManifestLocation;
XRE_AddStaticComponentType XRE_AddStaticComponent = 0;
 
XRE_mainInitType XRE_mainInit;
XRE_mainStartupType XRE_mainStartup; 

XRE_CreateAppDataType XRE_CreateAppData;
XRE_FreeAppDataType XRE_FreeAppData;
#ifdef XRE_HAS_DLL_BLOCKLIST
XRE_SetupDllBlocklistType XRE_SetupDllBlocklist;
#endif
XRE_TelemetryAccumulateType XRE_TelemetryAccumulate;
XRE_StartupTimelineRecordType XRE_StartupTimelineRecord;
XRE_mainType XRE_main;
//XRE_DisableWritePoisoningType XRE_DisableWritePoisoning;


static const nsDynamicFunctionLoad kXULFuncs[] = {
	{ "XRE_GetFileFromPath", (NSFuncPtr*) &XRE_GetFileFromPath },
	{ "XRE_CreateAppData", (NSFuncPtr*) &XRE_CreateAppData },
	{ "XRE_FreeAppData", (NSFuncPtr*) &XRE_FreeAppData },
#ifdef XRE_HAS_DLL_BLOCKLIST
	{ "XRE_SetupDllBlocklist", (NSFuncPtr*) &XRE_SetupDllBlocklist },
#endif
	{ "XRE_TelemetryAccumulate", (NSFuncPtr*) &XRE_TelemetryAccumulate },
	{ "XRE_StartupTimelineRecord", (NSFuncPtr*) &XRE_StartupTimelineRecord },
	{ "XRE_main", (NSFuncPtr*) &XRE_main },
	//{ "XRE_DisableWritePoisoning", (NSFuncPtr*) &XRE_DisableWritePoisoning },
	{ "XRE_NotifyProfile", (NSFuncPtr*) &XRE_NotifyProfile },
	{ "XRE_InitEmbedding2", (NSFuncPtr*) &XRE_InitEmbedding },
	{ "XRE_TermEmbedding", (NSFuncPtr*) &XRE_TermEmbedding },
	{ "XRE_AddManifestLocation", (NSFuncPtr*) &XRE_AddManifestLocation }, 
  { "XRE_AddStaticComponent", (NSFuncPtr*)&XRE_AddStaticComponent},
	//{ "XRE_mainInit", (NSFuncPtr*) &XRE_mainInit },
	//{ "XRE_mainStartup", (NSFuncPtr*) &XRE_mainStartup },
	{ nullptr, nullptr }
};


static nsIDirectoryServiceProvider * sAppFileLocProvider=0;
static nsIFile           *sProfileDir=0;

class GTKEmbedDirectoryProvider : public nsIDirectoryServiceProvider2
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSIDIRECTORYSERVICEPROVIDER
  NS_DECL_NSIDIRECTORYSERVICEPROVIDER2
};

static const GTKEmbedDirectoryProvider kDirectoryProvider;

NS_IMPL_QUERY_INTERFACE(GTKEmbedDirectoryProvider,
                         nsIDirectoryServiceProvider,
                         nsIDirectoryServiceProvider2)
 

NS_IMETHODIMP_(MozExternalRefCountType)
GTKEmbedDirectoryProvider::AddRef()
{
  return 1;
}

NS_IMETHODIMP_(MozExternalRefCountType)
GTKEmbedDirectoryProvider::Release()
{
  return 1;
}

NS_IMETHODIMP
GTKEmbedDirectoryProvider::GetFile(const char *aKey, bool *aPersist,
                                   nsIFile* *aResult)
{
  if (sAppFileLocProvider) {
    nsresult rv = sAppFileLocProvider->GetFile(aKey, aPersist,
                                                             aResult);
    if (NS_SUCCEEDED(rv))
      return rv;
  }

  if (sProfileDir && (!strcmp(aKey, NS_APP_USER_PROFILE_50_DIR)
                                 || !strcmp(aKey, NS_APP_PROFILE_DIR_STARTUP))) {
    *aPersist = PR_TRUE;
    return sProfileDir->Clone(aResult);
  }

  return NS_ERROR_FAILURE;
}

NS_IMETHODIMP
GTKEmbedDirectoryProvider::GetFiles(const char *aKey,
                                    nsISimpleEnumerator* *aResult)
{
  nsCOMPtr<nsIDirectoryServiceProvider2>
    dp2(do_QueryInterface(sAppFileLocProvider));

  if (!dp2)
    return NS_ERROR_FAILURE;

  return dp2->GetFiles(aKey, aResult);
}
 
nsIDirectoryServiceProvider* StartupProfile()
{
  
	nsCOMPtr<nsIFile> appDataDir;
	nsresult rv = NS_GetSpecialDirectory(NS_APP_APPLICATION_REGISTRY_DIR, getter_AddRefs(appDataDir));
	if (NS_FAILED(rv))
		return nullptr;

	appDataDir->AppendNative(nsCString("winembed52.0"));

	nsCOMPtr<nsIFile> localAppDataDir(do_QueryInterface(appDataDir));

	nsCOMPtr<nsProfileDirServiceProvider> locProvider;
	NS_NewProfileDirServiceProvider(PR_TRUE, getter_AddRefs(locProvider));

	if (!locProvider)
		return nullptr;

	rv = locProvider->Register();
	if (NS_FAILED(rv))
		return nullptr;

	rv = locProvider->SetProfileDir(localAppDataDir);

	return locProvider;
}

void messagebox(GtkWindow* parent, const char* fmt, ...)
{
    GtkWidget* dialog;
    GtkWidget *label;
    GtkWidget* hbox;
    
    char * pstr = new char[1000];
    va_list ap;  
    va_start(ap,fmt);         

    vsnprintf(pstr, 999, fmt, ap);
    
    va_end(ap);    

    
#ifndef SCHEMALESS_WINDOW
    dialog = gtk_dialog_new_with_buttons("tips:", parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
#else
    dialog = gtk_dialog_new_with_buttons("tips:", parent,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
#endif

    label = gtk_label_new(pstr);
    hbox  = gtk_hbox_new(FALSE,5);
    
    gtk_box_set_center_widget(GTK_BOX(hbox),label);
    //gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_set_center_widget(GTK_BOX(GTK_DIALOG(dialog)),hbox);
    //gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);    

    delete[] pstr;
#ifndef SCHEMALESS_WINDOW
    /*显示对话框*/
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
#else
    g_signal_connect(G_OBJECT(dialog),"response",G_CALLBACK(gtk_widget_destroy),NULL);
#endif
    
}

COhuiApp  theApp;
extern mozilla::Module kWndPromptServiceModule;//Components.cpp

int main (int argc,char *argv[])
{
    GtkWidget *wnd=0;
    GdkPixbuf *app_icon_pixbuf=0;
	nsresult rv;
    
    // the path to the GRE
    char *sGrePath=0;
    
    // the path to components
    char *sCompPath=0;

    XInitThreads();
    gtk_init (&argc,&argv);
     
    
    int dest_len = 1000;
    char *xpcom_path = new char[dest_len];
    xpcom_path[0] = 0;

    if (realpath (argv[0], xpcom_path) == 0)
    {
        //error !
    }
    dirname (xpcom_path);
    strcat  (xpcom_path, "/");
    //printf("path: %s\n", xpcom_path);
    int nlen = strlen(xpcom_path);
    g_pchCurrFullPath= new char[nlen+1];
    strcpy(g_pchCurrFullPath, xpcom_path);

    sCompPath = new char[strlen(xpcom_path)+100];
    strcpy(sCompPath, xpcom_path);
    strcat(sCompPath, "components/");

    // We do this because of data in bug 771745
    XPCOMGlueEnablePreload();

  	rv = XPCOMGlueStartup(xpcom_path);
	  if (NS_FAILED(rv))
    {
        messagebox(GTK_WINDOW (wnd), "XPCOMGlueStartup error: %s", xpcom_path);
    }
 
    
    rv = XPCOMGlueLoadXULFunctions(kXULFuncs);



    nsCOMPtr<nsIFile> nsBinPath;
    rv = XRE_GetFileFromPath(xpcom_path, getter_AddRefs(nsBinPath));
    
	  if (NS_FAILED(rv))
    {
        messagebox(GTK_WINDOW (wnd), "XRE_GetFileFromPath error: %s", xpcom_path);
    }
    
    nsCOMPtr<nsIFile> compDir;
    rv = NS_NewNativeLocalFile(nsDependentCString(sCompPath), PR_TRUE, getter_AddRefs(compDir));
    if (NS_FAILED(rv))
    {
        messagebox(GTK_WINDOW (wnd), "NS_NewNativeLocalFile error: %s", sCompPath);        
    } 

	 nsCOMPtr<nsIXULAppInfo> appinfo = do_GetService(XULAPPINFO_SERVICE_CONTRACTID, &rv);
       
   rv = XRE_InitEmbedding(nsBinPath
   ,nsBinPath
   ,nullptr);
       
	if (NS_FAILED(rv))
  {
      messagebox(GTK_WINDOW (wnd), "XRE_InitEmbedding error: %s", xpcom_path);
  }
      
  theApp.InitInstance();

  StartupProfile();//must be call after  XRE_InitEmbedding    
	nsCOMPtr<nsIAppStartup> appStartup(do_GetService(NS_APPSTARTUP_CONTRACTID));
	if (appStartup) {
		appStartup->CreateHiddenWindow();
		appStartup->Run();
	}
			
	nsCOMPtr<nsIServiceManager> servMan;
	rv = NS_GetServiceManager(getter_AddRefs(servMan));
    
	// register all components in our default component directory
	nsCOMPtr<nsIComponentRegistrar> registrar = do_QueryInterface(servMan);
	rv = registrar->AutoRegister(compDir);//nsComponentPath
	if (NS_FAILED(rv)) return -1;    

    XRE_NotifyProfile();

    //must be  register after  all default's extension have been registered! do that, we can just overlay default's services 
    //rv = XRE_AddStaticComponent(&kWndPromptServiceModule); 



    char *shortUrl=0;
    std::string sIniFile = g_pchCurrFullPath;
    sIniFile += INICFGFILE;
    uint32_t len = CMozUtil::GetValFromIni(sIniFile.c_str(), "Setting", "url", shortUrl, "xx");
    char* webPath = new char[len+200];
    CMozUtil::GetWebUIPath(OUT 0, OUT webPath);
    std::string sUrl;
    if(!shortUrl[0])
  	{
          strcpy(shortUrl,"index.html");
          sUrl = webPath;
          sUrl += shortUrl;
    }else if (!PL_strcasestr(shortUrl, ":/") && !PL_strcasestr(shortUrl, "://") && !PL_strcasestr(shortUrl, ":\\"))
  	{
        //is related path or file for './WebUI/'
        sUrl = webPath;
        sUrl += shortUrl;
    }else
    {
        //it's a absolute path
        sUrl = shortUrl;
    }
    
    delete[] webPath;
    if(shortUrl)
    {
        NS_Free(shortUrl);shortUrl=0;
    }    
 
    CMainFrame* pMain = new CMainFrame();
    wnd = (GtkWidget*)pMain->CreateWindow();
    gtk_window_set_title(GTK_WINDOW (wnd), "他山界面 v52.0.2");

 
     //"check-resize" 
    g_signal_connect(G_OBJECT(wnd)
			    ,"configure-event"//,"check-resize"  //
			    , G_CALLBACK(wnd_resizing)
			    , (gpointer) pMain->m_pBrowserWrapper);

    g_signal_connect(G_OBJECT(wnd)
                     ,"focus_out_event"
                     ,G_CALLBACK(handle_focus_out)
                     ,(gpointer)pMain->m_pBrowserWrapper
                     //,G_CONNECT_AFTER
                     );

     g_signal_connect(G_OBJECT(wnd)
                      ,"focus_in_event"
                      ,G_CALLBACK(handle_focus_in)
                      , (gpointer)pMain->m_pBrowserWrapper
                      //,G_CONNECT_AFTER
                      );
 
    moz_embed_signals[DOM_LOADCOMPLETED] =
    g_signal_new("dom_loadcompleted",
                 G_TYPE_OBJECT,//G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_LAST,
                 0,//G_STRUCT_OFFSET(GtkMozEmbedSignal, dom_loadcompleted),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__POINTER,//gtkmozembed_BOOL__POINTER,
		             G_TYPE_NONE,1, G_TYPE_POINTER
                );

    g_signal_connect(G_OBJECT(wnd),
                          "dom_loadcompleted",
                          G_CALLBACK(DomLoadCompleted),
                          (gpointer)pMain->m_pBrowserWrapper// G_OBJECT(wnd),//,
                          ); 

     GtkAccelGroup *accel_group;
     accel_group=gtk_accel_group_new();
     gtk_window_add_accel_group(GTK_WINDOW(wnd),accel_group);
     GClosure*closure=g_cclosure_new(G_CALLBACK(accel_f5),(void*)pMain->m_pBrowserWrapper, NULL);
     gtk_accel_group_connect (accel_group, GDK_F5 , (GdkModifierType)0, GTK_ACCEL_VISIBLE, closure);
  
    pMain->m_pBrowserWrapper->CreateBrowser(wnd);
    pMain->m_pBrowserWrapper->LoadURL(sUrl.c_str());
 
    gtk_main();

     if(shortUrl)
    {
        NS_Free(shortUrl);shortUrl=0;
    }
    delete[] xpcom_path;
    delete[] sCompPath;
    
	if (appStartup)
	{
		appStartup->DestroyHiddenWindow();
		appStartup->Quit(nsIAppStartup::eForceQuit);
	}
    // we no longer need a reference to the DirectoryServiceProvider
    if (sAppFileLocProvider)
    {
      NS_RELEASE(sAppFileLocProvider);
      sAppFileLocProvider = nsnull;
    }
    if (sProfileDir)
        NS_RELEASE(sProfileDir);

    theApp.ExitInstance();

    delete pMain; pMain=0;
    
    NS_ShutdownXPCOM(nsnull);
        
    return(0);
}
