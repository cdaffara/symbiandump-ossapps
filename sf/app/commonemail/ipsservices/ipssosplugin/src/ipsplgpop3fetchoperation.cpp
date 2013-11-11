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
* Description:  POP3 fetch message operation class
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> priority const has been removed
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> MFSMailRequestObserver& changed to pointer
CIpsPlgPop3FetchOperation* CIpsPlgPop3FetchOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImPop3GetMailInfo& aGetMailInfo,
    CMsvEntrySelection* aSelection,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    CIpsPlgEventHandler* aEventHandler )
    {
    FUNC_LOG;
    CIpsPlgPop3FetchOperation* op = new(ELeave) CIpsPlgPop3FetchOperation(
        aMsvSession,
        aObserverRequestStatus, 
        aService,
        aActivityTimer, 
        aGetMailInfo,
        aSelection,
        aFSMailBoxId, 
        aFSOperationObserver,
        aFSRequestId, 
        aEventHandler );
        
    CleanupStack::PushL(op);
    op->ConstructL();
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// ~CIpsPlgPop3FetchOperation
// ----------------------------------------------------------------------------
CIpsPlgPop3FetchOperation::~CIpsPlgPop3FetchOperation()
    {
    FUNC_LOG;
    delete iFetchErrorProgress;
    delete iSelection;
    
    delete iEntry;
    }

// ----------------------------------------------------------------------------
// ProgressL
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgPop3FetchOperation::ProgressL()
    {
    FUNC_LOG;
    
    if(iFetchErrorProgress && (iState == EStateIdle))
        {
        // Completed, but with an error during fetch.
        return *iFetchErrorProgress;
        }
    else
        {
        TPop3Progress progg = TPop3Progress();
        progg.iTotalSize = 0;
        progg.iErrorCode = KErrNone;
        TPckgBuf<TPop3Progress> param(progg);
        iProgressBuf.Copy(param);
        return iProgressBuf;   
        }
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::ConstructL()
    {
    FUNC_LOG;

    BaseConstructL( KUidMsgTypePOP3 );
    // <qmail> 1st entry must be serviceId
    iSelection->InsertL( 0, iService );
    // For Get Mail API, first selection element must be service.

    // It is possible that the complete flag is on and we want to
    // download the attachment. In these cases we need to clear the
    // complete flag.

    TInt count = iSelection->Count();

    for ( ; iEntryIndex < count && !iSubOperation; iEntryIndex++ )
        {
        delete iEntry;
        iEntry = NULL;
        
        iEntry = iMsvSession.GetEntryL( iSelection->At( iEntryIndex ) );

        TMsvEntry entry = iEntry->Entry();
        
        TBool complete = entry.Complete();

        if ( complete )
            {
            entry.SetComplete( EFalse );

            iSubOperation = iEntry->ChangeL( entry, iStatus );

            SetActive();
            
            iState = EStateClearCompleteFlag;
            }
        }

    if ( !iSubOperation )
        {
        DoConnectL();
        }
    }

// ----------------------------------------------------------------------------
// DoConnectL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoConnectL()
    {
    FUNC_LOG;
    

    iState = EStateConnecting;
    iStatus = KRequestPending;
    // <qmail>
    CIpsPlgPop3ConnectOp* connOp = CIpsPlgPop3ConnectOp::NewL(
        iMsvSession,
        iStatus, 
        iService, 
        EFalse, // We do fetch by ourselves
        *iActivityTimer,
        iFSMailboxId, 
        iFSOperationObserver, 
        iFSRequestId, 
        NULL,
        EFalse, // Signalling not allowed
        ETrue ); // Fetch will follow, let connection be open
        
    // </qmail>    
    delete iSubOperation;
    iSubOperation = connOp;

    SetActive();
    }

// ----------------------------------------------------------------------------
// DoFetchL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoFetchL()
    {
    FUNC_LOG;

    iState = EStateFetching;

    // <qmail> deletion of possible previous iSubOperaiton is handled in InvokeClientMtmAsyncFunctionL
    // <qmail> delete iSubOperation;
    // <qmail> iSubOperation = NULL;
    iStatus = KRequestPending;

    // Filters are not used when performing 'fetch' operation, use normal 
    // getmail info instead
    TPckg<TImPop3GetMailInfo> param( iGetMailInfo );
    // <qmail> using constant functionId as its always the same
    InvokeClientMtmAsyncFunctionL( KPOP3MTMCopyMailSelectionWhenAlreadyConnected, *iSelection, param );
    SetActive();
    
    if ( iEventHandler )
        {
        iEventHandler->SetNewPropertyEvent( 
            iService, KIpsSosEmailSyncStarted, KErrNone );
        } 
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
// <qmail> MFSMailRequestObserver& changed to pointer
// <qmail> aFunctionId parameter has been removed
// <qmail> aSelection parameter has been added
CIpsPlgPop3FetchOperation::CIpsPlgPop3FetchOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const TImPop3GetMailInfo& aGetMailInfo,
    CMsvEntrySelection* aSelection,
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
    iGetMailInfo( aGetMailInfo ), 
    iSelection( aSelection ),
    iEventHandler( aEventHandler )
    {
    FUNC_LOG;
    iService = aService;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::RunL()
    {
    FUNC_LOG;

    TRAP( iError, DoRunL() );
    
    if( iError )
        {        
        // Notify observer we have finished.
        CompleteObserver();
        }    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoCancel()
    {
    FUNC_LOG;

    CIpsPlgOnlineOperation::DoCancel();
    if(iState == EStateFetching)
        {
        // Stops any operations that a Server-side MTM for the specified service is running,
        // and then unloads the Server-side MTM.
        // The current operation and any queued operations are cancelled.
        iMsvSession.StopService( iService );
        }
    if ( iEventHandler )
        {
        // <qmail>
        iEventHandler->SetNewPropertyEvent( iService, KIpsSosEmailSyncCompleted, KErrCancel );
		// </qmail>
        }
    }

// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoRunL()
    {
    FUNC_LOG;

    switch(iState)
        {
        case EStateClearCompleteFlag:
            {
            // First clean things
            delete iSubOperation;
            iSubOperation = NULL;

            delete iEntry;
            iEntry = NULL;
            
            // Leave if setting the complete flag was a failure.
            User::LeaveIfError( iStatus.Int() );
            
            TInt count = iSelection->Count();

            for ( ; iEntryIndex < count && !iSubOperation; iEntryIndex++ )
                {
                delete iEntry;
                iEntry = NULL;
                
                iEntry = iMsvSession.GetEntryL(
                    iSelection->At( iEntryIndex ) );

                TMsvEntry entry = iEntry->Entry();
                
                TBool complete = entry.Complete();

                if ( complete )
                    {
                    entry.SetComplete( EFalse );

                    iSubOperation = iEntry->ChangeL( entry, iStatus );

                    SetActive();
                    }
                }

            if ( !iSubOperation )
                {
                DoConnectL();
                }
            
            break;
            }
        case EStateConnecting:
            {
            // Connect complete.
            // <qmail> Connected() usage
            if ( !Connected() )
                {
                CompleteObserver( KErrCouldNotConnect );
                return;
                }
            DoFetchL();
            }
            break;
        case EStateFetching:         
            {
            TInt err = iStatus.Int();
			// <qmail>
            if( err )
			// </qmail>
                {
                TPckgBuf<TPop3Progress> paramPack;
                if ( iSubOperation )
                    {
                    paramPack.Copy( iSubOperation->ProgressL() );
                    }
                TPop3Progress& progress = paramPack();
                progress.iErrorCode = err;
                iFetchErrorProgress = paramPack.AllocL();
                }
            DoDisconnectL();
            
            if ( iEventHandler )
                {
				// <qmail>
                iEventHandler->SetNewPropertyEvent( iService, KIpsSosEmailSyncCompleted, err );
				// </qmail>
                }
            }
            break;
        case EStateDisconnecting:
            {
            CompleteObserver( iStatus.Int() );
            }
            break;
            
        default:
            break;
        }        
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgPop3FetchOperation::GetErrorProgressL( TInt /* aError */ )
    {
    FUNC_LOG;

    return *iFetchErrorProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgPop3FetchOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    // <qmail> removed result.iError = KErrNone;
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

// <qmail> new func to this op
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgPop3FetchOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypePop3FetchOp;
    }
// </qmail>

//<qmail> new function
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPop3FetchOperation::DoDisconnectL()
    {
    FUNC_LOG;
    iState = EStateDisconnecting;
    InvokeClientMtmAsyncFunctionL( KPOP3MTMDisconnect, iService ); // <qmail> 1 param removed
    SetActive();
    }
// </qmail>


