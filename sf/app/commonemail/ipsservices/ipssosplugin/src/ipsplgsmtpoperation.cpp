/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgSmtpOperation.
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"
#include "miut_err.h" // SMTP error codes


const TInt KIpsSmtpOperationCharLessThan = '<';
const TInt KIpsSmtpOperationCharMoreThan = '>';

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
// <qmail> aPriority, aUsePublishSubscribe parameters removed, aOperationObserver, aRequestId added
EXPORT_C CIpsPlgSmtpOperation* CIpsPlgSmtpOperation::NewL( 
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus,
    MFSMailRequestObserver* aOperationObserver,
    TInt aRequestId )
    {
    FUNC_LOG;
    CIpsPlgSmtpOperation* self = CIpsPlgSmtpOperation::NewLC(
        aMsvSession,
        aObserverRequestStatus,
        aOperationObserver,
        aRequestId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
// <qmail> aPriority, aUsePublishSubscribe parameters removed, aOperationObserver, aRequestId added
EXPORT_C CIpsPlgSmtpOperation* CIpsPlgSmtpOperation::NewLC( 
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus,
    MFSMailRequestObserver* aOperationObserver,
    TInt aRequestId )
    {
    FUNC_LOG;
    CIpsPlgSmtpOperation* self = new( ELeave ) CIpsPlgSmtpOperation(
        aMsvSession,
        aObserverRequestStatus,
        aOperationObserver,
        aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
   
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// <qmail> aPriority parameter removed, aOperationObserver, aRequestId added
CIpsPlgSmtpOperation::CIpsPlgSmtpOperation( 
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId ) :
    CIpsPlgBaseOperation( aMsvSession, aObserverRequestStatus, aFSRequestId, TFSMailMsgId() ),
    iSmtpMtm( NULL ),
    iOperation( NULL ),
    iSelection( NULL ),  
    iMtmRegistry( NULL ),
    iState( EIdle ),
    iFSOperationObserver( aFSOperationObserver ),
    iEventHandler( NULL )
    {
    FUNC_LOG;
    }
  
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
CIpsPlgSmtpOperation::~CIpsPlgSmtpOperation() 
    {
    FUNC_LOG;
    Cancel();
    delete iSmtpMtm;
    delete iMtmRegistry;
    delete iSelection;
    delete iOperation;
    iOperation = NULL;
    iEventHandler = NULL;
    iState = EIdle;
    } 

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpOperation::ConstructL()
    {
    FUNC_LOG;
    iSelection = new (ELeave) CMsvEntrySelection();
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpOperation::CompleteObserver( TInt aStatus /*= KErrNone*/ )
    {
    FUNC_LOG;

    TRequestStatus* status = &iObserverRequestStatus;
    if ( status && status->Int() == KRequestPending )
        {
        User::RequestComplete( status, aStatus );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpOperation::CompleteThis()
    {
    FUNC_LOG;

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpOperation::RunL()
    {
    FUNC_LOG;

    switch ( iState )
        {
        case EMovingOutbox:
            {
            delete iOperation;
            iOperation = NULL;
            iSelection->InsertL( 0, iSmtpService );
            CallSendL();
            break;
            }
        case ESending:
            {
            TImSmtpProgress prog;
            TPckg<TImSmtpProgress> param(prog);
            param.Copy( iOperation->FinalProgress() );

// <qmail> Removed login error handling

// <qmail>
	        if ( iFSOperationObserver )
	            {
	            iFSProgress.iProgressStatus =
	                TFSProgress::EFSStatus_RequestComplete;
	            iFSProgress.iError = prog.Error();

	            TRAP_IGNORE( iFSOperationObserver->RequestResponseL(
	                iFSProgress, iFSRequestId ) );
	            }
	        TRequestStatus* status = &iObserverRequestStatus;
	        User::RequestComplete( status, prog.Error() );
    		break;
// <qmail>
            }
        }
	}

// <qmail>
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CIpsPlgSmtpOperation::RunError( TInt aError )
    {
    FUNC_LOG;

    if ( iFSOperationObserver )
        {
        iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        iFSProgress.iError = aError;

        TRAP_IGNORE( iFSOperationObserver->RequestResponseL(
			iFSProgress, iFSRequestId ) );
        }

    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aError );
    
    return KErrNone;
    }
// </qmail>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void CIpsPlgSmtpOperation::DoCancel()
    {
    FUNC_LOG;
    
    if ( iOperation )
        {
        iOperation->Cancel();
        }

    // It is not relevant to inform FS observer. Cancelling is initiated
    // either by the observer or the engine destructor when the application
    // exits.
    CompleteObserver( KErrCancel );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
const TDesC8& CIpsPlgSmtpOperation::ProgressL()
    {
    FUNC_LOG;
    
    if( iOperation && iState == ESending  )
        {
        return iOperation->ProgressL();
        }
    return KNullDesC8;
    }

// <qmail>
// ---------------------------------------------------------------------------
// CIpsPlgSmtpOperation::GetErrorProgressL
// ---------------------------------------------------------------------------
//   
const TDesC8& CIpsPlgSmtpOperation::GetErrorProgressL( TInt /*aError*/ )
    {
    FUNC_LOG;
    
    return KNullDesC8; // error progress info not supported
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpOperation::GetFSProgressL
// ---------------------------------------------------------------------------
//   
TFSProgress CIpsPlgSmtpOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    
    return iFSProgress;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------    
// <qmail> return type
TIpsOpType CIpsPlgSmtpOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeSmtp;
    }
// </qmail>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
EXPORT_C void CIpsPlgSmtpOperation::StartSendL( TMsvId aMessageId )
    {
    FUNC_LOG;
    
    TMsvId service;
    TMsvEntry entry;
    User::LeaveIfError( 
        iMsvSession.GetEntry( aMessageId, service, entry ) );
    if ( entry.Id() == KMsvNullIndexEntryIdValue )
        {
        User::Leave( KErrNotFound );
        }
    
    iSmtpService = entry.iServiceId;
    ConstructSmtpMtmL();
    iSmtpMtm->SwitchCurrentEntryL( entry.Id() );
    
    if ( entry.Parent() == KMsvGlobalOutBoxIndexEntryIdValue )
        {
        iSelection->Reset();
        // smtp service id have to be first in array
        // here must be iSmtpService
        iSelection->AppendL( iSmtpService );
        // then message id
        iSelection->AppendL( entry.Id() );
        CallSendL( );
        }
    else
        {
        // 1. perform validity checks for recipient addresses
        // 2. move message to outbox
        // Perform validity checks for recipient addresses
        // before moving
        CMsvEntry* msgCEntry = CMsvEntry::NewL(
            iMsvSession, aMessageId, TMsvSelectionOrdering() );
        CleanupStack::PushL( msgCEntry );
        ValidateRecipientsL( *msgCEntry );
        CleanupStack::PopAndDestroy( msgCEntry );
        msgCEntry = NULL;
        
        iSelection->Reset();
        // get message entry
        msgCEntry = CMsvEntry::NewL(
            iMsvSession, entry.Parent(), TMsvSelectionOrdering() );
        CleanupStack::PushL( msgCEntry );

        // do the move
        iSelection->AppendL( entry.Id());
        iStatus = KRequestPending;
        delete iOperation;
        iOperation = NULL;
        iOperation = msgCEntry->MoveL( 
            *iSelection, KMsvGlobalOutBoxIndexEntryIdValue, iStatus );
        SetActive();
        CleanupStack::PopAndDestroy( msgCEntry );
        iState = EMovingOutbox; 
        }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CIpsPlgSmtpOperation::EmptyOutboxFromPendingMessagesL( 
    TMsvId aMailboxId  )
    {
    FUNC_LOG;
    TMsvId service;
    TMsvEntry entry;
    User::LeaveIfError( 
        iMsvSession.GetEntry( aMailboxId, service, entry ) );
    
    iSmtpService = entry.iRelatedId;
    
    TInt retValue = KErrNone;
    
    iSelection->Reset();
    ConstructSmtpMtmL();
    iSmtpMtm->SwitchCurrentEntryL( KMsvGlobalOutBoxIndexEntryIdValue );
    
    // in sending first entry in selection array have to be service id
    iSelection->AppendL(iSmtpService);
    //index to first message entry of selection array
    
    CMsvEntrySelection* childs = GetOutboxChildrensL( );
    CleanupStack::PushL( childs );
    for ( TInt i = 0; i < childs->Count(); i++ )
        {
        iMsvSession.GetEntry( childs->At(i), service, entry );
        TUint sendState = entry.SendingState();
        
        if ( iSmtpService == entry.iServiceId 
		     && sendState != KMsvSendStateUnknown
	         && sendState != KMsvSendStateUponRequest
              && sendState != KMsvSendStateSent ) 
            {
            // change suspended flag to waiting
            // is it safe to change flag from suspended to waiting?
            if ( sendState == KMsvSendStateSuspended )
                {
                // <qmail> remove activeschedulerwait
                CMsvEntry* cEntry = CMsvEntry::NewL(
                    iMsvSession, entry.Id(), TMsvSelectionOrdering() );
                CleanupStack::PushL( cEntry );
                entry.SetSendingState( KMsvSendStateWaiting );
                cEntry->ChangeL( entry );
                CleanupStack::PopAndDestroy( cEntry ); // cEntry
                // </qmail>
                }
            // add to send list
            iSelection->AppendL( entry.Id() );
            ++retValue;
            }
        }
    CleanupStack::PopAndDestroy( childs );
    if (retValue > 0)
        {
        CallSendL();
        }
    else
        {
        // nothing to do, finish operation
        CompleteObserver( KErrNone );
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CMsvEntrySelection* CIpsPlgSmtpOperation::GetOutboxChildrensL( )
    {
    FUNC_LOG;
    ConstructSmtpMtmL();
    iSmtpMtm->SwitchCurrentEntryL( KMsvGlobalOutBoxIndexEntryIdValue );
    return iSmtpMtm->Entry().ChildrenL();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpOperation::CallSendL( )
    {
    FUNC_LOG;
    TBuf8<1> params;
    
    iOperation = iSmtpMtm->InvokeAsyncFunctionL(
        KSMTPMTMSendOnNextConnection, *iSelection, params, iStatus);
    SetActive();
    iState = ESending;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpOperation::ConstructSmtpMtmL()
    {
    FUNC_LOG;
    if ( !iMtmRegistry )
        {
        iMtmRegistry = CClientMtmRegistry::NewL( iMsvSession );
        }
    if ( !iSmtpMtm )
        {
        iSmtpMtm = static_cast<CSmtpClientMtm*>(
            iMtmRegistry->NewMtmL( KSenduiMtmSmtpUid ) );
        }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CIpsPlgSmtpOperation::ValidateRecipientsL( CMsvEntry& aEntry )
    {
    FUNC_LOG;
    
    // get store object
    CMsvStore* store = aEntry.ReadStoreL();
    CleanupStack::PushL( store );   // << store
    
    // create symbian email header object
    CImHeader* header = CImHeader::NewLC(); // << header
    header->RestoreL( *store );

    // checking separate recipient fields
    ValidateAddressArrayL( header->ToRecipients() );
    ValidateAddressArrayL( header->CcRecipients() );
    ValidateAddressArrayL( header->BccRecipients() );
    
    // Cleanup
    CleanupStack::PopAndDestroy( header );
    CleanupStack::PopAndDestroy( store );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CIpsPlgSmtpOperation::ValidateAddressArrayL( const CDesCArray& aRecipients )
    {
    FUNC_LOG;
    
    TInt count( aRecipients.Count() );
    for ( TInt i(0); i < count; i++ )
        {
        TPtrC fullName( aRecipients[i] );
        
        // Need to locate angle brackets from end of the data, if there
        // is display name with special characters
        TInt start( fullName.LocateReverse( KIpsSmtpOperationCharLessThan ) ); 
        TInt end( fullName.LocateReverse( KIpsSmtpOperationCharMoreThan ) );
        if ( KErrNotFound != start && KErrNotFound != end )
            {
            start += 1;
            fullName.Set( aRecipients[i].Mid( start, ( end - start ) ) );
            }
        // make basic sanity checks for email address
// <qmail>
        //if( !IpsSetUtils::IsValidEmailAddressL( fullName ) )
        //    {
        //    User::Leave( KErrBadName );
        //    }
// </qmail>
        }
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CIpsPlgSmtpOperation::SetEventHandler( TAny* aEventHandler )
    {
    iEventHandler = aEventHandler;
    }
// <qmail> TBool CIpsPlgSmtpOperation::QueryUserPassL() removed
// <qmail> void CIpsPlgSmtpOperation::CredientialsSetL( TInt aEvent )

// End of File

