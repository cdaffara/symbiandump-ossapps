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
* Description:  see class description.
 *
*/


// INCLUDE FILES
#include "phonelogger.h"
#include "phoneui.pan"

#include "cphonecenrepeventhandler.h"
#include "cphonecenrepproxy.h"

// ========================== MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CPhoneCenRepEventHandler* CPhoneCenRepEventHandler::NewL()
// ----------------------------------------------------------------------------

CPhoneCenRepEventHandler* CPhoneCenRepEventHandler::NewL( 
    const TUid& aUid )
    {       
    CPhoneCenRepEventHandler* self = 
        new ( ELeave ) CPhoneCenRepEventHandler( aUid );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 

    return self;
    }

// ----------------------------------------------------------------------------
// CPhoneCenRepEventHandler::CPhoneCenRepEventHandler
// ----------------------------------------------------------------------------

CPhoneCenRepEventHandler::CPhoneCenRepEventHandler( const TUid& aUid ) :
    iUid( aUid )
    {
    }

// ----------------------------------------------------------------------------
// CPhoneCenRepEventHandler::~CPhoneCenRepEventHandler
// ----------------------------------------------------------------------------

CPhoneCenRepEventHandler::~CPhoneCenRepEventHandler()
    {
    if ( iCenRepNotifyHandler )
        {
        iCenRepNotifyHandler->StopListening();
        }
    delete iCenRepNotifyHandler;
    delete iRepository;
    }

// ----------------------------------------------------------------------------
// CPhoneCenRepEventHandler::ConstructL
// ----------------------------------------------------------------------------

void CPhoneCenRepEventHandler::ConstructL()
    {
    __PHONELOG1( 
        EBasic, 
        EPhonePhoneapp, 
        "CPhoneCenRepEventHandler::ConstructL: uid=%d", 
        iUid );

    iRepository = CRepository::NewL( iUid );
    
    iCenRepNotifyHandler = CCenRepNotifyHandler::NewL( 
        *this, 
        *iRepository );

    iCenRepNotifyHandler->StartListeningL();
    }

// ----------------------------------------------------------------------------
// CPhoneCenRepEventHandler::HandleNotifyGeneric
// ----------------------------------------------------------------------------
void CPhoneCenRepEventHandler::HandleNotifyGeneric( TUint32 aId )
    {
    
    __PHONELOG2( 
        EBasic, 
        EPhonePhoneapp, 
        "CPhoneCenRepEventHandler::HandleNotifyGeneric: uid=%d, id=%d", 
        iUid,
        aId );

    CPhoneCenRepProxy::Instance()->HandleNotify( iUid, aId );
    }

// ----------------------------------------------------------------------------
// CPhoneCenRepEventHandler::HandleNotifyError
// ----------------------------------------------------------------------------
void CPhoneCenRepEventHandler::HandleNotifyError( 
    TUint32 /*aId*/, 
    TInt error, 
    CCenRepNotifyHandler* /*aHandler*/ )
    {
    if ( error )
        {
        __PHONELOG2( 
            EBasic, 
            EPhonePhoneapp, 
            "CPhoneCenRepEventHandler::HandleNotifyError: uid=%d, error = %d", 
            iUid,
            error );        
        }

    // Resume listening
    TRAPD( err, iCenRepNotifyHandler->StartListeningL() );
    if ( KErrNone != err )
        {
        Panic( err );
        }
    }

// End of File

