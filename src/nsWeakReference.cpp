/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// nsWeakReference.cpp

#include "mozilla/Attributes.h"

#include "nsWeakReference.h"
#include "nsCOMPtr.h"

///run/media/lixing/20170318_4TB/src/browser/firefox/firefox-52.0.2/src/xpcom/glue/nsWeakReference.cpp
class nsWeakReference  : public nsIWeakReference
  {
    public:
    // nsISupports...
      NS_DECL_ISUPPORTS

  // nsIWeakReference...
  NS_DECL_NSIWEAKREFERENCE
  virtual size_t SizeOfOnlyThis(mozilla::MallocSizeOf aMallocSizeOf) const override;

    private:
      friend class nsSupportsWeakReference;

      nsWeakReference( nsSupportsWeakReference* referent )
          : mReferent(referent)
          // ...I can only be constructed by an |nsSupportsWeakReference|
        {
          // nothing else to do here
        }

      ~nsWeakReference()
           // ...I will only be destroyed by calling |delete| myself.
        {
          if ( mReferent )
            mReferent->NoticeProxyDestruction();
        }

      void
      NoticeReferentDestruction()
          // ...called (only) by an |nsSupportsWeakReference| from _its_ dtor.
        {
          mReferent = 0;
        }

      nsSupportsWeakReference*  mReferent;
  };



 nsIWeakReference*  // or else |already_AddRefed<nsIWeakReference>|
NS_GetWeakReference( nsISupports* aInstancePtr, nsresult* aErrorPtr )
  {
    nsresult status;

    nsIWeakReference* result = nullptr;

    if ( aInstancePtr )
      {
        nsCOMPtr<nsISupportsWeakReference> factoryPtr = do_QueryInterface(aInstancePtr, &status);
        if ( factoryPtr )
          {
            status = factoryPtr->GetWeakReference(&result);
          }
        // else, |status| has already been set by |do_QueryInterface|
      }
    else
      status = NS_ERROR_NULL_POINTER;

    if ( aErrorPtr )
      *aErrorPtr = status;
    return result;
  }
  

nsresult
nsQueryReferent::operator()( const nsIID& aIID, void** answer ) const
  {
    nsresult status;
    if ( mWeakPtr )
      {
        if ( NS_FAILED(status = mWeakPtr->QueryReferent(aIID, answer)) )
          *answer = 0;
      }
    else
      status = NS_ERROR_NULL_POINTER;

    if ( mErrorPtr )
      *mErrorPtr = status;
    return status;
  }
  
 nsresult
nsSupportsWeakReference::GetWeakReference( nsIWeakReference** aInstancePtr )
  {
    if ( !aInstancePtr )
      return NS_ERROR_NULL_POINTER;

    if ( !mProxy )
      mProxy = new nsWeakReference(this);
    *aInstancePtr = mProxy;

    nsresult status;
    if ( !*aInstancePtr )
      status = NS_ERROR_OUT_OF_MEMORY;
    else
      {
        NS_ADDREF(*aInstancePtr);
        status = NS_OK;
      }

    return status;
  }

NS_IMPL_ISUPPORTS(nsWeakReference, nsIWeakReference)


NS_IMETHODIMP
nsWeakReference::QueryReferent( const nsIID& aIID, void** aInstancePtr )
  {
    return mReferent ? mReferent->QueryInterface(aIID, aInstancePtr) : NS_ERROR_NULL_POINTER;
  }

size_t
nsWeakReference::SizeOfOnlyThis(mozilla::MallocSizeOf aMallocSizeOf) const
{
  return aMallocSizeOf(this);
}
void
nsSupportsWeakReference::ClearWeakReferences()
	{
		if ( mProxy )
			{
				mProxy->NoticeReferentDestruction();
				mProxy = 0;
			}
	}

/**/