/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     This class implements notifications from bearers which is needed when
*     transferring data to/from WAP GW.
*
*/



// INCLUDE FILES
#include    <es_sock.h>

#include    "mmsbearerstatus.h"
#include    "mmssession.h"
#include    "mmstransportobserver.h"
#include    "mmsservercommon.h"
#include    "MmsServerDebugLogging.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CMmsBearerStatus::CMmsBearerStatus()
    :CActive ( EPriorityStandard),
    iRequester ( NULL ),
    iConnected( EFalse ),
    iSuspended( EFalse )
    {
    iBearerType = EMmsBearerGprs; // default
    }

// ---------------------------------------------------------
// CMmsBearerStatus::ConstructL
// ---------------------------------------------------------
//
void CMmsBearerStatus::ConstructL( RConnection& aRConnection )
    {
    LOG( _L("CMmsBearerStatus::ConstructL") );
    iRConnection = &aRConnection;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------
//
CMmsBearerStatus* CMmsBearerStatus::NewL( RConnection& aRConnection )
    {
    CMmsBearerStatus* self = new ( ELeave ) CMmsBearerStatus();
    CleanupStack::PushL( self );
    self->ConstructL( aRConnection );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CMmsBearerStatus::~CMmsBearerStatus()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CMmsBearerStatus::IsGprs
// ---------------------------------------------------------
//
TBool CMmsBearerStatus::IsGprs() const
    {
    if( iBearerType == EMmsBearerGprs )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CMmsBearerStatus::IsSuspendedL
// ---------------------------------------------------------
//
TBool CMmsBearerStatus::IsSuspended()
    {
    if( iBearerType != EMmsBearerGprs )
        {
        LOG( _L("CMmsBearerStatus::IsSuspended: Something wrong, no GPRS!") );
        return EFalse;
        }
    TNifProgress nifProgress;
    TInt err = iRConnection->Progress( nifProgress );
    if( err == KErrNone )
        {
        if( nifProgress.iStage == KDataTransferTemporarilyBlocked )
            {
            iSuspended = ETrue;
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsBearerStatus::SubscribeNotification
// ---------------------------------------------------------
//
void CMmsBearerStatus::SubscribeNotification( MMmsTransportObserver* aRequester )
    {
    LOG( _L("CMmsBearerStatus::SubscribeNotification") );
    
    iRequester = ( MMmsTransportObserver* ) aRequester;
    // set status to pending to make sure that it is RNif
    // that sets it to complete
    iStatus = KRequestPending;
    iRConnection->ProgressNotification( iProgressBuffer, iStatus );

#ifndef _NO_MMSS_LOGGING_
    if( IsActive() )
        {
        LOG( _L(" - already active") );
        }
#endif  //_NO_MMSS_LOGGING_
    SetActive();
    }

// ---------------------------------------------------------
// CMmsBearerStatus::RunL
// ---------------------------------------------------------
//
void CMmsBearerStatus::RunL()
    {
#ifndef _NO_MMSS_LOGGING_
    if( iStatus.Int() != KErrNone )
        {
        LOG2( _L("ERROR in CMmsBearerStatus::RunL status %d)"), iStatus.Int() );
        }
    LOG2( _L("CMmsBearerStatus::RunL(): progress stage %d"), iProgressBuffer().iStage );
#endif  //_NO_MMSS_LOGGING_


    if( iProgressBuffer().iStage == KLinkLayerClosed && iConnected )
        {
        iConnected = EFalse;
        iRequester->TransferCancelled();
        }
    else if( iProgressBuffer().iStage == KLinkLayerOpen ) 
        {
        iConnected = ETrue;
        if( iSuspended )
            {
            iSuspended = EFalse;
            iRequester->GprsResumed();
            // GprsResumed() puts the observer (this) active if necessary
            }
        else
            {
            SubscribeNotification( iRequester );
            }
        }
    else if( iProgressBuffer().iStage == KDataTransferTemporarilyBlocked )
        {
        iSuspended = ETrue;
        iRequester->GprsSuspended();
        SubscribeNotification( iRequester );
        }
    else
        {
        // Some other state, not really interested about those...
        SubscribeNotification( iRequester );        
        }    
    }

// ---------------------------------------------------------
// CMmsBearerStatus::DoCancel
// ---------------------------------------------------------
//
void CMmsBearerStatus::DoCancel()
    {
    LOG( _L("CMmsBearerStatus::DoCancel") );
    iRConnection->CancelProgressNotification();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

