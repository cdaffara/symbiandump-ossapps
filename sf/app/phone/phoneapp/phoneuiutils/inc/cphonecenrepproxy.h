/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Proxy for the connection to central repository.
*
*/


#ifndef __CPHONECENREPPROXY_H
#define __CPHONECENREPPROXY_H

//  INCLUDES
#include <coemain.h>
#include <e32base.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>

// CONSTANTS
class MPhoneCenRepObserver;
class CPhoneCenRepEventHandler;

// FORWARD DECLARATIONS
class TPhoneCmdParamAppInfo;

/**
*  CPhoneCenRepProxy is an API used as proxy for phone application to get/set
*  and observe central repository settings.
*
*  How to get some setting?
*  ---------------------------
*  example code:
*  TBuf<100> value;
*  CPhoneCenRepProxy::Instance()->GetString( uid, key, value ); 
*  
*  uid and key should be retrieved from <Module Name>InternalCRKeys.h
*  
*  How to set any settings
*  --------------------------------------
*  example code:
*  TInt value = 10;
*  CPhoneCenRepProxy::Instance()->SetInt( uid, key, value );
*
*  How to observe any settings
*  --------------------------------------
*  example code:
* 
*  class CPhoneSomeManager : private MPhoneCenRepObserver
*      {
*      ...
*      virtual void HandleCenRepChangeL( 
*          const TUid& aUid, 
*          const TUint aId );
*      ....
*      }
*
*  CPhoneSomeManager::SomeFunctionL
*      {
*      CPhoneCenRepProxy::Instance()->NotifyChangeL( uid1, id1, this );
*
*      CPhoneCenRepProxy::Instance()->NotifyChangeL( uid2, id2, this );
*      }
*  
*  CPhoneSomeManager::HandleCenRepChangeL( 
*      const TUid& aUid,
*      const TUint aId )
*      {
*      switch( aId )
*          {
*          case ESetting1:
*              .... //do something
*          case ESetting2:
*              .... //do something
*          default:
*              .... //panic in debug
*          }
*      }
*
*  //The CancelAllNotifies() should be always called before destroy the object
*  CPhoneSomeManager::~CPhoneSomeManager()
*      {
*      CPhoneCenRepProxy::Instance()->CancelAllNotifies( this );
*      }
*
*/


// CLASS DECLARATION

class CPhoneCenRepProxy : 
    public CCoeStatic 
    {
    public:  // Constructors and destructor
        
        /**
        * First call initializes the singleton object. Subsequent calls return
        * created instance.
        * @return the created instance.
        */
        IMPORT_C static CPhoneCenRepProxy* Instance();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneCenRepProxy();

    public: // New Functions

        /**
        * Cancel all requests by the observer. This function should always be 
        * call before an object is destroyed. During shutdown, this singleton
        * may be deleted before the classes that observe it. If this happens, the
        * routine will just return since of requests have already been cancelled.
        * @param aObserver
        */
        IMPORT_C static void CancelAllNotifies( 
            MPhoneCenRepObserver* aObserver );
        
        /**
        * Set integer value of setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aValue the value associated with the central repository ID
        */
        IMPORT_C TInt SetInt( 
            const TUid& aUid, 
            const TUint aId,
            const TInt aValue );
        
        /**
        * Set string value of setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aValue the value associated with the central repository ID
        * @return refer to SharedDataClient.h for detail explanation
        */
        IMPORT_C TInt SetString( 
            const TUid& aUid, 
            const TUint aId,
            const TDesC& aValue );
            
        /**
        * Set real value of setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aValue the value associated with the central repository ID
        */
        IMPORT_C TInt SetReal( 
            const TUid& aUid, 
            const TUint aId,
            const TReal aValue );
        
        /**
        * Get integer value of setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aValue the value associated with the central repository ID
        */
        IMPORT_C TInt GetInt( 
            const TUid& aUid, 
            const TUint aId, 
            TInt& aValue ) const;

        /**
        * Get string value of setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aValue the value associated with the central repository ID
        */
        IMPORT_C TInt GetString( 
            const TUid& aUid, 
            const TUint aId, 
            TDes& aValue ) const;
            
        /**
        * Get real value of setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aValue the value associated with the central repository ID
        */
        IMPORT_C TInt GetReal( 
            const TUid& aUid, 
            const TUint aId, 
            TReal& aValue ) const;
            
        /**
        * Notify the change of the setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aObserver The pointer to the observer to handle the change 
        *     notification
        */
        IMPORT_C void NotifyChangeL( 
            const TUid& aUid, 
            const TUint aId, 
            MPhoneCenRepObserver* aObserver );

        /**
        * Cancel request. If you don't want to receive notification of the 
        * setting you issued before, call this function.
        * @param aObserver The observer that have issued request before
        * @param aId The setting which is to be canceled
        */
        IMPORT_C void CancelNotify( 
            MPhoneCenRepObserver* aObserver,
            const TUint aId );
        
        /**
        * Return boolean value that states whether or not 
        * the indicated feature is supported.
        * @param aFeatureId the feature that is inspected (see values from 
        *        TelephonyVariant.hrh)
        * @return whether or not the feature is supported.
        */
        IMPORT_C TBool IsTelephonyFeatureSupported( const TInt aFeatureId );

        /**
        * Return boolean value that states whether or not 
        * the indicated feature is supported.
        * @param aFeatureId the feature that is inspected (see values from 
        *        PhoneUIVariant.hrh)
        * @return whether or not the feature is supported.
        */
        IMPORT_C TBool IsPhoneUIFeatureSupported( const TInt aFeatureId );
        
        /**
         * Get keys from given value range
         * @param aUid identifing the central repository UID.
         * @param aPartial partial key to be found
         * @param aMask mask for partial key
         * @param aValues reference to array object for found keys
         * @return KErrNone if successful, system wide error code otherwise.
         */
        IMPORT_C TInt Find(
                const TUid& aUid,
                const TUint32 aPartial, 
                const TUint32 aMask,
                RArray<TUint32>& aValues ) const;
        
        /**
        * Fetches values from central repository
        * @param aAppInfo reference to object containing info for 
        * application launching
        * @param aCode ascii value of the pressed key
        * @param aAppParam application parameter
        * @param aValuesFetched ETrue if values fetched ok
        */
        IMPORT_C void FetchValuesFromCenRepL( TPhoneCmdParamAppInfo& aAppInfo, 
                                        TKeyCode aCode,
                                        HBufC8* aAppParam, TBool& aValuesFetched );


    public: // New methods

        /**
        * Handle notifications from central repository observers
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        */
        void HandleNotify( 
            const TUid& aUid, 
            const TUint aId );
        
    private:

        /**
        * Two-phased constructor.
        * @return new instance.
        */
        static CPhoneCenRepProxy* NewL();

protected:        
        /**
        * C++ default constructor.
        */
        CPhoneCenRepProxy();        
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Find the observer for specific setting on aUid and aId
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aFromIndex find next observer interested in this setting after 
        *               aIndex
        * @return The index of the observer if it's found and KErrNotFound if 
        *         not
        */
        TInt FindByUidId( 
            const TUid& aUid, 
            const TUint aId, 
            TInt aFromIndex = 0 );
        
        /**
        * Find the specific Observer
        * @param aObserver The pointer of the observer
        * @param aId central repository ID.
        * @return The index in iObserverArray. KErrNotFound if can't find the 
                  specific observer
        */
        TInt FindByObserverId( 
            const MPhoneCenRepObserver* aObserver,
            const TUint aId );

        /**
        * Finds observer by uid, key and observer.
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aObserver The pointer of the observer
        */
        TInt FindByUidIdObserver( 
            const TUid& aUid, 
            const TUint aId, 
            MPhoneCenRepObserver* aObserver );
            
        /**
        * Returns Telephony variant read-only data.
        * @return error code.
        */
        TInt GetTelephonyVariantData();  

        /**
        * Returns PhoneUI variant read-only data.
        * @return error code.
        */
        TInt GetPhoneUIVariantData();  

        /**
        * Cancel all request by the observer. 
        * @param aObserver
        */
        void CancelAllObserverNotifies( MPhoneCenRepObserver* aObserver );
        
        /**
        * Cancel all requests from all observers.
        */
        void CancelAllNotifies();
        
    private:    // Data

        // Defines information of one observer.
        class TCenRepObserverTag
            {
            public:
                TUid iUid;
                TUint iId;
                MPhoneCenRepObserver* iObserver;
            };

        // Owned array of observers.
        CArrayFixFlat<TCenRepObserverTag>* iObserverArray;

        // Central Repository event handlers.
        CPhoneCenRepEventHandler* iCenRepAccessoryEventHandler;
        CPhoneCenRepEventHandler* iCenRepNetworkEventHandler;
        CPhoneCenRepEventHandler* iCenRepThemesEventHandler;
        CPhoneCenRepEventHandler* iCenRepLanguageEventHandler;
        CPhoneCenRepEventHandler* iCenRepQwertyEventHandler;
        CPhoneCenRepEventHandler* iCenRepKDRMHelperEventHandler;

        //Local Telephony variant read-only data.
        TInt iTelephonyVariantReadOnlyValues;

        //Local PhoneUI variant read-only data.
        TInt iPhoneUIVariantReadOnlyValues;
    };

#endif      // __CPHONECENREPPROXY_H
            
// End of File
