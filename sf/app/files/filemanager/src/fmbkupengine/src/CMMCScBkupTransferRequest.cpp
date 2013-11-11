/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CMMCScBkupTransferRequestBase implementation
*
*
*/

#include "CMMCScBkupTransferRequest.h"

// User includes
#include "MMCScBkupLogger.h"
#include "MMMCScBkupDriver.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMMCScBkupArchiveDataInterface.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::CMMCScBkupTransferRequestBase()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupTransferRequestBase::CMMCScBkupTransferRequestBase( MMMCScBkupDriver& aDriver, TMMCScBkupOwnerDataType aElementType, TBool aUpdateOperationalSizes, TInt aPriority )
:   CActive( aPriority ), iDriver( aDriver ), iElementType( aElementType ), iUpdateOperationalSizes( aUpdateOperationalSizes )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::~CMMCScBkupTransferRequestBase()
// 
// C++ destructor.
// ---------------------------------------------------------------------------
CMMCScBkupTransferRequestBase::~CMMCScBkupTransferRequestBase()
    {
    Cancel();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::RequestL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver )
    {
    __ASSERT_ALWAYS( !IsActive(), User::Invariant() );
    //
    iDataOwner = &aOwner;
    SetObserver( aObserver );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::SingleDriveTransferCompleteL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::SingleDriveTransferCompleteL()
    {
    if  ( iUpdateOperationalSizes )
        {
        const TDriveNumber drive = CurrentDrive();
        const TInt amount = AmountOfDataTransferred();
        __LOG3("CMMCScBkupTransferRequestBase::SingleDriveTransferCompleteL() - drive: %c:, amount: %8d, elementType: %S", drive + 'A', amount, &MMCScBkupLogger::DataType( ElementType() ) );
        //
        ASSERT( drive >= EDriveA && drive <= EDriveZ );
        }

    ResetDataTransferCounter();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::TransferCompleteL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::TransferCompleteL()
    {
    DataOwner().SetCompletionStatus( iElementType, ETrue );
    }
    

// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::CompleteSelf()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::CompleteSelf( TInt aCompletionCode )
    {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aCompletionCode);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::SetObserver()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::SetObserver( TRequestStatus& aObserver )
    {
    __ASSERT_DEBUG( iObserver == NULL, User::Invariant() );
    iObserver = &aObserver;
    aObserver = KRequestPending;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::CompleteObserverRequest()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::CompleteObserverRequest( TInt aCompletionCode )
    {
    __LOG1("CMMCScBkupTransferRequestBase::CompleteObserverRequest() - aCompletionCode: %d", aCompletionCode);
    __ASSERT_ALWAYS(iObserver != NULL, User::Invariant());
    User::RequestComplete(iObserver, aCompletionCode);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::RunL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::RunL()
    {
    __LOG2("CMMCScBkupTransferRequestBase::RunL() - START - iStatus: %d, iState: %d", iStatus.Int(), iState);

    User::LeaveIfError( iStatus.Int() );
    //
    switch( State() )
        {
    case ETransferData:
        __LOG("CMMCScBkupTransferRequestBase::RunL() - ETransferData");
        PrepareDataTransferL();
        break;

    case EProcessData:
        __LOG("CMMCScBkupTransferRequestBase::RunL() - EProcessData");
        ProcessDataChunkL();
        break;

    case EFinished:
        // Inform any interested derived classes
        __LOG("CMMCScBkupTransferRequestBase::RunL() - EFinished - transfer complete....");
        TransferCompleteL();

        // Notify observer
        __LOG("CMMCScBkupTransferRequestBase::RunL() - EFinished - complete observer...");
        CompleteObserverRequest( KErrNone );
        break;

    default:
    case EIdle:
        __LOG("CMMCScBkupTransferRequestBase::RunL() - EIdle - User::Invariant()");
        ASSERT( EFalse );
        break;
        }

    __LOG2("CMMCScBkupTransferRequestBase::RunL() - END - iStatus: %d, iState: %d", iStatus.Int(), iState);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::DoCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::DoCancel()
    {
    CompleteObserverRequest( KErrCancel );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::RunError()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupTransferRequestBase::RunError( TInt aError )
    {
    if  ( aError != KErrNone )
        {
        __LOGFILE1("CMMCScBkupTransferRequestBase::RunError() - **** - ERROR (%d) -> complete observer with error code", aError);
        }
    //
    CompleteObserverRequest( aError );
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupTransferRequestBase::ProcessDataChunkL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupTransferRequestBase::ProcessDataChunkL()
    {
    __LOG("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - START");

    // Now call virtual function to do the processing
    const TBool requiresMoreChunkProcessing = ProcessChunkOfDataL();
    __LOG1("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - requiresMoreChunkProcessing: %d", requiresMoreChunkProcessing);
    //
    TState nextState = EProcessData;
    if  ( !requiresMoreChunkProcessing )
        {
        __LOG("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - did not require more processing... ");
        SingleDriveTransferCompleteL();
        //
        nextState = ETransferData;
        __LOG("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - nextState is: ETransferData");
        }
    else
        {
        __LOG("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - nextState is: EProcessData");
        }

    // Must check IsActive(), since CMMCScBkupReadDataTransferRequestBase::ProcessChunkOfDataL()
    // changes state and set's active within the ProcessChunkOfDataL callback
    if  ( !IsActive() )
        {
        __LOG("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - not active - completing self and changing state!");
        SetState( nextState );
        CompleteSelf();
        }
    else
        {
        __LOG1("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - already active - state is: %d", State());
        }

    __LOG("CMMCScBkupTransferRequestBase::ProcessDataChunkL() - END");
    }



















