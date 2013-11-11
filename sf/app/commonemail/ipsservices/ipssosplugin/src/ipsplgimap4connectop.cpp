/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMAP4 connect operation
*
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> removed

// <qmail> KIpsPlgIpsConnPanic removed

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::NewL()
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
// <qmail> MFSMailRequestObserver& changed it to pointer
// <qmail> aSignallingAllowed parameter has been removed
CIpsPlgImap4ConnectOp* CIpsPlgImap4ConnectOp::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler,
    TBool aDoPlainConnect )
    {
    FUNC_LOG;
    CIpsPlgImap4ConnectOp* self = new (ELeave) CIpsPlgImap4ConnectOp(
        aMsvSession, 
        aObserverRequestStatus,
        aService, 
        aActivityTimer,
        aFSMailBoxId, 
        aFSOperationObserver,
        aFSRequestId,
        aDoPlainConnect,
        aEventHandler );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::CIpsPlgImap4ConnectOp()
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
// <qmail> MFSMailRequestObserver& changed it to pointer
// <qmail> aSignallingAllowed parameter has been removed
CIpsPlgImap4ConnectOp::CIpsPlgImap4ConnectOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    TBool aDoPlainConnect,
    CIpsPlgEventHandler* aEventHandler)
    :
    CIpsPlgOnlineOperation(
	    aMsvSession,
	    aObserverRequestStatus,
	    aActivityTimer,
	    aFSMailBoxId,
	    aFSOperationObserver,
	    aFSRequestId ),
    iState( EStateIdle ),
    iSelection( NULL ),
    iDoPlainConnect( aDoPlainConnect ),
    iEventHandler( aEventHandler ),
    iIsSyncStartedSignaled( EFalse )
    {
    FUNC_LOG;
    iService = aService;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::~CIpsPlgImap4ConnectOp()
// ----------------------------------------------------------------------------
// 
CIpsPlgImap4ConnectOp::~CIpsPlgImap4ConnectOp()
    {
    FUNC_LOG;
    Cancel();
    delete iSelection;
    // <qmail> setting state in destructor makes no sense
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::ConstructL()
// ----------------------------------------------------------------------------
// 
void CIpsPlgImap4ConnectOp::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypeIMAP4 ); 
    iSelection = new(ELeave) CMsvEntrySelection;
    
    TMsvEntry tentry;
    TMsvId service;
    iMsvSession.GetEntry( iService, service, tentry );
    
    if ( tentry.iType.iUid != KUidMsvServiceEntryValue )
        {
        User::Panic( KIpsPlgIpsConnPanic, KErrNotSupported );
        }
    
    // <qmail> moved here from StartL which was removed
    if ( Connected() && iDoPlainConnect )
        {
        // trivial case: connect requested, but already connected
        // (sync is done automatically on background)
        iState = EStateIdle;
        }
    else
        {
        iState = EStateStartConnect;
        }
    iStatus = KRequestPending;    
    // <qmail> SetActive(); moved inside CompleteThis();
    CompleteThis();
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgImap4ConnectOp::ProgressL()
    {
    FUNC_LOG;
    if( iError != KErrNone )
        {
        return GetErrorProgressL( iError );
        }
    else if( iSubOperation )
        {
        return iSubOperation->ProgressL();
        }
        
    TImap4CompoundProgress& prog = iProgressBuf();
    prog.iGenericProgress.iErrorCode = KErrNone;
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::GetErrorProgressL()
// ----------------------------------------------------------------------------    
//   
const TDesC8& CIpsPlgImap4ConnectOp::GetErrorProgressL(TInt aError)
    {
    FUNC_LOG;
    TImap4CompoundProgress& prog = iProgressBuf();
    prog.iGenericProgress.iErrorCode = aError;      
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::GetFSProgressL()
// ----------------------------------------------------------------------------
// 
TFSProgress CIpsPlgImap4ConnectOp::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    result.iError = KErrNone;
    switch( iState )
        {
        // <qmail> cases EStateQueryingDetails/EStateQueryingDetailsBusy removed
        case EStateStartConnect:
            result.iProgressStatus = TFSProgress::EFSStatus_Started;
            break;
        case EStateConnectAndSync:
            result.iProgressStatus = TFSProgress::EFSStatus_Connecting;
            break;
        case EStateCompleted:
        default:
            result.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
            break;
        }
    if ( iStatus.Int() == KErrCancel )
        {
        result.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
        result.iError = KErrCancel;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::IpsOpType()
// ----------------------------------------------------------------------------
// <qmail> return type
TIpsOpType CIpsPlgImap4ConnectOp::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeImap4SyncOp;
    }

// <qmail> Connected() moved to baseclass
// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoCancel()
// ----------------------------------------------------------------------------
// 
void CIpsPlgImap4ConnectOp::DoCancel()
    {
    FUNC_LOG;
    if( iSubOperation )
        {
        iSubOperation->Cancel();
        }
    SignalSyncCompleted( KErrCancel );
    CompleteObserver( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoRunL()
// ----------------------------------------------------------------------------    
//  
void CIpsPlgImap4ConnectOp::DoRunL()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    __ASSERT_DEBUG( !(iSubOperation && iSubOperation->IsActive()),
        User::Panic( KIpsPlgPanicCategory, KErrGeneral ) );
    if ( iSubOperation )
        {
        err = iSubOperation->iStatus.Int();
        delete iSubOperation;
        iSubOperation = NULL;
        }
    
    switch( iState )
        {
        // <qmail> removing case EStateQueryingDetails
        case EStateStartConnect:
            // <qmail> remove StartL() func and replace it directly with DoConnectOpL
            DoConnectOpL();
            break;
        case EStateConnectAndSync:
            // Connection completed
                
            // <qmail> all errors should fail the op
            if( err )
                {
                iState = EStateIdle;
                // <qmail> SetActive(); moved inside CompleteThis();
                CompleteThis();
                }
            else
                {
                // no errors in connection
                if( !iDoPlainConnect )
                    {
                    DoPopulateAllL();
                    }
                else
                    {
                    // Get on with others using this class for connection only
                    iState = EStateIdle;
                    // <qmail> SetActive(); moved inside CompleteThis();
                    CompleteThis();
                    }
                }
            break;
        case EStatePopulateAllCompleted:
            CIpsPlgSyncStateHandler::SaveSuccessfulSyncTimeL( iMsvSession, iService );
            // break command is intentially left out
        case EStateCompleted:
            if ( err == KErrNone )
                {
                iState = EStateIdle;
                // <qmail> SetActive(); moved inside CompleteThis();
                CompleteThis();
                }
            break;
        case EStateIdle:
        default:
            if ( iSubOperation )
                {
                delete iSubOperation;
                iSubOperation = NULL;
                }
            CompleteObserver();
            break;
        }
   
    // if iError < 0, observer is completed in base class
    iError = err;
    if ( err != KErrNone )
        {
        SignalSyncCompleted( err );
        }
    }

// <qmail> CIpsPlgImap4ConnectOp::HandleImapConnectionEvent() removed

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::RequestResponseL()
// ----------------------------------------------------------------------------    
//   
void CIpsPlgImap4ConnectOp::RequestResponseL( 
    TFSProgress /*aEvent*/, TInt /*aRequestId*/ )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoConnectOpL()
// ----------------------------------------------------------------------------    
//   
void CIpsPlgImap4ConnectOp::DoConnectOpL()
	{
    FUNC_LOG;
    iBaseMtm->SwitchCurrentEntryL( iService );

    iSelection->ResizeL(0);
    iSelection->AppendL(iService);
    
    // <qmail>
    iStatus = KRequestPending;
    if ( iDoPlainConnect && !Connected() )
        {
        TBuf8<1> parameter;
        NM_COMMENT("CIpsPlgImap4ConnectOp: do plain connect");
        // connect and synchronise starts background sync or idle
        iSubOperation = iBaseMtm->InvokeAsyncFunctionL(
            KIMAP4MTMConnect, *iSelection, parameter, iStatus);
        }
    else if ( Connected() )
        {
        // in this point cant use "connect and do something" commands,
        // use regular sync, when new mails is populated elsewhere.
        TBuf8<1> parameter;
        NM_COMMENT("CIpsPlgImap4ConnectOp: full sync starting");
        iSubOperation = iBaseMtm->InvokeAsyncFunctionL(
            KIMAP4MTMFullSync, *iSelection, parameter, iStatus);
        }
    else
        {
        // the used command requires an observer to be given even though we're not using it
        NM_COMMENT("CIpsPlgImap4ConnectOp: connect and sync");
        TPckg<MMsvImapConnectionObserver*> parameter( NULL );
        iSubOperation = iBaseMtm->InvokeAsyncFunctionL(
            KIMAP4MTMConnectAndSyncCompleteAfterFullSync, 
            *iSelection, parameter, iStatus );
        }
    SignalSyncStarted();
    iState = EStateConnectAndSync;
    SetActive();
    // </qmail>
	}

// ----------------------------------------------------------------------------
// CIpsPlgImap4ConnectOp::DoPopulateAll()
// ---------------------------------------------------------------------------- 
void CIpsPlgImap4ConnectOp::DoPopulateAllL()
    {
    FUNC_LOG;
    
    NM_COMMENT("CIpsPlgImap4ConnectOp: populate all");
    // construct partial fetch info according to imap settings
    // <qmail> new function to wrap settings loading
    CImImap4Settings* settings = GetImapSettingsLC();
    TImImap4GetPartialMailInfo info;
    ConstructImapPartialFetchInfo( info, *settings );
    TPckgBuf<TImImap4GetPartialMailInfo> package(info);
    CleanupStack::PopAndDestroy( settings );
    settings = NULL;
    
    // do populate operation unless only headers should be fetched
    if ( info.iTotalSizeLimit != KIpsSetDataHeadersOnly )
        {
        // Update iSelection (which will be given to populate op)
        // <qmail> new function to wrap iSelection populating
        CreateInboxMessagesSelectionL();
        
        iStatus = KRequestPending;
        iBaseMtm->SwitchCurrentEntryL( iService );
        // <qmail> priority parameter has been removed
        iSubOperation = CIpsPlgImap4PopulateOp::NewL(
            iMsvSession,
            iStatus,
            iService,
            // <qmail> passing in activity timer; only place where NULL "reference" was used
            // <qmail> -> can get rid of checking existence of a reference memeber...
            *iActivityTimer,
            info,
            *iSelection,
            iFSMailboxId,
            NULL, // no observer for suboperation
            0, // requestId can be 0
            iEventHandler );
        
        SetActive();
        // <qmail> SignalSyncStarted signal moved to happen later in case something goes wrong
        SignalSyncStarted();
        }
    else
        {
        // <qmail> SetActive(); moved inside CompleteThis();
        CompleteThis();
        }
    iState = EStatePopulateAllCompleted;
    }

// <qmail> removed CIpsPlgImap4ConnectOp::StartL()
// <qmail> removed CIpsPlgImap4ConnectOp::QueryUserPwdL()
// <qmail> removed CIpsPlgImap4ConnectOp::GetOperationErrorCodeL()

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------       
void CIpsPlgImap4ConnectOp::SignalSyncStarted()
    {
    FUNC_LOG;
    if ( iEventHandler && !iIsSyncStartedSignaled )
        {
        // mark that sync is signaled to prevent 
        // sending necessary event
        iIsSyncStartedSignaled = ETrue;
        iEventHandler->SetNewPropertyEvent( iService, KIpsSosEmailSyncStarted, KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------       
void CIpsPlgImap4ConnectOp::SignalSyncCompleted( TInt aError )
    {
    FUNC_LOG;
    // <qmail> don't test for aError code; all situations should complete op
    if ( iEventHandler )
        {
        iEventHandler->SetNewPropertyEvent( iService, KIpsSosEmailSyncCompleted, aError );
        iIsSyncStartedSignaled = EFalse;
        }
    }

// <qmail> Removing CIpsPlgImap4ConnectOp::CredientialsSetL 

// <qmail> new functions added
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4ConnectOp::ConstructImapPartialFetchInfo( 
    TImImap4GetPartialMailInfo& aInfo, 
    const CImImap4Settings& aImap4Settings )
    {
    FUNC_LOG;
    TInt sizeLimit = aImap4Settings.BodyTextSizeLimit();
    
    if ( sizeLimit == KIpsSetDataHeadersOnly )
        {
        aInfo.iTotalSizeLimit = KIpsSetDataHeadersOnly;
        }
    else if ( sizeLimit == KIpsSetDataFullBodyAndAttas )
        {        
        aInfo.iTotalSizeLimit = KMaxTInt;
        aInfo.iAttachmentSizeLimit = KMaxTInt;
        aInfo.iBodyTextSizeLimit = KMaxTInt;
        aInfo.iMaxEmailSize = KMaxTInt;
        aInfo.iPartialMailOptions = ENoSizeLimits;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
        }
    else if ( sizeLimit == KIpsSetDataFullBodyOnly )
        {
        aInfo.iTotalSizeLimit = KMaxTInt; 
        aInfo.iAttachmentSizeLimit = 0;
        aInfo.iBodyTextSizeLimit = KMaxTInt;
        aInfo.iMaxEmailSize = KMaxTInt;
        aInfo.iPartialMailOptions = EBodyAlternativeText;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyAlternativeText;
        }
    else
        {
    //<qmail> include html in body
        aInfo.iTotalSizeLimit = sizeLimit*1024; 
        aInfo.iAttachmentSizeLimit = 0;
        aInfo.iMaxEmailSize = sizeLimit*1024;
        aInfo.iBodyTextSizeLimit = sizeLimit*1024;
        aInfo.iPartialMailOptions = EBodyAlternativeText;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyAlternativeText;
    // </qmail>    
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CImImap4Settings* CIpsPlgImap4ConnectOp::GetImapSettingsLC()
    {
    FUNC_LOG;
    CImImap4Settings* settings = new ( ELeave ) CImImap4Settings();
    CleanupStack::PushL( settings );
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TImapAccount imapAcc;
    accounts->GetImapAccountL(iService, imapAcc );
    accounts->LoadImapSettingsL( imapAcc, *settings );
    CleanupStack::PopAndDestroy( accounts );
    return settings;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4ConnectOp::CreateInboxMessagesSelectionL()
    {
    FUNC_LOG;
    // get children of service entry 
    CMsvEntry* cEntry = iMsvSession.GetEntryL( iService );
    CleanupStack::PushL( cEntry );
    CMsvEntrySelection* childrenSelection = cEntry->ChildrenL();
    CleanupStack::PopAndDestroy( cEntry );
    cEntry = NULL;
    CleanupStack::PushL( childrenSelection );

    if ( childrenSelection->Count() )
        {
        TMsvId id = (*childrenSelection)[0]; // index 0 == inbox
        cEntry = iMsvSession.GetEntryL( id ); // reusing cEntry pointer for Inbox entry
        CleanupStack::PushL( cEntry );
        delete iSelection;
        iSelection = NULL;
        // get message-type children of inbox
        iSelection = cEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
        CleanupStack::PopAndDestroy( cEntry );
        }
    CleanupStack::PopAndDestroy( childrenSelection );
    }

// </qmail>
