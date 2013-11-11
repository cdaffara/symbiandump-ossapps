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
*       folder refresh operation
*
*/


// <qmail> aknmessagequerydialog include removed

#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> ipssossettings rsg removed
// <qmail> fsmailserver rsg removed

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter removed, aSelection moved from ConstructL -> constructor
// <qmail> MFSMailRequestObserver& changed to pointer
// <qmail> renamed selection parameter
CIpsPlgConnectAndRefreshFolderList* CIpsPlgConnectAndRefreshFolderList::NewL(
    CMsvSession& aSession, 
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService, 
    TFSMailMsgId& aMailboxId, 
    CMsvEntrySelection* aSelection, 
    MFSMailRequestObserver* aFSObserver, 
    CIpsPlgTimerOperation& aTimer )
    {
    FUNC_LOG;
    CIpsPlgConnectAndRefreshFolderList* self = new (ELeave) CIpsPlgConnectAndRefreshFolderList( 
        aSession, 
        aObserverRequestStatus, 
        aService, 
        aMailboxId, 
        aSelection,
        aFSObserver, 
        aTimer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> priority parameter removed, aSelection moved from ConstructL -> constructor
// <qmail> MFSMailRequestObserver& changed to pointer
// <qmail> renamed selection parameter
CIpsPlgConnectAndRefreshFolderList::CIpsPlgConnectAndRefreshFolderList(
    CMsvSession& aSession, 
    TRequestStatus& aObserverRequestStatus,
    TMsvId aService, 
    TFSMailMsgId& aMailboxId, 
    CMsvEntrySelection* aSelection,
    MFSMailRequestObserver* aFSObserver,
    CIpsPlgTimerOperation& aTimer )
	:
	CIpsPlgOnlineOperation(
        aSession,
        aObserverRequestStatus,
        aTimer,
        aMailboxId,
        aFSObserver,
        0 ), // requestId
	iState( EIdle ),
    iSelection( aSelection )
    {
    FUNC_LOG;
    iService = aService;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// <qmail> aMsvEntry removed
void CIpsPlgConnectAndRefreshFolderList::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( KSenduiMtmImap4Uid );
    iState = EStartConnect;
    DoRunL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgConnectAndRefreshFolderList::~CIpsPlgConnectAndRefreshFolderList()
    {
    FUNC_LOG;
    Cancel();
    delete iSelection;
    }

// <qmail> removed 3 functions from MAknBackgroundProcess

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgConnectAndRefreshFolderList::ProgressL()
    {
    FUNC_LOG;
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const TDesC8& CIpsPlgConnectAndRefreshFolderList::GetErrorProgressL( TInt aError )
    {
    FUNC_LOG;
    TImap4CompoundProgress& prog = iProgressBuf();
    prog.iGenericProgress.iErrorCode = aError;
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSProgress CIpsPlgConnectAndRefreshFolderList::GetFSProgressL() const
    {
    FUNC_LOG;
    // Hardly ever called
    return TFSProgress();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgConnectAndRefreshFolderList::DoRunL()
    {
    FUNC_LOG;
    
    if( iStatus.Int() != KErrNone )
// </qmail>
        {
        iState = ECompleted;
// <qmail> DisplayLoginFailedDialogL removed
        CompleteObserver();
        return;
        }
    
    switch(iState)
        {
        case EStartConnect:
            delete iSubOperation;
            iSubOperation = NULL;
            // <qmail> priority parameter has been removed
            iSubOperation = CIpsPlgImap4ConnectOp::NewL(
                iMsvSession,
                iStatus,
                iService,
                *iActivityTimer,
                iFSMailboxId,
                NULL, // no observer for suboperations
                0, // no requestId needed
                NULL, // Event handler
                ETrue ); // Plain connect
            iState = EConnecting;
            SetActive();
            break;
	    case EConnecting:
	        {
	        //  We have successfully completed connecting
	        TBuf8<1> dummyParam;
	        delete iSubOperation;
	        iSubOperation = NULL;
	        InvokeClientMtmAsyncFunctionL( KIMAP4MTMSyncTree, *iSelection, dummyParam ); // <qmail> 1 param removed 
	        iState = ERefreshing;
	        SetActive();
	        }
	        break;
	    case ERefreshing:
	        //  We have successfully completed refreshing the folder list
	        delete iSubOperation;
	        iSubOperation = NULL;
	        iSubOperation = CIpsPlgDisconnectOp::NewL( 
                iMsvSession, 
                iStatus, 
                iService, 
                *iActivityTimer,
                iFSMailboxId, 
                NULL, // no observer for suboperations
                0 ); // no requestId needed
	        iState = EDisconnecting;
	        SetActive();
	        break;
	    case EDisconnecting:
	        iState = ECompleted;
	        CompleteObserver();
	        break;
	    default:
	        User::Panic( KIpsPlgPanicCategory, EIpsPlgNoParameters );
	    	break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsPlgConnectAndRefreshFolderList::DoCancel()
    {
    FUNC_LOG;
    if( iSubOperation )
        {
        iSubOperation->Cancel();
        }
    CompleteObserver( KErrCancel );
    iState = ECompleted;
    }


// <qmail> removed in Qmail
//void CIpsPlgConnectAndRefreshFolderList::DisplayLoginFailedDialogL()
// </qmail>

// <qmail> new func to this op
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgConnectAndRefreshFolderList::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeConnectAndRefreshFolderListOp;
    }
