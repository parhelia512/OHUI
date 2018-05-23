 

gint
handle_focus_out(GtkWidget     *aWidget,
		       GdkEventFocus *aGdkFocusEvent,
		       gpointer data)
{
  CBrowserWrapper* pWrapper = (CBrowserWrapper*)data;
  pWrapper->SetActive(false);

  return true;//must return true!!
}

gint
handle_focus_in(GtkWidget     *aWidget,
		      GdkEventFocus *aGdkFocusEvent,
		      gpointer data)
{
  nsresult rv;
  
  CBrowserWrapper* pWrapper = (CBrowserWrapper*)data;
  pWrapper->SetActive(true);

  return FALSE;
}

gint 
wnd_resizing( GtkWidget *widget,
		    GdkEvent *event,
		    gpointer data )
{
   int cX,cY;

   cX = event->configure.width;
   cY = event->configure.height;
   //printf("wnd_resizing....type:%d  %d, %d, %d, %d %s\n",event->configure.type, x,y,cX,cY, pWrapper->mTest.c_str());
   
   CBrowserWrapper* pWrapper = (CBrowserWrapper*)data; 
   nsIWebBrowserChrome *chrome = static_cast<nsIWebBrowserChrome*>(pWrapper->mpBrowserImpl);
   nsCOMPtr<nsIWebBrowser> webBrowser = pWrapper->GetWebBrowser();
   do
   {
       if(!webBrowser) break;
	
      	nsCOMPtr<nsIBaseWindow> webBrowserAsWin = do_QueryInterface(webBrowser);
      	if(!webBrowserAsWin) break;
      	
      	webBrowserAsWin->SetPositionAndSize(0,0,cX,cY,PR_TRUE);	
   }while(0);

  return FALSE;
}

gint DomLoadCompleted( GtkWidget *widget,
                       GdkEvent *event,
                       gpointer data,
                       gpointer userData
                       )
{
    CBrowserWrapper* pWrapper = (CBrowserWrapper*)data;
    printf("Dom Loa dCompleted! \n");
    gtk_widget_show_all(widget);
    return FALSE;
}

gboolean accel_f5(GtkAccelGroup *accel_group,
GObject *acceleratable,
guint keyval,
GdkModifierType modifier,
gpointer data)
{
    CBrowserWrapper* pWrapper = (CBrowserWrapper*)data;
    nsIWebBrowserChrome *chrome = 0;
    do{
        if(pWrapper && pWrapper->mWebBrowser)
            pWrapper->mWebBrowser->GetContainerWindow(&chrome);
        if(!chrome) break;

        nsCOMPtr<nsIWebBrowser> newBrowser;
        chrome->GetWebBrowser(getter_AddRefs(newBrowser));
        nsCOMPtr<nsIWebNavigation> webNav(do_QueryInterface(newBrowser));
        webNav->Reload(0); //LOAD_RELOAD_NORMAL
    }while(0);
    //printf("accel_f5 \n");
    return false;
}
