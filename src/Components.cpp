
#include "nsIWindowWatcher.h"
#include "nsIWebBrowserChrome.h"
#include "nsServiceManagerUtils.h"  //do_GetService

#include "MozUtil.h"   //CreateBrowserWindow

#include "PromptService.h"
#include "OhuiApp.h"
//////////////////////////////////////////////////////////////////////////
// app component registration
#include "mozilla/ModuleUtils.h"
#include "mozilla/GenericFactory.h"
#include "nsICategoryManager.h"
#include "nsIComponentManager.h"
#include "nsIComponentRegistrar.h"
#include "nsServiceManagerUtils.h"
#include "nsIComponentManager.h"
#include "nsEmbedCID.h"  // NS_PROMPTSERVICE_CONTRACTID

NS_GENERIC_FACTORY_CONSTRUCTOR(CPromptService) 


static const mozilla::Module::CIDEntry kWndPromptServiceCIDs[] = {
	{ &kNS_PROMPTSERVICE_CID, true/*false*/, NULL, CPromptServiceConstructor },
	{ NULL }
};


static const mozilla::Module::ContractIDEntry kWndPromptServiceContracts[] = {
	{ NS_PROMPTSERVICE_CONTRACTID, &kNS_PROMPTSERVICE_CID },
	{ NULL }
};


static const mozilla::Module::CategoryEntry kWndPromptServiceCategories[] = {
	{ "Prompt Service", "@mozilla.org/embedcomp/prompt-service;1", NS_PROMPTSERVICE_CONTRACTID },
	{ NULL }
};

static const mozilla::Module kWndPromptServiceModule = 
{
	mozilla::Module::kVersion,
	kWndPromptServiceCIDs,
	kWndPromptServiceContracts,
	kWndPromptServiceCategories
};

NSMODULE_DEFN(nsWndPromptServiceModule) = &kWndPromptServiceModule;
NS_IMPL_MOZILLA192_NSGETMODULE(&kWndPromptServiceModule)

nsresult COhuiApp::OverrideComponents()
{
	nsCOMPtr<nsIComponentRegistrar> compReg;
	nsresult rv = NS_GetComponentRegistrar(getter_AddRefs(compReg));
	NS_ENSURE_SUCCESS(rv, rv);

	nsCOMPtr<nsIComponentManager> compManager;
	rv = NS_GetComponentManager(getter_AddRefs(compManager));
	NS_ENSURE_SUCCESS(rv, rv);

	nsCOMPtr<nsIComponentRegistrar> cr;
	rv = NS_GetComponentRegistrar(getter_AddRefs(cr));
	NS_ENSURE_SUCCESS(rv, rv);

	nsCOMPtr<nsIFactory> componentFactory = 
		new mozilla::GenericFactory(kWndPromptServiceModule.mCIDs->constructorProc);
	 
	rv = cr->RegisterFactory(*kWndPromptServiceModule.mCIDs->cid
		, kWndPromptServiceModule.mCategoryEntries->value
		, kWndPromptServiceModule.mContractIDs->contractid
		, componentFactory);

	bool bRegisted = true;
	compReg->IsCIDRegistered(*kWndPromptServiceModule.mCIDs->cid, &bRegisted);

	return rv;
}
