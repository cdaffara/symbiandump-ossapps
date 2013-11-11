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
* Description:  Used as proxy for phone application to get and observe 
*                the change of the variable.state of the phone.
*
*/


#ifndef __CPHONEPUBSUBPROXY_H
#define __CPHONEPUBSUBPROXY_H

//  INCLUDES
#include <coemain.h>
#include <e32property.h>
#include "mphonepubsubobserver.h"

// FORWARD DECLARATIONS
class CPhonePublishSubscriberAO;
class CIdle;

/**
*  CPhonePubSubProxy is an API used as proxy for phone application to get/set
*  and observe variable states.
*
*  How to get some setting?
*  ---------------------------
*  example code:
*
*  TUid category = KUidSystemCategory
*  TUInt key = KPSUidAutolockStatusValue
*  Tint value = CPhonePubSubProxy::Instance()->GetValue( category, key ); 
*  
*  uid and states can be found in PSVariables.h
*  
*  How to set any settings
*  --------------------------------------
*  example code:
*
*  TUid category = KUidSystemCategory
*  TUInt key = KPSUidAutolockStatusValue
*  TInt value = EPSAutolockOn;
*
*  CPhonePubSubProxy::Instance()->ChangePropertyValue( category, key, value );
*
*  How to observe any settings
*  --------------------------------------
*  example code:
* 
*  class CPhoneSomeManager : private MPhonePubSubObserver
*  {
*  
*  virtual void HandlePropertyChangedL( const TUid& aCategory, 
*            const TUint aKey, const TInt aValue );
*  }
*
*  CPhoneSomeManager::SomeFunctionL
*      {
*      CPhonePubSubProxy::Instance()->NotifyChangeL( cat1, key1, this );
*      CPhonePubSubProxy::Instance()->NotifyChangeL( cat2, key2, this );
*      }
*  
*  CPhoneSomeManager::HandlePropertyChangedL( 
*      const TUid& aCategory, 
*      const TUint aKey, 
*      const TInt aValue )
*      {
*      switch( aCategory )
*          {
*          case KUidSomeCategory1:
*               .... //do something for the category i.e. look at value
*          case KUidSomeCategory2:
*               .... //do something for the category i.e. look at value
*          default:
*               .... //panic in debug
*          }
*      }
*
*  The CancelAllNotifies() should be always called before destroy the object
*  CPhoneSomeManager::~CPhoneSomeManager()
*      {
*      CPhonePubSubProxy::Instance()->CancelAllNotifies( this );
*      }
*/

// CLASS DECLARATION

/**
*  Used as proxy for phone application to get and observe the change of the
*  state of the phone.
*/
class CPhonePubSubProxy : public CCoeStatic, public MPhonePubSubObserver
    {
    public:  // Constructors and destructor
        
        /**
        * First call initializes the singleton object. Subsequent calls 
        * return instance.
        * @return the created instance.
        */
        IMPORT_C static CPhonePubSubProxy* Instance();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhonePubSubProxy();

    public: // New Functions 

        /**
        * Cancel all requests by the observer. This function should always be 
        * call before an object is destroyed. During shutdown, this singleton
        * may be deleted before the classes that observe it. If this happens, the
        * routine will just return since of requests have already been cancelled.
        * @param aObserver
        */
        IMPORT_C static void CancelAllNotifications( 
            MPhonePubSubObserver* aObserver );
        
        /**
        * Get the property value of particular category key.
        *
        * @param aCategory the category for the key.
        * @param aKey the property key.
        * @return current property value.
        */
        IMPORT_C TInt Value( const TUid& aCategory, const TUint aKey );

        /**
        * Notify the change of state to particular varible.
        *
        * @param aUid Specify the uid of the particular varible.
        * @param aObserver The observer to receive the notification.
        */
        IMPORT_C void NotifyChangeL( 
            const TUid& aCategory, 
            const TUint aKey,
            MPhonePubSubObserver* aObserver );

        /**
        * Change a specific property value.
        *
        * @param aCategory Category of property.
        * @param aKey Property key to be changed
        * @param aValue New property value.
        */
        IMPORT_C void ChangePropertyValue( 
            const TUid& aCategory,
            const TUint aKey,
            const TInt aValue );

    private:
        /**
        * Two-phased constructor.
        * @return new instance.
        */
        static CPhonePubSubProxy* NewL();

        /**
        * C++ constructor.
        */
        CPhonePubSubProxy();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

    private:
        
        /**
        * Cancel all request by the observer. 
        * @param aObserver
        */
        void CancelAllObserverNotifies( 
            MPhonePubSubObserver* aObserver );
        
        /**
        * function used to forward change events to observers.
        * @param aCategory Category of property
        * @param aKey Property key that is changed
        * @param aValue New property value
        */
        void HandlePropertyChangedL( 
            const TUid& aCategory, 
            const TUint aKey, 
            const TInt aValue );
        
    private:    // Data

        // Observer elements.
        class TPubSubObserverTag
            {
           public: 
                // Unique property category.
                TUid iCategory; 
                // Property key
                TUint iKey;
                // Observer.
                MPhonePubSubObserver* iObserver;
            };

        // Owned array of observers.
        CArrayFixFlat<TPubSubObserverTag>* iObserverArray;

        // Owned array of publish subscribers.
        CArrayPtrFlat<CPhonePublishSubscriberAO>* iPublishSubscriberArray;

    };

#endif      // __CPHONEPUBSUBPROXY_H
            
// End of File
