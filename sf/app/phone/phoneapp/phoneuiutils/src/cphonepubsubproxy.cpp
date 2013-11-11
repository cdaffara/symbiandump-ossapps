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
* Description:  Publish & Subscribe proxy.
*
*/


// INCLUDE FILES
#include <e32base.h>     // TPriority::EPriorityHigh=20
#include <PSVariables.h>
#include <telinternalpskeys.h>
#include <telephonydomainpstypes.h>
#include <btengdomainpskeys.h>

#include <startupdomainpskeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <hwrmdomainpskeys.h>

#include  "phoneui.pan"
#include  "phoneconstants.h"
#include  "phonelogger.h"

#include  "cphonepubsubproxy.h"
#include  "cphonepublishsubscriberao.h"

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CPhonePubSubProxy::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhonePubSubProxy* CPhonePubSubProxy::Instance()
    {
    CPhonePubSubProxy* instance = static_cast<CPhonePubSubProxy*> 
        ( CCoeEnv::Static ( KUidPubSubProxySingleton ) );
    
    if ( !instance )
        {
        TRAPD( err, instance = CPhonePubSubProxy::NewL() );
        if ( err )
            {
            Panic( EPhoneUtilsCouldNotCreateSingleton );    
            }
        }
    return instance;
    }

// ---------------------------------------------------------
// CPhonePubSubProxy::CancelAllNotifications
// Cancel the notification if the singleton still exists.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPhonePubSubProxy::CancelAllNotifications( 
    MPhonePubSubObserver* aObserver )
    {
    CPhonePubSubProxy* instance = static_cast<CPhonePubSubProxy*> 
        ( CCoeEnv::Static( KUidPubSubProxySingleton ) );
    
    // Ignore the call since the singleton has already been destroyed and the
    // notifications removed
    if ( instance )
        {
        instance->CancelAllObserverNotifies( aObserver );
        }
    }


// ---------------------------------------------------------
// CPhonePubSubProxy::NewL
// ---------------------------------------------------------
//
CPhonePubSubProxy* CPhonePubSubProxy::NewL()
    {
    CPhonePubSubProxy* self = new (ELeave) CPhonePubSubProxy();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CPhonePubSubProxy::~CPhonePubSubProxy
// ---------------------------------------------------------
//
EXPORT_C CPhonePubSubProxy::~CPhonePubSubProxy()
    {
    if ( iPublishSubscriberArray )
        {
        iPublishSubscriberArray->ResetAndDestroy();
        }
    delete iPublishSubscriberArray;
    delete iObserverArray;
    }

// ---------------------------------------------------------
// CPhonePubSubProxy::CPhonePubSubProxy
// ---------------------------------------------------------
//
CPhonePubSubProxy::CPhonePubSubProxy() :
    CCoeStatic( KUidPubSubProxySingleton, EThread )
    {
    }

// ---------------------------------------------------------
// CPhonePubSubProxy::ConstructL
// ---------------------------------------------------------
//
void CPhonePubSubProxy::ConstructL()
    {
    iObserverArray = new ( ELeave ) CArrayFixFlat< TPubSubObserverTag >( 
        KPhonePubSubProxyObserverArrayGranularity );

    iPublishSubscriberArray = new ( ELeave ) 
        CArrayPtrFlat< CPhonePublishSubscriberAO >( 
            KPhonePubSubArrayGranularity );
    }

// ---------------------------------------------------------
// CPhonePubSubProxy::Value
// ---------------------------------------------------------
//
EXPORT_C TInt CPhonePubSubProxy::Value( 
    const TUid& aCategory,
    const TUint aKey )
    {
    TInt err( KErrNone );
    TInt value( 0 );

    err = RProperty::Get( aCategory, aKey, value );
    if ( err != KErrNone )
        {
        value = err;

        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhonePubSubProxy::GetValue error: %d", 
            err );
        }

    return value;
    }

// ---------------------------------------------------------
// CPhonePubSubProxy::NotifyChangeL
// ---------------------------------------------------------
//
EXPORT_C void CPhonePubSubProxy::NotifyChangeL( 
    const TUid& aCategory,
    const TUint aKey, 
    MPhonePubSubObserver* aObserver )
    {
    __ASSERT_DEBUG( aObserver, Panic( EPhoneUtilsParameterNotInitialized ) );
    __LOGMETHODSTARTEND(EPhoneUIUtils, "CPhonePubSubProxy::NotifyChangeL() ");

    // make sure this a new item in iObserverArray
    TBool requestIssued = EFalse;
    TBool sameRequestIssuedByObserver = EFalse;
    TInt count = iObserverArray->Count();

    // check if the request has already been issued
    for ( TInt i = 0; i < count && !requestIssued; i++ ) 
        {
        const TPubSubObserverTag& observerTag = iObserverArray->At( i );
        if ( ( observerTag.iCategory == aCategory ) &&
            ( observerTag.iKey == aKey ) )
            {
            requestIssued = ETrue;
            if ( observerTag.iObserver == aObserver )
                {
                sameRequestIssuedByObserver = ETrue;
                }
            }
        }

    // make sure the same request hasn't been issued by this observer
    if ( !sameRequestIssuedByObserver )
        {
        //add new observer to the array
        TPubSubObserverTag newObserver;
        newObserver.iCategory = aCategory;
        newObserver.iKey = aKey;
        newObserver.iObserver = aObserver;
        iObserverArray->AppendL( newObserver );
        }

    //make sure the request hasn't been issued before
    if ( !requestIssued )
        {
        //issue a new request
        CPhonePublishSubscriberAO* publishSubscriber = 
            CPhonePublishSubscriberAO::NewL( this, aCategory, aKey );
    
        CleanupStack::PushL( publishSubscriber );
        iPublishSubscriberArray->AppendL( publishSubscriber );
        CleanupStack::Pop( publishSubscriber );
        
        //subscribe for property change notications
        publishSubscriber->Subscribe();
        }
    }

// ---------------------------------------------------------
// CPhonePubSubProxy::ChangePropertyValue
// ---------------------------------------------------------
//
EXPORT_C void CPhonePubSubProxy::ChangePropertyValue( 
    const TUid& aCategory,
    const TUint aKey, 
    const TInt aValue )
    {
    TInt err( KErrNone );

    err = RProperty::Set( aCategory, aKey, aValue );
    if ( err != KErrNone )
        {
        __PHONELOG1( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhonePubSubProxy::ChangePropertyValue error: %d", 
            err );
        }
    }

// ---------------------------------------------------------
// CPhonePubSubProxy::HandlePropertyChangedL
// ---------------------------------------------------------
//
void CPhonePubSubProxy::HandlePropertyChangedL( 
    const TUid& aCategory, 
    const TUint aKey,
    const TInt aValue )
    {
    TInt count = iObserverArray->Count();
    for ( TInt i = 0; i < count; )
        {
        // Take observer information.
        TPubSubObserverTag observerTag = iObserverArray->At( i );
        
        if ( ( observerTag.iCategory == aCategory ) && 
            ( observerTag.iKey == aKey ) ) 
            {
            observerTag.iObserver->HandlePropertyChangedL( 
                aCategory, aKey, aValue );
            }

        // Update count variable - might have been modified by the
        // HandlePropertyChangedL call.
        count = iObserverArray->Count();

        // If still in bounds, check if nothing changed. If so, then
        // we can move to next observer.
        if ( i < count )
            {
            const TPubSubObserverTag& obs = iObserverArray->At( i );

            if ( ( obs.iObserver == observerTag.iObserver ) &&
                 ( obs.iCategory == observerTag.iCategory ) &&
                 ( obs.iKey == observerTag.iKey ) )
                {
                i++;
                }
            }
        }
    }
               
// ---------------------------------------------------------
// CPhonePubSubProxy::CancelAllObserverNotifies
// ---------------------------------------------------------
//
void CPhonePubSubProxy::CancelAllObserverNotifies( 
    MPhonePubSubObserver* aObserver )
    {
    TInt count = iObserverArray->Count();

    // Remove the observer
    TBool observerRemoved = EFalse;
    for( TInt i = count - 1; i >= 0 && !observerRemoved; i-- ) 
        {
        const TPubSubObserverTag& observerTag = iObserverArray->At( i );
        if ( observerTag.iObserver == aObserver )
            {
            // Remove observer 
            observerRemoved = ETrue;
            iObserverArray->Delete( i );
            }
        }
    }

//  End of File  

