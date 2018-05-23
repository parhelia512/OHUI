/****************************************************************
OHUI  v52, developed with vs enterprise 2015 update3
whenever you meet any problem  about OHUI, please contact me. 
http://yaolixing.oltag.com/gn-A65A3AD3-97A4-4A3B-AFEB-EF4F36C814CC.html
QQ groups: 264695584
					------By Other Hill  2017.12
*****************************************************************/

#include "nsProfileDirServiceProvider.h"
#include "nsIFile.h"
#include "nsDirectoryServiceDefs.h" 
#include "nsAppDirectoryServiceDefs.h"
#include "nsISupportsUtils.h"
#include "nsISimpleEnumerator.h"
#include "nsIObserverService.h"
#include "nsXULAppAPI.h"
#include "nsArrayEnumerator.h"
#include "nsCOMArray.h"

#define PREFS_FILE_50_NAME           NS_LITERAL_CSTRING("prefs.js")
#define USER_CHROME_DIR_50_NAME      NS_LITERAL_CSTRING("chrome")
#define LOCAL_STORE_FILE_50_NAME     NS_LITERAL_CSTRING("localstore.rdf")
#define PANELS_FILE_50_NAME          NS_LITERAL_CSTRING("panels.rdf")
#define MIME_TYPES_FILE_50_NAME      NS_LITERAL_CSTRING("mimeTypes.rdf")
#define BOOKMARKS_FILE_50_NAME       NS_LITERAL_CSTRING("bookmarks.html")
#define DOWNLOADS_FILE_50_NAME       NS_LITERAL_CSTRING("downloads.rdf")
#define SEARCH_FILE_50_NAME          NS_LITERAL_CSTRING("search.rdf" )
#define STORAGE_FILE_50_NAME         NS_LITERAL_CSTRING("storage.sdb")


NS_IMPL_ISUPPORTS(nsProfileDirServiceProvider,
						nsIDirectoryServiceProvider, 
						nsIDirectoryServiceProvider2
						)

nsProfileDirServiceProvider::nsProfileDirServiceProvider(bool aNotifyObservers):
	mNotifyObservers(aNotifyObservers),
	mSharingEnabled(false)
{ 
}


nsProfileDirServiceProvider::~nsProfileDirServiceProvider()
{
}

void
nsProfileDirServiceProvider::SetGREDirs(nsIFile* greDir)
{
	mGREDir = greDir;
	mGREDir->Clone(getter_AddRefs(mGREBinDir));
#ifdef XP_MACOSX
	nsAutoCString leafName;
	mGREDir->GetNativeLeafName(leafName);
	if (leafName.Equals("Resources")) {
		mGREBinDir->SetNativeLeafName(NS_LITERAL_CSTRING("MacOS"));
	}
#endif
}

void
nsProfileDirServiceProvider::SetAppFile(nsIFile* appFile)
{
	mAppFile = appFile;
}

void
nsProfileDirServiceProvider::SetAppDir(nsIFile* appDir)
{
	mAppDir = appDir;
}

void
nsProfileDirServiceProvider::SetPluginDir(nsIFile* pluginDir)
{
	mPluginDir = pluginDir;
}

NS_IMETHODIMP
nsProfileDirServiceProvider::GetFile(const char* prop, bool* persistent,
	nsIFile* *result)
{
	nsIFile* domainDir = mProfileDir;

	if (mGREDir && !strcmp(prop, NS_GRE_DIR)) {
		*persistent = true;
		return mGREDir->Clone(result);
	}
	else if (mGREBinDir && !strcmp(prop, NS_GRE_BIN_DIR)) {
		*persistent = true;
		return mGREBinDir->Clone(result);
	}
	else if (mAppFile && !strcmp(prop, XRE_EXECUTABLE_FILE)) {
		*persistent = true;
		return mAppFile->Clone(result);
	}
	else if (mProfileDir &&!strcmp(prop, NS_APP_USER_PROFILE_50_DIR) )
	{
		*persistent = true;
		return domainDir->Clone(result);
	} 
	else if (mProfileDir && !strcmp(prop, NS_APP_PERMISSION_PARENT_DIR))
	{
		*persistent = true;
		nsCOMPtr<nsIFile> file;
		if (NS_FAILED(domainDir->Clone(getter_AddRefs(file))) ||
			NS_FAILED(file->AppendNative(NS_LITERAL_CSTRING("permissionDB"))))
			return NS_ERROR_FAILURE;

		file.forget(result);
		return NS_OK;
	}
	else if (mProfileDir && !strcmp(prop, NS_APP_CACHE_PARENT_DIR))
	{
		*persistent = true;
		nsCOMPtr<nsIFile> file; 
		if (NS_FAILED(domainDir->Clone(getter_AddRefs(file))) ||
			NS_FAILED(file->AppendNative(NS_LITERAL_CSTRING("cache"))))
			return NS_ERROR_FAILURE;

		file.forget(result);
		return NS_OK;
	}
	else if (mGREDir && !strcmp(prop, NS_APP_PREF_DEFAULTS_50_DIR)) 
	{
		nsCOMPtr<nsIFile> file;
		*persistent = true;
		if (NS_FAILED(mGREDir->Clone(getter_AddRefs(file))) ||
			NS_FAILED(file->AppendNative(NS_LITERAL_CSTRING("defaults"))) ||
			NS_FAILED(file->AppendNative(NS_LITERAL_CSTRING("pref"))))
			return NS_ERROR_FAILURE;
		file.forget(result);
		return NS_OK;
	}

	return NS_ERROR_FAILURE;
}

NS_IMETHODIMP
nsProfileDirServiceProvider::GetFiles(const char* prop, nsISimpleEnumerator* *result)
{
	if (mGREDir && !strcmp(prop, "ChromeML")) {
		nsCOMArray<nsIFile> dirs;

		nsCOMPtr<nsIFile> file;
		mGREDir->Clone(getter_AddRefs(file));
		file->AppendNative(NS_LITERAL_CSTRING("chrome"));
		dirs.AppendObject(file);

		nsresult rv = NS_GetSpecialDirectory(NS_APP_CHROME_DIR,
			getter_AddRefs(file));
		if (NS_SUCCEEDED(rv))
			dirs.AppendObject(file);

		return NS_NewArrayEnumerator(result, dirs);
	}
	else if (!strcmp(prop, NS_APP_PREFS_DEFAULTS_DIR_LIST)) {
		nsCOMArray<nsIFile> dirs;
		nsCOMPtr<nsIFile> appDir;
		bool exists;
		if (mAppDir &&
			NS_SUCCEEDED(mAppDir->Clone(getter_AddRefs(appDir))) &&
			NS_SUCCEEDED(appDir->AppendNative(NS_LITERAL_CSTRING("defaults"))) &&
			NS_SUCCEEDED(appDir->AppendNative(NS_LITERAL_CSTRING("preferences"))) &&
			NS_SUCCEEDED(appDir->Exists(&exists)) && exists) {
			dirs.AppendObject(appDir);
			return NS_NewArrayEnumerator(result, dirs);
		}
		return NS_ERROR_FAILURE;
	}
	else if (!strcmp(prop, NS_APP_PLUGINS_DIR_LIST)) {
		nsCOMArray<nsIFile> dirs;
		// Add the test plugin location passed in by the caller or through
		// runxpcshelltests.
		if (mPluginDir) {
			dirs.AppendObject(mPluginDir);
			// If there was no path specified, default to the one set up by automation
		}
		else {
			nsCOMPtr<nsIFile> file;
			bool exists;
			// We have to add this path, buildbot copies the test plugin directory
			// to (app)/bin when unpacking test zips.
			if (mGREDir) {
				mGREDir->Clone(getter_AddRefs(file));
				if (NS_SUCCEEDED(mGREDir->Clone(getter_AddRefs(file)))) {
					file->AppendNative(NS_LITERAL_CSTRING("plugins"));
					if (NS_SUCCEEDED(file->Exists(&exists)) && exists) {
						dirs.AppendObject(file);
					}
				}
			}
		}
		return NS_NewArrayEnumerator(result, dirs);
	}
	return NS_ERROR_FAILURE;
}

nsresult  nsProfileDirServiceProvider::Initialize()
{
	return NS_OK;
}

nsresult
nsProfileDirServiceProvider::InitProfileDir(nsIFile *profileDir)
{
	// Make sure our "Profile" folder exists.
	// If it does not, copy the profile defaults to its location.

	nsresult rv;
	bool exists;
	rv = profileDir->Exists(&exists);
	if (NS_FAILED(rv))
		return rv;

	if (!exists) {
		nsCOMPtr<nsIFile> profileDefaultsDir;
		nsCOMPtr<nsIFile> profileDirParent;
		nsAutoCString profileDirName;

		(void)profileDir->GetParent(getter_AddRefs(profileDirParent));
		if (!profileDirParent)
			return NS_ERROR_FAILURE;
		rv = profileDir->GetNativeLeafName(profileDirName);
		if (NS_FAILED(rv))
			return rv;
		
		rv = NS_GetSpecialDirectory(NS_APP_USER_PROFILE_50_DIR, getter_AddRefs(profileDefaultsDir));
		if (NS_FAILED(rv)) {
			return rv;
		}
		rv = profileDefaultsDir->CopyToNative(profileDirParent, profileDirName);
		if (NS_FAILED(rv)) {
			// if copying failed, lets just ensure that the profile directory exists.
			profileDirParent->AppendNative(profileDirName);
			rv = profileDirParent->Create(nsIFile::DIRECTORY_TYPE, 0700);
			if (NS_FAILED(rv))
				return rv;
		}

		rv = profileDir->SetPermissions(0700);
		if (NS_FAILED(rv))
			return rv;
	}
	else {
		bool isDir;
		rv = profileDir->IsDirectory(&isDir);

		if (NS_FAILED(rv))
			return rv;
		if (!isDir)
			return NS_ERROR_FILE_NOT_DIRECTORY;
	}

	return rv;
}

nsresult
nsProfileDirServiceProvider::SetProfileDir(nsIFile* aProfileDir,
	nsIFile* aLocalProfileDir)
{
	if (!aLocalProfileDir)
		aLocalProfileDir = aProfileDir;
	if (mProfileDir) {
		bool isEqual;
		if (aProfileDir &&
			NS_SUCCEEDED(aProfileDir->Equals(mProfileDir, &isEqual)) && isEqual) {
			NS_WARNING("Setting profile dir to same as current");
			return NS_OK;
		}
#ifdef MOZ_PROFILELOCKING
		mProfileDirLock->Unlock();
#endif
		UndefineFileLocations();
	}
	mProfileDir = aProfileDir;
	mLocalProfileDir = aLocalProfileDir;
	if (!mProfileDir)
		return NS_OK;

	nsresult rv = InitProfileDir(mProfileDir);
	if (NS_FAILED(rv))
		return rv;

	// Make sure that the local profile dir exists
	// we just try to create it - if it exists already, that'll fail; ignore
	// errors
	mLocalProfileDir->Create(nsIFile::DIRECTORY_TYPE, 0700);

#ifdef MOZ_PROFILELOCKING
	// Lock the non-shared sub-dir if we are sharing,
	// the whole profile dir if we are not.
	nsCOMPtr<nsIFile> dirToLock;
	if (mSharingEnabled)
		dirToLock = mNonSharedProfileDir;
	else
		dirToLock = mProfileDir;
	rv = mProfileDirLock->Lock(dirToLock, nullptr);
	if (NS_FAILED(rv))
		return rv;
#endif

	if (mNotifyObservers) {
		nsCOMPtr<nsIObserverService> observerService =
			do_GetService("@mozilla.org/observer-service;1");
		if (!observerService)
			return NS_ERROR_FAILURE;

		NS_NAMED_LITERAL_STRING(context, "startup");
		// Notify observers that the profile has changed - Here they respond to new profile
		observerService->NotifyObservers(nullptr, "profile-do-change", context.get());
		// Now observers can respond to something another observer did on "profile-do-change"
		observerService->NotifyObservers(nullptr, "profile-after-change", context.get());
	}

	return NS_OK;
}

nsresult
nsProfileDirServiceProvider::UndefineFileLocations()
{
	nsresult rv;

	nsCOMPtr<nsIProperties> directoryService =
		do_GetService(NS_DIRECTORY_SERVICE_CONTRACTID, &rv);
	NS_ENSURE_TRUE(directoryService, NS_ERROR_FAILURE);

	(void)directoryService->Undefine(NS_APP_PREFS_50_DIR);
	(void)directoryService->Undefine(NS_APP_PREFS_50_FILE);
	(void)directoryService->Undefine(NS_APP_USER_PROFILE_50_DIR);
	(void)directoryService->Undefine(NS_APP_USER_CHROME_DIR);
	(void)directoryService->Undefine(NS_APP_LOCALSTORE_50_FILE);
	(void)directoryService->Undefine(NS_APP_USER_PANELS_50_FILE);
	(void)directoryService->Undefine(NS_APP_USER_MIMETYPES_50_FILE);
	(void)directoryService->Undefine(NS_APP_DOWNLOADS_50_FILE);
	(void)directoryService->Undefine(NS_APP_SEARCH_50_FILE);

	return NS_OK;
}

nsresult
nsProfileDirServiceProvider::Register()
{
	nsCOMPtr<nsIDirectoryService> directoryService =do_GetService(NS_DIRECTORY_SERVICE_CONTRACTID);
	if (!directoryService)
		return NS_ERROR_FAILURE;
	return directoryService->RegisterProvider(this);
}

nsresult NS_NewProfileDirServiceProvider(bool aNotifyObservers,
	nsProfileDirServiceProvider** aProvider)
{
	NS_ENSURE_ARG_POINTER(aProvider);
	*aProvider = nullptr;

	nsProfileDirServiceProvider *prov = new nsProfileDirServiceProvider(aNotifyObservers);
	if (!prov)
		return NS_ERROR_OUT_OF_MEMORY;
	nsresult rv = prov->Initialize();
	if (NS_FAILED(rv)) {
		delete prov;
		return rv;
	}
	NS_ADDREF(*aProvider = prov);
	return NS_OK;
}
