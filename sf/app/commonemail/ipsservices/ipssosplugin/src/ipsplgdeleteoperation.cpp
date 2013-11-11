/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
//<qmail>
* Description: This file implements class CIpsPlgDeleteOperation.
//</qmail>
*
*/



// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

//<qmail> comment removed

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::CIpsPlgDeleteOperation
// ----------------------------------------------------------------------------
//
//<qmail>
CIpsPlgDeleteOperation::CIpsPlgDeleteOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus )
    :
    CMsvOperation( 
        aMsvSession, 
        CActive::EPriorityStandard, 
        aObserverRequestStatus),
    iOperationObserver(NULL),
    iFSRequestId(KErrNotFound),
    iState( ESetFlags ) // <qmail>
    //</qmail> iBlank removed    
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    }
//</qmail>

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::CIpsPlgDeleteOperation
// ----------------------------------------------------------------------------
//
//<qmail>
CIpsPlgDeleteOperation::CIpsPlgDeleteOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    :
    CMsvOperation( 
        aMsvSession, 
        CActive::EPriorityStandard, 
        aObserverRequestStatus),
    iOperationObserver(&aOperationObserver),
    iFSRequestId(aRequestId),
    iState( ESetFlags )
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    }
//</qmail>


// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::ConstructL
// ----------------------------------------------------------------------------
//
// <qmail>
void CIpsPlgDeleteOperation::ConstructL(
    CMsvEntrySelection* aEntriesToDelete ) //<qmail> param name changed
    {
    FUNC_LOG;
    //<qmail>
    iEntrySelection = new (ELeave) CMsvEntrySelection();
    
    for ( TInt i=0; i<aEntriesToDelete->Count(); i++ )
        {
        iEntrySelection->AppendL( aEntriesToDelete->At(i) );
        }
    
    if ( !iEntrySelection->Count() )
        {
        User::Leave( KErrNotSupported );
        }
    //</qmail>

    //<qmail>
    // Messages will be deleted from one specific folder at a time
    TMsvId serviceId;
    TMsvEntry entry;
    User::LeaveIfError(
        iMsvSession.GetEntry(
            (*iEntrySelection)[0], serviceId, entry ) );
    iEntry = CMsvEntry::NewL(
        iMsvSession, entry.Parent(), TMsvSelectionOrdering() );
    
    // For CMsvOperation
    iMtm = iEntry->Entry().iMtm;
    //</qmail>
    iStatus = KRequestPending;

    // It is known that there is at least one entry -> no return value check
    SetNextLocallyDeletedFlagL();

    SetActive();
    }
// </qmail>

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgDeleteOperation* CIpsPlgDeleteOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CMsvEntrySelection* aEntriesToDelete ) //<qmail> param name changed
    {
    FUNC_LOG;
    CIpsPlgDeleteOperation* self=new (ELeave) CIpsPlgDeleteOperation(
        aMsvSession, aObserverRequestStatus );
    CleanupStack::PushL(self);
    //<qmail>
    self->ConstructL( aEntriesToDelete );
    //</qmail>
    CleanupStack::Pop( self ); 
    return self;
    }

// <qmail>
// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgDeleteOperation* CIpsPlgDeleteOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CMsvEntrySelection* aEntriesToDelete,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgDeleteOperation* self=new (ELeave) CIpsPlgDeleteOperation(
        aMsvSession, aObserverRequestStatus, aOperationObserver, aRequestId );
    CleanupStack::PushL(self);
    self->ConstructL( aEntriesToDelete );
    CleanupStack::Pop( self ); 
    return self;
    }
// </qmail>

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::~CIpsPlgDeleteOperation
// ----------------------------------------------------------------------------
//
CIpsPlgDeleteOperation::~CIpsPlgDeleteOperation()
    {
    FUNC_LOG;
    Cancel();

    delete iOperation;
    delete iEntry;
    delete iEntrySelection;
	// <qmail>    
    delete iSetFlagEntry;
	// </qmail>    
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::DoCancel
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteOperation::DoCancel()
    {
    FUNC_LOG;
    if (iOperation)
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    if ( status && status->Int() == KRequestPending )
        {
        User::RequestComplete(status, iStatus.Int());
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteOperation::RunL()
    {
    FUNC_LOG;
    // Make first a check, that nothing has failed
    if ( iStatus.Int() != KErrNone )
        {
        // something failed, just complete
        SignalFSObserver(iStatus.Int());
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        return;
        }

	// <qmail>    
    if ( iState == ESetFlags )
    //</qmail>    
        {
        // cleanup is handled by SetNextLocallyDeletedFlagL
        TBool ret = EFalse;
        
        TRAPD( err, ret = SetNextLocallyDeletedFlagL() );
        
        if ( err != KErrNone )
            {
            SignalFSObserver(err);
            TRequestStatus* status = &iObserverRequestStatus;
            User::RequestComplete(status, iStatus.Int());
            }
        else if ( ret )
            {
            SetActive();
            }
        else
            { // setting the flags is ready, start local deletion
            StartNextDeleteLocally();
            }
        }
    //<qmail>
    else if ( iState == ELocally )
    //</qmail>
        {
        // local delete completed, start deleting from server
        StartDeleteFromServer();
        }
    else
        {
        // Inform observer
        SignalFSObserver(iStatus.Int());
        // nothing left to process, so complete the observer
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        }
    // </qmail>    
	}

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::ProgressL
// ----------------------------------------------------------------------------
//
//<qmail>
const TDesC8& CIpsPlgDeleteOperation::ProgressL()
    {
    FUNC_LOG;
    // Make sure that operation is active
    if ( IsActive() )
        {
        // Get progress
        if ( iOperation )
            {
            return iOperation->ProgressL();
            }
        }
    //<qmail>
    return KNullDesC8;
    //</qmail>
    }
//</qmail>
// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::StartNextDeleteLocally
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteOperation::StartNextDeleteLocally()
    {
    //<qmail>
    FUNC_LOG;
    iState = ELocally;
    //</qmail>
    iStatus = KRequestPending;
    
    TRAPD( err, MakeNextDeleteLocallyL() );
    SetActive();
    // if error then complete this pass with the error code
    if ( err )
        {
        SignalFSObserver(err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::MakeNextDeleteLocallyL
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteOperation::MakeNextDeleteLocallyL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    iOperation = CIpsPlgDeleteLocal::NewL(
        iEntrySelection, iMsvSession, iStatus );
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::StartDeleteFromServer
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteOperation::StartDeleteFromServer()
    {
    FUNC_LOG;
    //<qmail>
    iState = EFromServer;
    //</qmail>
    iStatus = KRequestPending;
    TRAPD( err, MakeDeleteFromServerL() );

    SetActive();
    // if error then complete this pass with the error code
    if ( err )
        {
        SignalFSObserver(err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::MakeDeleteFromServerL
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteOperation::MakeDeleteFromServerL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    iOperation = iEntry->DeleteL( *iEntrySelection, iStatus );
    }


// <qmail>    
// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::SetNextLocallyDeletedFlagL
// ----------------------------------------------------------------------------
//
TBool CIpsPlgDeleteOperation::SetNextLocallyDeletedFlagL()
    {
    FUNC_LOG;
    
    delete iOperation;
    iOperation = NULL;
    
    delete iSetFlagEntry;
    iSetFlagEntry = NULL;

    TBool ret = EFalse;
    //<qmail>
    if ( iSetFlagIndex < iEntrySelection->Count() )
    //</qmail>    
        {
        TMsvId entryId = ( *iEntrySelection )[ iSetFlagIndex++ ];
        
        iSetFlagEntry = CMsvEntry::NewL( 
            iMsvSession, entryId, TMsvSelectionOrdering() );
        
        TMsvEntry tEntry = iSetFlagEntry->Entry();

        // Sets bit 32 of iMtmData1, used when msg deleted in Offline
        // and status hasn't updated to server (client entry still exists)
        tEntry.SetLocallyDeleted( ETrue );

        iOperation = iSetFlagEntry->ChangeL( tEntry, iStatus );
        
        ret = ETrue;
        }
    
    return ret;
    }
// </qmail>

//<qmail> 
// ----------------------------------------------------------------------------
// CIpsPlgDeleteOperation::SignalFSObserver
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteOperation::SignalFSObserver(TInt aStatus)
    {
    if (iOperationObserver)
        {
        iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        iFSProgress.iError = aStatus;
        iFSProgress.iParam = NULL;
    
        TRAP_IGNORE( iOperationObserver->RequestResponseL( iFSProgress, iFSRequestId ) );
        }
    }
// </qmail>

//  End of File


