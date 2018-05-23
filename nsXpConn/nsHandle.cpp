#include "nsHandle.h"
#include "nsMemory.h"
 
#include <stdlib.h>
#include <stdio.h>
#include "../include/common.h"

#include "nsCOMPtr.h" 
#include "nsEmbedString.h"
#include "nsIClassInfoImpl.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsIDOMWindow.h"

#include "nsIDOMDocument.h"
#include "nsIDOMElement.h"
#include "nsIDOMNodeList.h"
#include "nsIDOMChromeWindow.h"
#include "nsIBrowserDOMWindow.h"
#include "nsIBaseWindow.h"
#include "nsIWebBrowser.h"
#include "nsIWebNavigation.h"
#include "nsIDocShell.h"
//
#include "nsIDocument.h"
#include "nsIWebBrowserChrome.h"
#include "nsIDocShellTreeOwner.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIWebBrowserPrint.h"
#include "nsIPrintSettingsService.h"
#include "nsServiceManagerUtils.h"   //do_GetService
#include "nsIWindowWatcher.h"


#ifdef XP_WIN
#include <windows.h>
#include <shellapi.h>
#else
#include <gtk/gtk.h>
#include <glib-unix.h>  //gint
#include <gtk/gtkwindow.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/wait.h>
#include <pthread.h>
#endif

 
NS_IMPL_CLASSINFO(nsHandleImpl, NULL, 0, NS_SAMPLE_CID)
NS_IMPL_ISUPPORTS_CI(nsHandleImpl, nsIOhsXpConn)


nsHandleImpl::nsHandleImpl()
{
}

nsHandleImpl::~nsHandleImpl()
{
}
 
NS_IMETHODIMP nsHandleImpl::Add(PRInt32 a, PRInt32 b, PRInt32 *_retval)
{
	*_retval = a + b ;
	return NS_OK;
}

char* g_appstr=0;
char* g_paramsstr=0;
static void* thread_exec(void *param)
{
    char* appstr=0;
    char* paramsstr=0;
    do
    {
        if(!g_appstr || !g_paramsstr) break;
        int len = strlen(g_appstr);
        int len2 = strlen(g_paramsstr);
        
        appstr = (char*)malloc(len+len2+100);
        strcpy(appstr, g_appstr);         
        strcat(appstr, "  '");
        strcat(appstr, g_paramsstr);
        strcat(appstr, "'");
        
        system(appstr);
        
        free(appstr);
        if(paramsstr) free(paramsstr);        
    }while(0);
    
    return NULL;
}

NS_IMETHODIMP nsHandleImpl::Exec(const nsACString & app, const nsACString & params, int32_t *_retval)
{
    const char* appstr;
    const char* paramsstr;
    int len=0;
    do
    {
       char *const env[] = {"",NULL};
       //execv execve execlp execl execvp
       int status; 
      
       len=NS_CStringGetData(app, &appstr);
       if(!len) break;

       int len2 = NS_CStringGetData(params, &paramsstr);
       if(!g_appstr)
       {
           g_appstr= (char*)malloc(len+1);
       }else
       {
           g_appstr= (char*)realloc(g_appstr,len+1);
       }
       strcpy(g_appstr, appstr);
       
       if(!g_paramsstr)
       {
           g_paramsstr= (char*)malloc(len2+1);
       }else
       {
           g_paramsstr= (char*)realloc(g_paramsstr,len2+1);
       }
       strcpy(g_paramsstr, paramsstr);
       
       pthread_t thread;
       pthread_create(&thread,NULL,&thread_exec,NULL);

    }while(0);
    return NS_OK;
}
 
