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
* Description:  IMAP4 fetch message operation
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> priority const has been removed

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
// <qmail> MFSMailRequestObserver& changed to pointer
EXPORT_C CIpsPlgImap4PopulateOp* CIpsPlgImap4PopulateOp::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImImap4GetPartialMailInfo& aPartialMailInfo,
    const CMsvEntrySelection& aSel,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler,
    TBool aDoFilterSelection )
    {
    FUNC_LOG;
    CIpsPlgImap4PopulateOp* op = new (ELeave) CIpsPlgImap4PopulateOp(
        aMsvSession,
        aObserverRequestStatus,
        aService,
        aActivityTimer,
        aPartialMailInfo,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId,
        aEventHandler );
        
    CleanupStack::PushL(op);
    op->ConstructL( aSel, aDoFilterSelection );
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
CIpsPlgImap4PopulateOp::CIpsPlgImap4PopulateOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImImap4GetPartialMailInfo& aPartialMailInfo,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler )
    :
    CIpsPlgOnlineOperation(
        aMsvSession,
        aObserverRequestStatus,
        aActivityTimer,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId ),
    iState( EStateIdle ),
    iPartialMailInfo(aPartialMailInfo),
    iSelection( NULL ),
    iTempSelection( NULL ),
    iEventHandler( aEventHandler ),
    iFetchErrorProgress( NULL )
    {
    FUNC_LOG;
    iService = aService;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4PopulateOp::~CIpsPlgImap4PopulateOp()
    {
    FUNC_LOG;
    delete iSelection;

    if ( iTempSelection )
    	{
    	iTempSelection->Reset();
   		delete iTempSelection;
    	}
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::ConstructL( 
        const CMsvEntrySelection& aSel,
        TBool aDoFilterSelection )
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypeIMAP4 );
    
    // <qmail> instantiation moved here
    iSelection = new ( ELeave ) CMsvEntrySelection();
    iTempSelection = new ( ELeave ) CMsvEntrySelection();

    if ( aDoFilterSelection )
        {
        FilterSelectionL( aSel );
        }
    else // no filtering; populate all
        {
        // first entry in selection is serviceId
        for ( TInt i = 1; i < aSel.Count(); i++ )
            {
            if ( aSel[i] != iService )
                {
                iSelection->AppendL( aSel.At(i) );
                }
            }
        }
    DoConnectL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> return type changed: TInt -> TIpsOpType
TIpsOpType CIpsPlgImap4PopulateOp::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeImap4PopulateOp;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::DoConnectL()
    {
    FUNC_LOG;
    iState = EStateConnecting;
    iStatus = KRequestPending;

    // <qmail> priority parameter has been removed
    CIpsPlgImap4ConnectOp* connOp = CIpsPlgImap4ConnectOp::NewL(
        iMsvSession,
        iStatus, 
        iService,
        *iActivityTimer,
        iFSMailboxId,
        NULL, // no observer for suboperations
        0,    // requestId not needed
        iEventHandler,
        ETrue ); // Do plain connect
        
    delete iSubOperation;
    iSubOperation = connOp;

    SetActive();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::RunL()
    {
    FUNC_LOG;
    TRAPD(err, DoRunL());
    if(err != KErrNone )
        {
        iSyncProgress().iErrorCode = err;
        Complete();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::DoRunL()
    {
    FUNC_LOG;
    TInt err = iStatus.Int();
    delete iSubOperation;
    iSubOperation = NULL;
    
    switch( iState )
        {
        case EStateConnecting:
            {
            TMsvEntry tentry;
            TMsvId service;
            iMsvSession.GetEntry(iService, service, tentry );
            if( !tentry.Connected() )
                {
                CompleteObserver( KErrCouldNotConnect );
                return;
                }
            DoPopulateL();
            break;
            }
        case EStateFetching:         
            {
            if( err != KErrNone && iSubOperation )
                {
                iFetchErrorProgress = iSubOperation->ProgressL().AllocL();
                iState = EStateIdle;
                Complete();
                }
            break;
            }
        case EStateInfoEntryChange:
            {
            DoPopulateL();
            break;
            }
        case EStateIdle:
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::DoCancel()
    {
    FUNC_LOG;
    if( iSubOperation )
        {
        iSubOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgImap4PopulateOp::ProgressL()
    {
    FUNC_LOG;
    if(iFetchErrorProgress && (iState == EStateIdle))
        {
        // Completed, but with an error during fetch.
        return *iFetchErrorProgress;
        }
    else
        {
        TImap4SyncProgress progg = TImap4SyncProgress();
    	progg.iFoldersNotFound = 0;
        progg.iErrorCode = KErrNone;
        TPckgBuf<TImap4SyncProgress> param(progg);
        iSyncProgress.Copy(param);
        return iSyncProgress;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgImap4PopulateOp::GetErrorProgressL(TInt /*aError*/ )
    {
    FUNC_LOG;
    return *iFetchErrorProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgImap4PopulateOp::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    switch( iState )
        {
        case EStateConnecting:
            result.iProgressStatus = TFSProgress::EFSStatus_Connecting;
            break;
        case EStateFetching:
            result.iProgressStatus = TFSProgress::EFSStatus_Connected;
            break;
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

// <qmail> removed CIpsPlgImap4PopulateOp::GetEngineProgress( const TDesC8& aProgress )

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::Complete()
    {
    FUNC_LOG;
    TRequestStatus* observer=&iObserverRequestStatus;
    User::RequestComplete(observer, KErrNone);
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::FilterSelectionL( const CMsvEntrySelection& aSelection )
    {
    FUNC_LOG;
    iSelection->Reset();
    TMsvId messageId;
    // NOTE: this code is taken from symbian os source IMPCMTM.CPP
    // filter selection is in here because messages are
    // fetched separately then we dont have to make unneccessery imap 
    // client mtm calls

    for (TInt i=0; i<aSelection.Count(); i++)
        {
        messageId = (aSelection)[i];
        if ( messageId == iService )
            {
            continue; // ignore serviceId
            }
        TMsvEmailEntry entry;
        TMsvId service = KMsvNullIndexEntryId;          
        User::LeaveIfError(iMsvSession.GetEntry(messageId, service, entry));
        
        TBool isComplete = !(   ( entry.Complete() && entry.PartialDownloaded() ) 
                             || ( !entry.Complete() && ( !entry.BodyTextComplete() 
                             || ( iPartialMailInfo.iGetMailBodyParts == EGetImap4EmailBodyTextAndAttachments ) ) 
                                  && !entry.PartialDownloaded() ) );

        TBool isMsgEntry = entry.iType == KUidMsvMessageEntry;
        TBool isSizeUnderMax = entry.iSize <= iPartialMailInfo.iMaxEmailSize;
        TBool isParentComp = entry.Parent() == 
            iPartialMailInfo.iDestinationFolder && isComplete;
        
        if( IsPartialPopulate( )
              && !isComplete
              && entry.iType == KUidMsvMessageEntry )
            {
            iSelection->AppendL( messageId ); 
            }
        else if ( isMsgEntry && isSizeUnderMax && !isParentComp )
            {
            iSelection->AppendL( messageId );
            }
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsPlgImap4PopulateOp::IsPartialPopulate( )
    {
    // <qmail> cleaned up code and took one "always true" condition out from the if statement
    FUNC_LOG;
    // NOTE: this code is taken from symbian os source IMPCMTM.CPP
    // code is modified to this class purpose 
    TBool isPartialPopulate( EFalse );
    if ( iPartialMailInfo.iPartialMailOptions == ENoSizeLimits &&
         iPartialMailInfo.iTotalSizeLimit == KMaxTInt &&
         iPartialMailInfo.iBodyTextSizeLimit == KMaxTInt && 
         iPartialMailInfo.iAttachmentSizeLimit == KMaxTInt )
        {
        isPartialPopulate = EFalse;
        }
    else
        {
        isPartialPopulate = ETrue;
        }
    return isPartialPopulate;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4PopulateOp::DoPopulateL( )
    {
    FUNC_LOG;
    if ( iSelection->Count() > 0 )
        {
        TMsvEmailEntry tEntry;
        TMsvId dummy;
        TInt lastIndex = iSelection->Count()-1;
        User::LeaveIfError( iMsvSession.GetEntry( iSelection->At(lastIndex), dummy, tEntry ) );
        
        iState = EStateFetching;

        iTempSelection->Reset();
        iTempSelection->AppendL( iService );
        iTempSelection->AppendL( iSelection->At( lastIndex ) );
        iSelection->Delete( lastIndex );
            
        // Filters are not used when performing 'fetch'
        // operation, use normal getmail info instead
        TPckg<TImImap4GetPartialMailInfo> param( iPartialMailInfo );
        iBaseMtm->SwitchCurrentEntryL( iService );
        iSubOperation = iBaseMtm->InvokeAsyncFunctionL(
            KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected, 
            *iTempSelection, param, iStatus );
        iState = EStateInfoEntryChange;
        SetActive();
        }
    else
        {
        iState = EStateIdle;
        CompleteObserver();
        }
    }

// End of File
