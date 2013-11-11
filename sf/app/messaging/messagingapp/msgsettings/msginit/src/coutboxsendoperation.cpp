/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements class
*
*/



// INCLUDE FILES

#include "coutboxsendoperation.h"   // header

#include <SendUiConsts.h>           // MTM Uids
#include <msvids.h>                  // Entry Ids
#include <gsmerror.h>                // SMS sending failure error codes
#include <smutset.h>
#include "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
COutboxSendOperation* COutboxSendOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus )
    {
    // Create the instance of sending operation
    COutboxSendOperation* self =
        new (ELeave) COutboxSendOperation(
            aMsvSession, aObserverRequestStatus );

    // Push self into cleanup stack during construction
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    // Return the object
    return self;
    }

// C++ default constructor can NOT contain any code that
// might leave.
//
COutboxSendOperation::COutboxSendOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus )
    :
    CMsvOperation( aMsvSession, CActive::EPriorityStandard, aObserverRequestStatus ),
    iSelections( 4 ),
    iServices(),
    iSupportedMsgs( COutboxSendOperation::ESupportsSmsSending )
    {
    // Start scheduler
    CActiveScheduler::Add(this);
    }

//destructor
COutboxSendOperation::~COutboxSendOperation()
    {
    // Cancel sending
    Cancel();

    // Delete sending operation
    delete iOperation;
    iOperation = NULL;

    // Delete entry
    delete iEntry;
    iEntry = NULL;

    // Remove services from queue and destroy message selections
    iServices.Reset();
    iSelections.ResetAndDestroy();
    }

// ----------------------------------------------------
// COutboxSendOperation::ConstructL
// ----------------------------------------------------
//
void COutboxSendOperation::ConstructL()
    {
    // Get rootindex entry
    iEntry = iMsvSession.GetEntryL( KMsvRootIndexEntryId );
    iEntry->SetSortTypeL(
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );

    // Set sending flags
    iSupportedMsgs |= ESendSms;

    // Start sending
    StartSendingL();
    }

// ----------------------------------------------------
// COutboxSendOperation::RunL
// ----------------------------------------------------
//
void COutboxSendOperation::RunL()
    {
    QDEBUG_WRITE("COutboxSendOperation::RunL");
    User::LeaveIfError( iStatus.Int() );

    // Check and start sending, if needed
    TUid sendMtm;

    // Check if messages needs to be sent
    if( IsSendingNeeded( sendMtm ) )
        {
        StartSendingL();
        }
    // Nothing to send, complete operation
    else
        {
        CompleteObserver( iStatus.Int() );
        }
    }

// ----------------------------------------------------
// COutboxSendOperation::RunError
// ----------------------------------------------------
//
TInt COutboxSendOperation::RunError( TInt aError )
    {   
    CompleteObserver( aError );
    return aError;
    }

// ----------------------------------------------------
// COutboxSendOperation::DoCancel
// ----------------------------------------------------
//
void COutboxSendOperation::DoCancel()
    {
    // Check if sending operation is running
    if( iOperation )
        {
        // Cancel it
        iOperation->Cancel();
        }

    // Complete operation with current status
    CompleteObserver( iStatus.Int() );
    }

// ----------------------------------------------------
// COutboxSendOperation::ProgressL
// ----------------------------------------------------
//
const TDesC8& COutboxSendOperation::ProgressL()
    {
    // Check if operation exists
    if( iOperation )
        {
        // Return the operation
        return iOperation->ProgressL();
        }

    // If no operation, return blank information
    return KNullDesC8();
    }

// ---------------------------------------------------------
// COutboxSendOperation::CompleteOperation
// ---------------------------------------------------------
//
void COutboxSendOperation::CompleteObserver( TInt aStatus )
    {
    // Get the observer status
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aStatus );
    }

// ----------------------------------------------------
// COutboxSendOperation::StartSendingL
// ----------------------------------------------------
//
void COutboxSendOperation::StartSendingL()
    {
    // Remove any sending operation that currently may be running
    delete iOperation;
    iOperation = NULL;


    // Check if there was errors with creating selection
    if  ( CheckAndCreateSelectionL() )
        {
        // Create new operation and trap any errors
        SendWaitingSMSMessages();
        RemoveSelection();
        }
    else
        {
        CompleteSelf( KErrNone );
        }
    }

    // if error, then complete this pass with the error code
void COutboxSendOperation::SendWaitingSMSMessages()
    {
    TRAPD( err, SendWaitingSMSMessagesL() );
    if  ( err != KErrNone )
        {
        ASSERT( !IsActive() );
        CompleteSelf( err );
        }
    }

// ----------------------------------------------------
// COutboxSendOperation::SendWaitingSMSMessagesL
// ----------------------------------------------------
//
void COutboxSendOperation::SendWaitingSMSMessagesL()
    {
    // Get first selection from queue
    CMsvEntrySelection& selection = ( *iSelections[0] );

    // Get count of messages in queue
    TInt count = selection.Count();

    // Go through all messages
    while( count-- )
        {
        // Select message
        iEntry->SetEntryL( selection[count] );
        TMsvEntry entry( iEntry->Entry() );

        // Check if the message is tried to send when in offline
        if( ( entry.SendingState() == KMsvSendStateSuspended ||
              entry.SendingState() == KMsvSendStateFailed) &&
            ( entry.iError == KErrGsmOfflineOpNotAllowed ||
              entry.iError == KErrGsmOfflineSimOpNotAllowed) )
            {
            // Set message to wait sending
            entry.SetSendingState( KMsvSendStateWaiting );
            iEntry->ChangeL( entry );
            }
		else
			{
			selection.Delete( count );
			}
        }
    selection.Compress();

    // Set entry to outbox
    iMtm = iEntry->Entry().iMtm;
    iEntry->SetEntryL( KMsvGlobalOutBoxIndexEntryId );
	if ( selection.Count() )
		{
		iOperation = iEntry->CopyL( selection, iServices[0], iStatus );
        SetActive();
		}
    else
        {
        // Nothing to send, but we must complete the observer via our RunL callback
        CompleteSelf( KErrNone );
        }
    }


// ----------------------------------------------------
// COutboxSendOperation::RemoveSelection
// ----------------------------------------------------
//
void COutboxSendOperation::RemoveSelection()
    {
    // Clear the the current selection.
    iServices.Delete(0);

    // Delete selection object and index
    delete iSelections[0];
    iSelections.Delete(0);
    }

// ----------------------------------------------------
// COutboxSendOperation::CreateSelectionL
// ----------------------------------------------------
//
void COutboxSendOperation::CreateSelectionL(
    const TUid &aUidForSel,
    const TMsvId& aServiceId,
    CMsvEntrySelection*& aSelection )
    {
    // Set entry to outbox and get messages from outbox
    iEntry->SetEntryL( KMsvGlobalOutBoxIndexEntryId );
    aSelection = iEntry->ChildrenWithMtmL( *&aUidForSel );

    // Check if there is anything to put into array
    if( aSelection->Count() )
        {
        // Put selection to queue
        CleanupStack::PushL( aSelection );
        iSelections.AppendL( aSelection );
        CleanupStack::Pop( aSelection );

        // Put service to queue
        iServices.AppendL( aServiceId );
        }
    else
        {
        // Remove selection
        delete aSelection;
        aSelection = NULL;
        }
    }

// ----------------------------------------------------
// COutboxSendOperation::CheckAndCreateSelectionL
// ----------------------------------------------------
//
TBool COutboxSendOperation::CheckAndCreateSelectionL()
    {
    // Get root index
    iEntry->SetEntryL( KMsvRootIndexEntryId );

    // MTM, for which the selection is collected
    TUid sendMtm;

    // Selection of messages for sending
    CMsvEntrySelection* smsSelection = NULL;

    // While MTM's available for sending
    while( smsSelection == NULL && IsSendingNeeded( sendMtm ) )
        {
        // Find default SMS service
        TMsvId serviceId =0;
        TSmsUtilities::ServiceIdL(*iEntry, serviceId);
        QDEBUG_WRITE_FORMAT("COutboxSendOperation::CheckAndCreateSelectionL ",serviceId );
        // Check if the service ID is found
        if( serviceId != KMsvNullIndexEntryId )
            {
            // Create selection of messages of specified MTM
            CreateSelectionL( sendMtm, serviceId, smsSelection );
            }
        // Selection has been created, remove the flag
        RemoveSendingFlag( *&sendMtm );
        }

    const TBool selectionAvailable = ( smsSelection != NULL );
    return selectionAvailable;
    }

// ----------------------------------------------------
// COutboxSendOperation::RemoveSendingFlag
// ----------------------------------------------------
//
void COutboxSendOperation::RemoveSendingFlag( const TUid& aMtm )
    {
    // Decide actions by mtm
    switch( aMtm.iUid )
        {
        // SMS-messages
        case KSenduiMtmSmsUidValue:
            iSupportedMsgs &= ~ESendSms;
            break;        
        default:
            break;
        }
    }

// ----------------------------------------------------
// COutboxSendOperation::IsSendingNeeded
// ----------------------------------------------------
//
TBool COutboxSendOperation::IsSendingNeeded(
    TUid& aMtm ) const
    {
    // Set starting condition
    TBool needSending = EFalse;

    // Check if sms-sending is supported and messages need to be sent
    if( iSupportedMsgs & ESupportsSmsSending &&
        iSupportedMsgs & ESendSms )
        {
        aMtm = KSenduiMtmSmsUid;
        needSending = ETrue;
        }
    // Otherwise nothing needs to be done
    else
        {
        aMtm.iUid = 0;
        needSending = EFalse;
        }

    // Return the result
    return needSending;
    }

// ----------------------------------------------------
// COutboxSendOperation::CompleteSelf
// ----------------------------------------------------
//
void COutboxSendOperation::CompleteSelf( TInt aValue )
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aValue );
    SetActive();
    }
    
// End of file
