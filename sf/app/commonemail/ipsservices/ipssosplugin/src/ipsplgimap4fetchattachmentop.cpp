/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*       IpsPlgImap4FetchAttachmentOp implementation file
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> priority const has been removed, interval changed to 0.2 sec
const TInt KIpsAttaFetchProgressReportInterval = 200000; // 0.2 sec
const TInt KIpsAttaFetchRetryInterval = 1000000; // 1 sec
const TInt KIpsAttaFetchRetryCount = 30;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsFetchProgReport* CIpsFetchProgReport::NewL( 
    CIpsPlgImap4FetchAttachmentOp& aAttaOp )
    {
    FUNC_LOG;
    CIpsFetchProgReport* self = new (ELeave) CIpsFetchProgReport( aAttaOp );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
CIpsFetchProgReport::CIpsFetchProgReport( 
    CIpsPlgImap4FetchAttachmentOp& aAttaOp ) : 
    CActive( CActive::EPriorityStandard ), 
    iAttaOp(aAttaOp)
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
CIpsFetchProgReport::~CIpsFetchProgReport()
    {
    FUNC_LOG;
    Cancel();
    iTimer.Close();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------	    
void CIpsFetchProgReport::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    User::LeaveIfError( iTimer.CreateLocal() );
    AdjustTimer();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsFetchProgReport::AdjustTimer()   
    {
    FUNC_LOG;
    TTimeIntervalMicroSeconds32 sec( KIpsAttaFetchProgressReportInterval ); 
    TTime time;
    time.HomeTime();
    time = time + sec;
    iTimer.At(iStatus, time);
    SetActive();    
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
void CIpsFetchProgReport::DoCancel()
    {
    FUNC_LOG;
    iTimer.Cancel();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------	
void CIpsFetchProgReport::RunL()
    {
    FUNC_LOG;
    iAttaOp.ReportProgressL();
    AdjustTimer();
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4FetchAttachmentOp
// ----------------------------------------------------------------------------
// <qmail> aFunctionId removed, CMsvEntrySelection& changed to pointer
// <qmail> MFSMailRequestObserver& changed to pointer
CIpsPlgImap4FetchAttachmentOp* CIpsPlgImap4FetchAttachmentOp::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const CMsvEntrySelection* aSelection,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId )
    {
    FUNC_LOG;
    CIpsPlgImap4FetchAttachmentOp* op = new (
        ELeave) CIpsPlgImap4FetchAttachmentOp(
        aMsvSession,
        aObserverRequestStatus,
        aService,
        aActivityTimer,
        aSelection,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId );
        
    CleanupStack::PushL( op );
    op->ConstructL();
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
// <qmail> MFSMailRequestObserver& changed to pointer
CIpsPlgImap4FetchAttachmentOp::CIpsPlgImap4FetchAttachmentOp(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService,
    CIpsPlgTimerOperation& aActivityTimer,
    const CMsvEntrySelection* aSelection,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId )
    :
    CIpsPlgOnlineOperation(
        aMsvSession,
        aObserverRequestStatus,
        aActivityTimer,
        aFSMailBoxId,
        aFSOperationObserver,
        aFSRequestId ),
    	iSelection( aSelection ),
    	iRetryCount( 0 )
    {
    FUNC_LOG;
    iService = aService;
    iRetryTimer.CreateLocal();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgImap4FetchAttachmentOp::~CIpsPlgImap4FetchAttachmentOp()
    {
    FUNC_LOG;
    iRetryTimer.Close();
    delete iSelection;
    delete iProgReport;
    delete iFetchErrorProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( KUidMsgTypeIMAP4 );
    DoConnectL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::DoConnectL()
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
        NULL, // no observer for subopertaion
        0,    // no requestId needed
        NULL, // event handler not needed whin plain connect
        ETrue );
        
    delete iSubOperation;
    iSubOperation = connOp;

    SetActive();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::RunL()
    {
    FUNC_LOG;
    TRAPD(err, DoRunL());
    if(err != KErrNone)
        {
        iProgress().iGenericProgress.iErrorCode = err;
        Complete();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::DoRunL()
    {
    FUNC_LOG;
    switch( iState )
        {
        case EStateConnecting:
            {
            // <qmail> Connected() usage
            if ( !Connected() )
                {
                CompleteObserver( KErrCouldNotConnect );
                return;
                }
            iRetryCount = 0;
            DoFetchAttachmentL();
            break;
            }
        case EStateWaiting:
            {
            DoFetchAttachmentL();
            break;
            }
        case EStateFetching:         
            {
            delete iProgReport;
            iProgReport = NULL;
            
            TInt err = iStatus.Int();

            // If the server was busy, try again after a short delay, up to
            // some retry limit.
            if ( err == KErrServerBusy && iRetryCount < KIpsAttaFetchRetryCount )
                {
                iRetryCount++;
                INFO_1( "CIpsPlgImap4FetchAttachmentOp::DoRunL: iRetryCount = %d", iRetryCount );
                iState = EStateWaiting;
                iStatus = KRequestPending;
                iRetryTimer.After( iStatus, KIpsAttaFetchRetryInterval );
                SetActive();
                }
            else
                {
                if( err != KErrNone && iSubOperation )
                    {
                    iFetchErrorProgress = iSubOperation->ProgressL().AllocL();
                    }
                
                iState = EStateIdle;
                CompleteObserver( err );
                }
            break;
            }
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgImap4FetchAttachmentOp::ProgressL()
    {
    FUNC_LOG;
    if(iFetchErrorProgress && (iState == EStateIdle))
        {
        // Completed, but with an error during fetch.
        return *iFetchErrorProgress;
        }
    
    if ( iSubOperation )
        {
        iProgress.Copy( iSubOperation->ProgressL() );
        }
    else
        {
        TImap4CompoundProgress progg;
        progg.iGenericProgress.iErrorCode = KErrNone;
        progg.iGenericProgress.iState = TImap4GenericProgress::EIdle;
        iProgress.Copy( TPckgBuf<TImap4CompoundProgress>(progg) );
        }
           
    return iProgress;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::ReportProgressL()
    {
    FUNC_LOG;
    TInt error = KErrNone;
    TFSProgress fsProgress = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    if ( iSubOperation && iState == EStateFetching )
        {
        TRAP(error, iProgress.Copy( iSubOperation->ProgressL() ) );
        }
    
    if ( error == KErrNone )
        {
        const TImap4GenericProgress& progg = iProgress().iGenericProgress;
        fsProgress.iProgressStatus = TFSProgress::EFSStatus_Status;
        fsProgress.iMaxCount = progg.iBytesToDo;
        fsProgress.iCounter = progg.iBytesDone;
        fsProgress.iError = progg.iErrorCode;
        }
    else if ( error == KErrNotReady )
        {
        // This error indicates that operation not started yet
        // and waiting to other operation compleion
        fsProgress.iProgressStatus = TFSProgress::EFSStatus_Waiting;
        fsProgress.iMaxCount = 1;
        fsProgress.iCounter = 0;
        fsProgress.iError = KErrNone;
        }
    else
        {
        User::Leave( error );
        }

// <qmail>
    // signal observer if it exists
    if ( iFSOperationObserver )
        {
        iFSOperationObserver->RequestResponseL( fsProgress, iFSRequestId );
        }
// </qmail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgImap4FetchAttachmentOp::GetErrorProgressL(TInt /*aError*/ )
    {
    return *iFetchErrorProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgImap4FetchAttachmentOp::GetFSProgressL() const
    {
    FUNC_LOG;
    // might not never called, but gives something reasonable if called
    TFSProgress result = { TFSProgress::EFSStatus_Waiting, 0, 0, KErrNone };
    result.iError = KErrNone;
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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::Complete()
    {
    FUNC_LOG;
    TRequestStatus* observer=&iObserverRequestStatus;
    User::RequestComplete(observer, KErrNone);
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgImap4FetchAttachmentOp::DoFetchAttachmentL( )
    {
    FUNC_LOG;
    iState = EStateFetching;

    // Switch operations.
    delete iSubOperation;
    iSubOperation = NULL;
    iStatus = KRequestPending;

    iProgReport = CIpsFetchProgReport::NewL( *this );

    // Filters are not used when performing 'fetch' operation, 
    // use normal getmail info instead
// <qmail>
    TPckgBuf<TImImap4GetMailInfo> param;
    TImImap4GetMailInfo& options = param();
    options.iMaxEmailSize = KMaxTInt32;
    options.iGetMailBodyParts = EGetImap4EmailAttachments;
    options.iDestinationFolder = 0; // not used

    InvokeClientMtmAsyncFunctionL( KIMAP4MTMPopulate, *iSelection, param ); // <qmail> 1 param removed
// </qmail>
    SetActive();
    }
    
// <qmail> GetEngineProgress function removed

// <qmail> new func to this op
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgImap4FetchAttachmentOp::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeFetchAttachmentOp;
    }
// </qmail>


// End of File

