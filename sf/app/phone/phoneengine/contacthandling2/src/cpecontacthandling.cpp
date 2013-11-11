/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEContactHandling class
*
*/

//  INCLUDE FILES
#include "cpecontacthandling.h"
#include "cpecontactmatch.h"
#include <mpedatastore.h>
#include <e32std.h>
#include <mpephonemodelinternal.h>
#include <pepanic.pan>
#include <barsc.h> 
#include <barsread.h>
#include <talogger.h>
#include <bldvariant.hrh>
#include <featmgr.h>

// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPEContactHandling::CPEContactHandling
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEContactHandling::CPEContactHandling
        ( 
        MPEPhoneModelInternal& aModel,
        RFs& aFsSession
        ) : iModel( aModel ),
            iFsSession( aFsSession )
    {
    }

// -----------------------------------------------------------------------------
// CPEContactHandling::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEContactHandling::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "CNT CPEContactHandling::ConstructL > CPEContactMatch::NewL" );
    iContactMatcher = CPEContactMatch::NewL( *this, *iModel.DataStore() );  
    }

// -----------------------------------------------------------------------------
// CPEContactHandling::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEContactHandling* CPEContactHandling::NewL
        ( 
        MPEPhoneModelInternal& aModel,  // Phone Model
        RFs& aFsSession
        )
    {
    TEFLOGSTRING(KTAOBJECT, "CNT CPEContactHandling::NewL start.");
    CPEContactHandling* self = new ( ELeave ) CPEContactHandling( aModel, aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING(KTAOBJECT, "CNT CPEContactHandling::NewL Complete.");
    return self;
    }

// Destructor
CPEContactHandling::~CPEContactHandling
        (
        // None.
        )
    {
    delete iContactMatcher;
    }

// -----------------------------------------------------------------------------
// CPEContactHandling::FindContactInfoSyncL
// Synchronous method for searching contact information with the key defined
//
// Supported synchronous searching keys are searching with phone number,
// name, contact id and voice tag (actually uses contact id)
// -----------------------------------------------------------------------------
//
void CPEContactHandling::FindContactInfoSyncL
        ( 
        TInt aCallId,             // call id from phone engine
        TPEFindContactKey aFindKey     // search key
        )     
    {
    TPEPhoneNumber line1 = iModel.DataStore()->VoiceMailBoxNumberLine1();
    TPEPhoneNumber line2 = iModel.DataStore()->VoiceMailBoxNumberLine2();
    TPEPhoneNumber remoteNumber = iModel.DataStore()->RemotePhoneNumber( aCallId );
    
    if( remoteNumber.Length() < 3 ) //according to UI spec numbers less than 3 digits are not matched.
        {
        return;
        }

    if ( remoteNumber == line1 )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactHandling::FindContactInfoSyncL, number matched to vmbx line 1" );
        iModel.DataStore()->SetRemotePhoneNumberType( EPEVmbxNumberLine1, aCallId );
        }
    else if ( remoteNumber == line2 )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactHandling::FindContactInfoSyncL, number matched to vmbx line 2" );
        iModel.DataStore()->SetRemotePhoneNumberType( EPEVmbxNumberLine2, aCallId );
        }
    else // If number was not voicemailboxnumber, check number from contacts.
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactHandling::FindContactInfoSyncL, number did not match vmbx" );
        FindContactInfoSyncFromContactDbL( aCallId, aFindKey );
        }
    }
// -----------------------------------------------------------------------------
// CPEContactHandling::FindContactInfoSync
// Calls method FindContactInfoSyncL which can leave. This leave is trapped in 
// this function.
// -----------------------------------------------------------------------------
//
TInt CPEContactHandling::FindContactInfoSync
        ( 
        const TInt aCallId,
        const TPEFindContactKey aFindKey
        )     
    {
    TRAPD( error, FindContactInfoSyncL( aCallId, aFindKey ) );
    TEFLOGSTRING2( KTAMESOUT, "CNT CPEContactHandling::FindContactInfoSync, error code: %d", error );
    return (error);
    }

// -----------------------------------------------------------------------------
// CPEContactHandling::SendErrorMessage
// Method reroutes error messages to the CPhoneModel object
// -----------------------------------------------------------------------------
//
void CPEContactHandling::SendErrorMessage
        ( 
        TInt aErrorCode 
        )
    {
    iModel.DataStore()->SetErrorCode( aErrorCode );
    iModel.SendMessage( MEngineMonitor::EPEMessageContactHandlingError );
    }

// -----------------------------------------------------------------------------
// CPEContactHandling::SendMessage
// Method reroutes error messages to the CPhoneModel object
// -----------------------------------------------------------------------------
//
void CPEContactHandling::SendMessage
        (
        MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        const TInt aCallId
        )
    {
    if ( aMessage == MEngineMonitor::EPEMessageThumbnailLoadingCompleted )
        {
        CFbsBitmap* thumbnailData = iContactMatcher->ContactThumbnail();
        if ( thumbnailData )
            {
            iModel.DataStore()->SetCallerThumbnail( thumbnailData, aCallId );
            }
        }

    iModel.SendMessage( aMessage, aCallId );
    }

// -----------------------------------------------------------------------------
// CPEContactHandling::FindContactInfoSyncFromContactDb
// Synchronous method for searching contact information from
// contact database with the key defined.
//
// Supported synchronous searching keys are searching with phone number and
// contact id.
// -----------------------------------------------------------------------------
//
void CPEContactHandling::FindContactInfoSyncFromContactDbL
        (
        const TInt aCallId,
        const TPEFindContactKey aFindKey
        ) const
    {
    ASSERT( iContactMatcher );

    // Check validity of the search key
    switch ( aFindKey )
        {
        case EPEFindWithPhoneNumber:
            iContactMatcher->MatchWithNumberL( aCallId );
            break;
        case EPEFindWithContactId:
            iContactMatcher->MatchWithContactIdL( aCallId );
            break;
        default:
            break;
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

//  End of File 
