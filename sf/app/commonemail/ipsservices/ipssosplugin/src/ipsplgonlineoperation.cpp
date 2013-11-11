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
* Description:  Base class for online operations
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::~CIpsPlgOnlineOperation()
// ----------------------------------------------------------------------------
//
CIpsPlgOnlineOperation::~CIpsPlgOnlineOperation()
    {
    FUNC_LOG;
    Cancel();   
    delete iSubOperation;
    delete iBaseMtm;
    delete iMtmReg;
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::CIpsPlgOnlineOperation()
// ----------------------------------------------------------------------------
// <qmail> priority parameter has been removed
// <qmail> MFSMailRequestObserver& changed to pointer
// <qmail> aSignallingAllowed parameter has been returned
CIpsPlgOnlineOperation::CIpsPlgOnlineOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CIpsPlgTimerOperation& aActivityTimer,
    TFSMailMsgId aFSMailBoxId,
    MFSMailRequestObserver* aFSOperationObserver,
    TInt aFSRequestId,
    TBool aSignallingAllowed )
    :
    CIpsPlgBaseOperation(
        aMsvSession, 
        aObserverRequestStatus,
        aFSRequestId, 
        aFSMailBoxId ), 
    iActivityTimer( &aActivityTimer ),
    iBaseMtm( NULL ),
    iMtmReg( NULL ), 
    iSubOperation( NULL ),
    iError( KErrNone ),
    iSignallingAllowed( aSignallingAllowed ),
    iFSOperationObserver( aFSOperationObserver )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::BaseConstructL()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::BaseConstructL( TUid aMtmType )
    {
    FUNC_LOG;
    // reset timer, if operation not completed after timer fires causes
    // disconnection
    // <qmail> remove cheking of existence of this reference member
    if (iActivityTimer)
        {
        iActivityTimer->ResetTimerOperation();
        }

    iMtmReg = CClientMtmRegistry::NewL( iMsvSession );
    iBaseMtm = iMtmReg->NewMtmL( aMtmType );
    iObserverRequestStatus = KRequestPending;
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::DoCancel()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::DoCancel()
    {
    FUNC_LOG;
    if( iSubOperation )
        {
        iSubOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::RunL()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::RunL()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TRAP( err, DoRunL() );
    
    // Just end the operation, if something has gone wrong
    if ( err )
        {
        CompleteObserver( err );
        }
    else if ( iError )
        {
        CompleteObserver( iError );
        }
    }

// <qmail> removing TInt CIpsPlgOnlineOperation::RunError( TInt aError )

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::CompleteObserver()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::CompleteObserver( TInt aStatus )
    {
    FUNC_LOG;
    NM_COMMENT("CIpsPlgOnlineOperation::CompleteObserver");
    TRequestStatus* status = &iObserverRequestStatus;
    if (status && status->Int() == KRequestPending)
        {
        SignalFSObserver( aStatus );
        // <qmail>
        // removed checks to prevent unwanted disconnections
        //if connected, reset activitytimer. if not, there is no reason to.
        if ( Connected() )
            {
            NM_COMMENT("CIpsPlgOnlineOperation::reseting activitytimer");
            iActivityTimer->ResetTimerOperation();
            }
        else
            {
            iActivityTimer->Cancel();
            }
        // </qmail>
        User::RequestComplete(status, aStatus);
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::CompleteThis()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::CompleteThis()
    {
    FUNC_LOG;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL()
// ----------------------------------------------------------------------------
// <qmail> remove contextId as not needed
void CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL(
    TInt aFunctionId,
    TMsvId aEntryId,
    const TDesC8& aParams)
    {
    FUNC_LOG;
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( sel );
    sel->AppendL( aEntryId );
    InvokeClientMtmAsyncFunctionL(aFunctionId, *sel, aParams);
    CleanupStack::PopAndDestroy( sel ); 
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL()
// ----------------------------------------------------------------------------
// <qmail> remove contextId as not needed
void CIpsPlgOnlineOperation::InvokeClientMtmAsyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSel,
    const TDesC8& aParams )
    {
    FUNC_LOG;
    
    TMsvEntry tEntry;
    TMsvId service;
    if ( aSel.Count() )
        {
        iMsvSession.GetEntry( aSel.At(0), service, tEntry );
        }

    // setting our context to serviceId
    iBaseMtm->SwitchCurrentEntryL( tEntry.iServiceId );

    HBufC8* params = aParams.AllocLC(); // can not pass const parameter to InvokeAsyncFunctionL
    TPtr8 ptr(params->Des());
    // Delete previous operation if it exist
    if ( iSubOperation )
        {
        delete iSubOperation;
        iSubOperation = NULL;
        }
    iSubOperation = iBaseMtm->InvokeAsyncFunctionL( aFunctionId, aSel, ptr, iStatus );
    CleanupStack::PopAndDestroy( params ); 
    }

// ----------------------------------------------------------------------------
// CIpsPlgOnlineOperation::SignalFSObserver()
// ----------------------------------------------------------------------------
//
void CIpsPlgOnlineOperation::SignalFSObserver( TInt aStatus )
    {
    FUNC_LOG;
    // <qmail> clean up this function
    // <qmail>
    if( iSignallingAllowed )
    // </qmail>    
        {
        // Initialize the progress data
        TFSProgress prog = { TFSProgress::EFSStatus_RequestComplete, 1, 1, aStatus, NULL };
        // it would be better to get fs progress from inherited class
        // by calling FSProgressL method??
        if ( aStatus == KErrCancel )
            {
            prog.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
            }
        // <qmail>
        if( iFSOperationObserver )
            {
            // do the actual signalling
            TRAP_IGNORE( iFSOperationObserver->RequestResponseL( prog, iFSRequestId ) );
            }
        // </qmail>
        }
    }

// <qmail> removed IpsOpType()

// <qmail> makes more sence to have this method here in "base" online op than in every derived class
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsPlgOnlineOperation::Connected() const
    {
    FUNC_LOG;
    TMsvEntry tentry;
    TMsvId service;
    iMsvSession.GetEntry(iService, service, tentry );
    return tentry.Connected();
    }
// </qmail>
