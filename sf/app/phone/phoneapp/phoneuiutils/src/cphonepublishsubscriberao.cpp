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
* Description:  see header description
 *
*/


// INCLUDES
#include <e32svr.h>
#include "cphonepublishsubscriberao.h"
#include "phonelogger.h"

// CONSTANTS

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::NewL()
// ----------------------------------------------------------------------------
CPhonePublishSubscriberAO* CPhonePublishSubscriberAO::NewL( 
    MPhonePubSubObserver* aPubSubObserver, 
    const TUid& aCategory, 
    const TUint aKey )
    {
    CPhonePublishSubscriberAO* self = 
        new (ELeave) CPhonePublishSubscriberAO( 
            aPubSubObserver, aCategory, aKey );

    return self;
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::Subscribe()
// ----------------------------------------------------------------------------
void CPhonePublishSubscriberAO::Subscribe()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::CPhonePublishSubscriberAO()
// ----------------------------------------------------------------------------
CPhonePublishSubscriberAO::CPhonePublishSubscriberAO( 
    MPhonePubSubObserver* aPubSubObserver, 
    const TUid& aCategory, 
    const TUint aKey ) :
    CActive( EPriorityStandard ),
    iPubSubObserver( aPubSubObserver ),
    iCategory( aCategory ),
    iKey( aKey )
    {
    CActiveScheduler::Add( this );
    iProperty.Attach( iCategory, iKey );
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::RunL()
// ----------------------------------------------------------------------------
void CPhonePublishSubscriberAO::RunL()
    {
    Subscribe();
    
    // Get the value     
    TInt value( 0 );
    iProperty.Get( iCategory, iKey, value );

    // Notify the observer
    iPubSubObserver->HandlePropertyChangedL( iCategory, iKey, value );
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::DoCancel()
// ----------------------------------------------------------------------------
void CPhonePublishSubscriberAO::DoCancel()
    {
    iProperty.Cancel();
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::RunError()
// ----------------------------------------------------------------------------
TInt CPhonePublishSubscriberAO::RunError( TInt aError )
    {    
    __PHONELOG1( 
        EBasic, 
        EPhonePhoneapp, 
        "CPhonePublishSubscriberAO::RunError: error: %d", 
        aError );

    return aError;
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::~CPhonePublishSubscriberAO()
// ----------------------------------------------------------------------------
CPhonePublishSubscriberAO::~CPhonePublishSubscriberAO()
    {
    Cancel();
    iProperty.Close();
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::Category()
// ----------------------------------------------------------------------------
const TUid CPhonePublishSubscriberAO::Category() const
    {
    return iCategory;
    }

// ----------------------------------------------------------------------------
// CPhonePublishSubscriberAO::Key()
// ----------------------------------------------------------------------------
TUint CPhonePublishSubscriberAO::Key() const
    {
    return iKey;
    }

// End of File
