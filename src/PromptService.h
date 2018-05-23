/****************************************************************
OHUI  v52, developed with vs enterprise 2015 update3
whenever you meet any problem  about OHUI, please contact me. 
http://yaolixing.oltag.com/gn-A65A3AD3-97A4-4A3B-AFEB-EF4F36C814CC.html
QQ groups: 264695584
					------By Other Hill  2017.12
*****************************************************************/

#ifndef PROMPT_SERVICE_H_566754677
#define PROMPT_SERVICE_H_566754677

#include "nsIPrompt.h"
#include "nsIPromptService.h"
#include "nsIPromptService2.h"
#include "nsIWindowWatcher.h"
#include "nsEmbedCID.h"

#include "nsStringGlue.h"
#include "nsIFactory.h" 


class nsIDOMWindow;

#define OHUI_PROMPTSERVICE_CONTRACTID "@oltag.com/OHUI/prompt-service;1"

#define NS_PROMPTSERVICE_CID \
{0xa2112d6a, 0x0e28, 0x421f, {0xb4, 0x6a, 0x25, 0xc0, 0xb3, 0x8, 0xcb, 0xd0}}


static NS_DEFINE_CID(kPromptServiceCID, NS_PROMPTSERVICE_CID);

#include "nsIPromptFactory.h"

class PromptFactory:public nsIPromptFactory, public nsIFactory
{
    NS_DECL_ISUPPORTS
	NS_DECL_NSIPROMPTFACTORY 
	NS_DECL_NSIFACTORY

protected:
	virtual ~PromptFactory() {};

};

class PromptFactoryFactory:public PromptFactory
{ 
};


class CPromptService : public nsIPromptFactory, public nsIPromptService, public nsIPrompt
{
private:
    virtual ~CPromptService();

public:
    CPromptService();


    NS_DECL_ISUPPORTS
	NS_DECL_NSIPROMPTFACTORY
    NS_DECL_NSIPROMPTSERVICE
	NS_DECL_NSIPROMPT
 
protected:
	nsCOMPtr<mozIDOMWindowProxy> mDomWindow;
};

extern "C" NS_EXPORT nsresult NS_NewPromptServiceFactory(nsIFactory** aFactory);
typedef nsresult ( *MakeFactoryType)(nsIFactory** p);
#define kPromptServiceFactoryFuncName "NS_NewPromptServiceFactory"

 
#define kPromptServiceInitFuncName "InitPromptService"

#endif
