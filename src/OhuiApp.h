#pragma once
#include "nsIObserver.h"
#include "nsWeakReference.h"
#include "nsIWindowCreator.h"

class COhuiApp:
		public nsIObserver,
		public nsIWindowCreator,
		public nsSupportsWeakReference
{

public:
	COhuiApp(void);
  	~COhuiApp(void);
	
	PRBool  InitInstance();
	int  ExitInstance();
	nsresult InitializeWindowCreator();
	nsresult OverrideComponents();

	NS_DECL_ISUPPORTS
	NS_DECL_NSIOBSERVER
	NS_DECL_NSIWINDOWCREATOR
};

