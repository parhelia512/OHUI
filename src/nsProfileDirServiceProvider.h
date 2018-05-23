/****************************************************************
OHUI  v52, developed with vs enterprise 2015 update3
whenever you meet any problem  about OHUI, please contact me. 
http://yaolixing.oltag.com/gn-A65A3AD3-97A4-4A3B-AFEB-EF4F36C814CC.html
QQ groups: 264695584
					------By Other Hill  2017.12
*****************************************************************/

#pragma once
// Interfaces Needed
#include "nsIDirectoryService.h"
#include "nsIFile.h"

#include "nsCOMPtr.h"
#include "nsDirectoryServiceUtils.h"
#include "nsComponentManagerUtils.h"
#include "nsServiceManagerUtils.h"

#ifdef MOZILLA_INTERNAL_API
#include "nsString.h"
#else
#include "nsEmbedString.h"
#endif
 

#define NS_PROFILEDIRECTORYSERVICEPROVIDER_IID_STR "38C06941-B169-46E3-A04E-3EAC09A85F90"
 
#define NS_PROFILEDIRECTORYSERVICEPROVIDER_IID \
{ 0x38c06941, 0xb169, 0x46e3, \
	{ 0xa0, 0x4e, 0x3e, 0xac, 0x9, 0xa8, 0x5f, 0x90 } }


class nsProfileDirServiceProvider : public nsIDirectoryServiceProvider2
{
	NS_DECL_ISUPPORTS
	NS_DECL_NSIDIRECTORYSERVICEPROVIDER
	NS_DECL_NSIDIRECTORYSERVICEPROVIDER2

	friend nsresult NS_NewProfileDirServiceProvider(bool, nsProfileDirServiceProvider**);
protected:
	nsProfileDirServiceProvider(bool aNotifyObservers = true);
	virtual                 ~nsProfileDirServiceProvider();

	nsresult                 Initialize();
	nsresult                 InitProfileDir(nsIFile* profileDir);
	nsresult                 UndefineFileLocations();

	nsCOMPtr<nsIFile>        mLocalProfileDir; 
	bool                     mNotifyObservers;
private:
	nsCOMPtr<nsIFile> mProfileDir;
	nsCOMPtr<nsIFile> mGREDir;//
	nsCOMPtr<nsIFile> mGREBinDir;
	nsCOMPtr<nsIFile> mAppDir;
	nsCOMPtr<nsIFile> mPluginDir;
	nsCOMPtr<nsIFile> mAppFile;
	bool              mSharingEnabled;

public:
	NS_DECLARE_STATIC_IID_ACCESSOR(NS_PROFILEDIRECTORYSERVICEPROVIDER_IID)

	/**
	* Register
	*
	* Convenience method to register the provider with directory service.
	* The service holds strong references to registered providers so consumers
	* don't need to hold a reference to this object after calling Register().
	*/
	virtual nsresult         Register();

	/**
	* SetProfileDir
	*
	* @param aProfileDir   The directory containing the profile files.
	*                      It does not need to exist before calling this
	*                      method. If it does not, it will be created and
	*                      defaults will be copied to it.
	* @param aLocalProfileDir
	*                      Directory for local profile data, e.g. Cache.
	*                      If null, aProfileDir will be used for this purpose.
	*/

	virtual nsresult        SetProfileDir(nsIFile* aProfileDir,
		nsIFile* aLocalProfileDir = nullptr);

	// The platform resource folder
	void SetGREDirs(nsIFile* greDir);
	void ClearGREDirs() {
		mGREDir = (nsIFile*)nullptr;
		mGREBinDir = (nsIFile*)nullptr;
	}
	// The application resource folder
	void SetAppDir(nsIFile* appFile);
	void ClearAppDir() { mAppDir = (nsIFile*)nullptr; }
	// The app executable
	void SetAppFile(nsIFile* appFile);
	void ClearAppFile() { mAppFile = (nsIFile*)nullptr; }
	// An additional custom plugin dir if specified
	void SetPluginDir(nsIFile* pluginDir);
	void ClearPluginDir() { mPluginDir = (nsIFile*)nullptr; }
};
NS_DEFINE_STATIC_IID_ACCESSOR(nsProfileDirServiceProvider, NS_PROFILEDIRECTORYSERVICEPROVIDER_IID)

/**
* Global method to create an instance of nsProfileDirServiceProvider
*
* @param aNotifyObservers    If true, will send out profile startup
*                            notifications when the profile directory is set.
*                            See notifications.txt
*/

nsresult NS_NewProfileDirServiceProvider(bool aNotifyObservers,
	nsProfileDirServiceProvider** aProvider);
