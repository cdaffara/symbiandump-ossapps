/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   operation for sending a read report
*
*/



#include <msvstd.h>
#include <msventry.h>

#include "mmsreadreport.h"
#include "mmsheaders.h"
#include "mmsservercommon.h"
#include "mmsserverentry.h"
#include "mmsencode.h"
#include "mmserrors.h"
#include "mmssettings.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CMmsReadReport::CMmsReadReport( RFs& aFs ):
    CMmsBaseOperation( aFs )
    {
    //
    // members that get initial value 0, need not be initialized here
    //
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmsReadReport::ConstructL( CMmsSettings* aMmsSettings )
    {
    CMmsBaseOperation::ConstructL( aMmsSettings );
    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion()  );
    CActiveScheduler::Add( this );
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmsReadReport* CMmsReadReport::NewL( RFs& aFs,  CMmsSettings* aMmsSettings  )
    {
    CMmsReadReport* self = new( ELeave ) CMmsReadReport( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aMmsSettings );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmsReadReport::~CMmsReadReport()
    {
    Cancel(); // has to be called first
    delete iMmsHeaders;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CMsvEntrySelection& CMmsReadReport::Failed() const
    {
    return *iFailed;
    }
    
// ---------------------------------------------------------------------------
// From class CMmsBaseOperation
//
// ---------------------------------------------------------------------------
//
void CMmsReadReport::StartL(
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService,
            TRequestStatus& aStatus )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsReadReport::StartL") );
#endif
    // initialize common variables
    CMmsBaseOperation::InitializeL( aSelection, aServerEntry, aService );
    iMmsHeaders->Reset();
    
    if ( iCurrentMessageNo < 1 )
        {
        // nothing to send. Give up immediately
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNotFound );
        return;
        }
    
    Queue( aStatus );
    FallThrough();   
    }
    
// ---------------------------------------------------------------------------
// From class CActive
//
// ---------------------------------------------------------------------------
//
void CMmsReadReport::DoCancel()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsReadReport::DoCancel") );
    #endif
    CMmsBaseOperation::DoCancel();
    
    // Unhandled entries are deleted in CMmsServerMtm::UpdateEntriesL()
    }
    
    
// ---------------------------------------------------------
// From class CMsgActive
//
// ---------------------------------------------------------
//
void CMmsReadReport::DoComplete( TInt& aError )
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsReadReport DoComplete"));
#endif
    // We should get here if we are cancelled, or if
    // the cycle has completed (with or without error)

    // Only final cleanup can be done here, nothing asychronous is allowed
    
    if ( aError == KMmsErrorOfflineMode )
        {
        // if we cannot connect, read report is not sent
        // It will not be retried.
        aError = KErrNone;
        }
    
    // Normally we are not supposed to have anything in the failed list
    // Even if read report sending fails, entries are deleted.
    // Read report sending is "best effort" only, no retries.
    UnSetSendMask( *iFailed, aError );

    }

// ---------------------------------------------------------------------------
// From class CMmsBaseOperation
//
// ---------------------------------------------------------------------------
//
void CMmsReadReport::EncodePDUL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsReadReport::EncodePDU") );
    #endif
    
    if( iError != KErrNone )
        {
        FallThrough();
        return;
        }
    // Encode the PDU to be sent to MMSC    
    //
    // Set entry's context to next message in the selection
    //
    iError = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if( iError != KErrNone )
        {
        FallThrough();
        return;
        }

    CMsvStore* store = NULL;
    TRAP( iError, store = iServerEntry->ReadStoreL(); )
    if ( iError != KErrNone )
        {
        FallThrough();
        return;
        }
    CleanupStack::PushL( store );

    //
    // Filling headers object from the data in the MessageStore
    //
    iMmsHeaders->RestoreL( *store );
    iMmsHeaders->SetMessageType( KMmsMessageTypeReadRecInd );
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    iError = iEntryWrapper->SetCurrentEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( iError != KErrNone )
        {
        FallThrough();
        }
    else
        {
        iEncoder->EncodeHeadersL( *iMmsHeaders, *iEncodeBuffer );
        FallThrough();
        }

    }
    
    
// ---------------------------------------------------------------------------
// From class CMmsBaseOperation
//
// ---------------------------------------------------------------------------
//
void CMmsReadReport::MoveEntryL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "CMmsReadReport::MoveEntryL" ) );
    #endif
    
    // The entry will be just deleted - no need to report failure
    TInt error = KErrNone;
    error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( error == KErrNone )
        {
        TMsvEntry entry = iServerEntry->Entry();
        error = iServerEntry->SetEntry( entry.Parent() );
        
        if ( error == KErrNone )
            {
            // Delete entry
            error = iServerEntry->DeleteEntry( entry.Id() );
            }
        if ( error == KErrNone )
            {
            // Everything has been originally appended to the failed list
            // If an entry was successfully deleted, it is removed from
            // the list. The rest will be deleted in MMS Server MTM -
            // or garbage collected later.
            iFailed->Delete( iCurrentMessageNo - 1 );
            }
        }
        
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    FallThrough();

    }
    
