/****************************************************************
OHUI  v52, developed with vs enterprise 2015 update3
whenever you meet any problem  about OHUI, please contact me. 
http://yaolixing.oltag.com/gn-A65A3AD3-97A4-4A3B-AFEB-EF4F36C814CC.html
QQ groups: 264695584
					------By Other Hill  2017.12
*****************************************************************/

#include "nsStringGlue.h"
#include "nsIDOMWindow.h"
#include "nsIWindowWatcher.h"
#include "nsIWebBrowserChrome.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsCOMPtr.h"
#include "nsIServiceManager.h"
#include "nsServiceManagerUtils.h"
 
#include "PromptService.h"
#include "mozilla/ModuleUtils.h"
#include "nsIClassInfoImpl.h"
#include "nsCOMPtr.h" 
#include "nsMemory.h"
#include "mozIDOMWindow.h"
#include "nsPIDOMWindow.h"
#include "nsIWebBrowser.h"
#include "nsIBaseWindow.h"
#include "nsIWidget.h"
#include "nsIInterfaceRequestorUtils.h"   //do_GetInterface
#include "nsIDOMChromeWindow.h"
#include "nsIBrowserDOMWindow.h"
#include "nsIDOMWindow.h"
#include "./MainFrame.h"  

extern CMainFrame* g_pMainFrame;

NS_IMPL_ISUPPORTS(PromptFactory, nsIPromptFactory, nsIFactory)

NS_IMETHODIMP 
PromptFactory::CreateInstance(nsISupports *aOuter, const nsIID & iid, void **result)
{
	NS_ENSURE_NO_AGGREGATION(aOuter);

	nsresult res = NS_ERROR_NO_AGGREGATION;
	if (aOuter == NULL) 
	{
		nsIFactory* factory = new PromptFactory;
		if (factory != NULL) 
		{
			res = factory->QueryInterface(iid, result);
			if (res != NS_OK)
				delete factory;
		}else 
		{
			res = NS_ERROR_OUT_OF_MEMORY;
		}
	}
	return res; 
}
 
NS_IMETHODIMP 
PromptFactory::LockFactory(bool lock)
{

	return NS_OK;
}

nsresult PromptFactory::GetPrompt(mozIDOMWindowProxy *aParent, const nsIID & iid, void **result)
{

	return NS_OK;
}

NS_IMPL_ISUPPORTS(CPromptService, nsIPromptFactory, nsIPrompt, nsIPromptService)

CPromptService::CPromptService()
{
}

CPromptService::~CPromptService()
{
}

NS_IMETHODIMP 
CPromptService::GetPrompt(mozIDOMWindowProxy *aParent, const nsIID & iid, void **result)
{
	mDomWindow = aParent;
	return QueryInterface(iid, result);
}

NS_IMETHODIMP CPromptService::Alert(const char16_t * dialogTitle, const char16_t * text)
{
	return Alert(mDomWindow, dialogTitle, text);
}

NS_IMETHODIMP CPromptService::AlertCheck(const char16_t * dialogTitle, const char16_t * text, const char16_t * checkMsg, bool *checkValue)
{
	return AlertCheck(mDomWindow, dialogTitle, text, checkMsg, checkValue);
}

NS_IMETHODIMP CPromptService::Confirm(const char16_t * dialogTitle, const char16_t * text, bool *_retval)
{
	return Confirm(mDomWindow, dialogTitle, text, _retval);
}

NS_IMETHODIMP CPromptService::ConfirmCheck(const char16_t * dialogTitle, const char16_t * text, const char16_t * checkMsg, bool *checkValue, bool *_retval)
{
	return ConfirmCheck(mDomWindow, dialogTitle, text, checkMsg, checkValue, _retval);
}

NS_IMETHODIMP CPromptService::ConfirmEx(const char16_t * dialogTitle, const char16_t * text, uint32_t buttonFlags, const char16_t * button0Title, const char16_t * button1Title, const char16_t * button2Title, const char16_t * checkMsg, bool *checkValue, int32_t *_retval)
{
	return ConfirmEx(mDomWindow, dialogTitle, text, buttonFlags, button0Title, button1Title, button2Title, checkMsg, checkValue, _retval);
}

NS_IMETHODIMP CPromptService::Prompt(const char16_t * dialogTitle, const char16_t * text, char16_t * *value, const char16_t * checkMsg, bool *checkValue, bool *_retval)
{
	return Prompt(mDomWindow, dialogTitle, text, value, checkMsg, checkValue, _retval);
}


NS_IMETHODIMP CPromptService::PromptPassword(const char16_t * dialogTitle, const char16_t * text, char16_t * *password, const char16_t * checkMsg, bool *checkValue, bool *_retval)
{
	return PromptPassword(mDomWindow, dialogTitle, text, password, checkMsg, checkValue, _retval);
}

NS_IMETHODIMP CPromptService::PromptUsernameAndPassword(const char16_t * dialogTitle, const char16_t * text, char16_t * *username, char16_t * *password, const char16_t * checkMsg, bool *checkValue, bool *_retval)
{
	return PromptUsernameAndPassword(mDomWindow, dialogTitle, text, username, password, checkMsg, checkValue, _retval);
}

NS_IMETHODIMP CPromptService::Select(const char16_t * dialogTitle, const char16_t * text, uint32_t count, const char16_t * *selectList, int32_t *outSelection, bool *_retval)
{
	return Select(mDomWindow, dialogTitle, text, count, selectList, outSelection, _retval);
}


NS_IMETHODIMP
CPromptService::Alert(mozIDOMWindowProxy* aParent, const char16_t* aDialogTitle,
                        const char16_t* aDialogText)
{
 
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::AlertCheck(mozIDOMWindowProxy* aParent,
                             const char16_t* aDialogTitle,
                             const char16_t* aDialogText,
                             const char16_t* aCheckMsg, bool* aCheckValue)
{
    NS_ENSURE_ARG_POINTER(aCheckValue);

    return NS_OK;
}

NS_IMETHODIMP
CPromptService::Confirm(mozIDOMWindowProxy* aParent,
                          const char16_t* aDialogTitle,
                          const char16_t* aDialogText, bool* aConfirm)
{
	*aConfirm = PR_FALSE;
  
 

	int nRtn = 0; 
	if (nRtn == 0)
	{
		*aConfirm = PR_TRUE;
	}  
	return NS_OK;
}

NS_IMETHODIMP
CPromptService::ConfirmCheck(mozIDOMWindowProxy* aParent,
                               const char16_t* aDialogTitle,
                               const char16_t* aDialogText,
                               const char16_t* aCheckMsg,
                               bool* aCheckValue, bool* aConfirm)
{
    NS_ENSURE_ARG_POINTER(aCheckValue); 
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::ConfirmEx(mozIDOMWindowProxy* aParent,
                            const char16_t* aDialogTitle,
                            const char16_t* aDialogText,
                            PRUint32 aButtonFlags,
                            const char16_t* aButton0Title,
                            const char16_t* aButton1Title,
                            const char16_t* aButton2Title,
                            const char16_t* aCheckMsg, bool* aCheckValue,
                            PRInt32* aRetVal)
{ 
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::Prompt(mozIDOMWindowProxy* aParent, const char16_t* aDialogTitle,
                         const char16_t* aDialogText, char16_t** aValue,
                         const char16_t* aCheckMsg, bool* aCheckValue,
                         bool* aConfirm)
{ 
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::PromptUsernameAndPassword(mozIDOMWindowProxy* aParent,
                                            const char16_t* aDialogTitle,
                                            const char16_t* aDialogText,
											char16_t** aUsername,
											char16_t** aPassword,
                                            const char16_t* aCheckMsg,
                                            bool* aCheckValue,
                                            bool* aConfirm)
{ 
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::PromptPassword(mozIDOMWindowProxy* aParent,
                                 const char16_t* aDialogTitle,
                                 const char16_t* aDialogText,
								 char16_t** aPassword,
                                 const char16_t* aCheckMsg,
                                 bool* aCheckValue, bool* aConfirm)
{ 
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::Select(mozIDOMWindowProxy* aParent, const char16_t* aDialogTitle,
                         const char16_t* aDialogText, PRUint32 aCount,
                         const char16_t** aSelectList, PRInt32* outSelection,
                         bool* aConfirm)
{ 
    return NS_OK;
}
 