/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#include "MemSpyDeviceWideOperationDialog.h"

// System includes
#include <eikprogi.h>
#include <AknWaitDialog.h>
#include <AknQueryDialog.h>
#include <coemain.h>
#include <eikenv.h>
#include <avkon.hrh>
#include <memspyui.rsg>

#include <memspysession.h>

/*
CMemSpyDeviceWideOperationDialog::CMemSpyDeviceWideOperationDialog( CMemSpyEngine& aEngine, MMemSpyDeviceWideOperationDialogObserver& aObserver )
:   iEngine( aEngine ), iObserver( aObserver )
    {
    }

CMemSpyDeviceWideOperationDialog::CMemSpyDeviceWideOperationDialog( RMemSpySession& aSession, MMemSpyDeviceWideOperationDialogObserver& aObserver )
:   iSession( aSession ), iObserver( aObserver )
    {
    }    
*/

CMemSpyDeviceWideOperationDialog::CMemSpyDeviceWideOperationDialog( RMemSpySession& aSession )
:   iSession( aSession )
    {
    }    


CMemSpyDeviceWideOperationDialog::~CMemSpyDeviceWideOperationDialog()
    {
#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::~CMemSpyDeviceWideOperationDialog() - START - iForcedCancel: %d", iForcedCancel );
#endif

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::~CMemSpyDeviceWideOperationDialog() - deleting operation...: 0x%08x", iOperation );
#endif
    delete iOperation;

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::~CMemSpyDeviceWideOperationDialog() - deleting dialog...: 0x%08x", iProgressDialog );
#endif
    iProgressInfo = NULL;
    delete iProgressDialog;

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::~CMemSpyDeviceWideOperationDialog() - sending EDialogDismissed to observer..." );
#endif

    //iObserver.DWOperationCompleted(); //TODO

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::~CMemSpyDeviceWideOperationDialog() - END" );
#endif
    }


//void CMemSpyDeviceWideOperationDialog::ExecuteL( CMemSpyDeviceWideOperations::TOperation aOperation )
void CMemSpyDeviceWideOperationDialog::ExecuteL( TDeviceWideOperation aOp )
    {
	/*
#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::ExecuteL() - START" );
#endif

    iOperation = CMemSpyDeviceWideOperations::NewL( iEngine, *this, aOperation );

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::ExecuteL() - constructed operation..." );
#endif

    ASSERT( iProgressDialog == NULL );
    iProgressDialog = new( ELeave ) CAknProgressDialog( reinterpret_cast< CEikDialog** >( &iProgressDialog ), ETrue );
    iProgressDialog->PrepareLC( R_MEMSPY_DEVICE_WIDE_OPERATION_PROGRESS_DIALOG );
    iProgressDialog->SetCallback( this );
    iProgressDialog->SetGloballyCapturing( ETrue );
    //
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressInfo->SetFinalValue( iOperation->TotalOperationSize() );
    //
    iProgressDialog->RunLD();

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::ExecuteL() - END" );
#endif
	*/
    }

void CMemSpyDeviceWideOperationDialog::ExecuteLD( RMemSpySession& aSession, TDeviceWideOperation aOp )
    {
    //CMemSpyDeviceWideOperationDialog* self = new(ELeave) CMemSpyDeviceWideOperationDialog( aSession, aObserver );
	CMemSpyDeviceWideOperationDialog* self = new(ELeave) CMemSpyDeviceWideOperationDialog( aSession );
    CleanupStack::PushL( self );
    self->ExecuteL( aOp );
    CleanupStack::PopAndDestroy( self );
    }


void CMemSpyDeviceWideOperationDialog::Cancel()
    {
#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::Cancel() - START - iOperation: 0x%08x, iForcedCancel: %d", iOperation, iForcedCancel );
#endif

    iForcedCancel = ETrue;
    //
    if  ( iOperation )
        {
        iOperation->Cancel();
        }
    //
    //iObserver.DWOperationCancelled();

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::Cancel() - END" );
#endif
    }


void CMemSpyDeviceWideOperationDialog::DialogDismissedL( TInt aButtonId )
    {
#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::DialogDismissedL() - START - aButtonId: %d, iForcedCancel: %d", aButtonId, iForcedCancel );
#else
    (void) aButtonId;
#endif

    Cancel();

#ifdef _DEBUG
    RDebug::Printf("[MemSpy] CMemSpyDeviceWideOperationDialog::DialogDismissedL() - END - aButtonId: %d, iForcedCancel: %d", aButtonId, iForcedCancel );
#endif
    }


void CMemSpyDeviceWideOperationDialog::HandleDeviceWideOperationEvent( TEvent aEvent, TInt aParam1, const TDesC& aParam2 )
    {
#ifdef _DEBUG
    RDebug::Print( _L("[MemSpy] CMemSpyDeviceWideOperationDialog::HandleDeviceWideOperationEvent() - START - aEvent: %d, iProgressDialog: 0x%08x, aParam1: %d, aParam2: %S"), aEvent, iProgressDialog, aParam1, &aParam2 );
#endif

    switch( aEvent )
        {
    case MMemSpyDeviceWideOperationsObserver::EOperationSized:
        break;
    case MMemSpyDeviceWideOperationsObserver::EOperationStarting:
        //iObserver.DWOperationStarted();
        break;
    case MMemSpyDeviceWideOperationsObserver::EOperationProgressStart:
        ASSERT( iProgressDialog != NULL );
        SetDialogCaptionL( aParam2 );
        break;
    case MMemSpyDeviceWideOperationsObserver::EOperationProgressEnd:
        ASSERT( iProgressDialog != NULL );
        iProgressInfo->IncrementAndDraw( aParam1 );
        break;
    case MMemSpyDeviceWideOperationsObserver::EOperationCancelled:
        break;
    case MMemSpyDeviceWideOperationsObserver::EOperationCompleting:
        {
        ASSERT( iProgressDialog != NULL );
        const TInt finalValue = iProgressInfo->Info().iFinalValue;
        iProgressInfo->SetAndDraw( finalValue );
        break;
        }
    case MMemSpyDeviceWideOperationsObserver::EOperationCompleted:
        if  ( iProgressDialog )
            {
            iProgressDialog->ProcessFinishedL();
            }
        break;
    default:
        break;
        }

#ifdef _DEBUG
    RDebug::Print( _L("[MemSpy] CMemSpyDeviceWideOperationDialog::HandleDeviceWideOperationEvent() - END - aEvent: %d, aParam1: %d, aParam2: %S"), aEvent, aParam1, &aParam2 );
#endif
    }


void CMemSpyDeviceWideOperationDialog::SetDialogCaptionL( const TDesC& aText )
    {
    if  ( aText.Length() )
        {
        iProgressDialog->SetTextL( aText );
        iProgressDialog->DrawNow();
        }
    }

CMemSpyDwoTracker* CMemSpyDeviceWideOperationDialog::CreateDeviceWideOperation( RMemSpySession& aSession, TDeviceWideOperation aOp )
{
	return new CMemSpyDwoTracker( aSession, aOp );
}













CMemSpyDwoProgressTracker::CMemSpyDwoProgressTracker(RMemSpySession &aSession) 
	: CActive( EPriorityStandard ), iSession( aSession )
	{
	CActiveScheduler::Add(this);
	}

CMemSpyDwoProgressTracker::~CMemSpyDwoProgressTracker()
	{
	Cancel();	
	}

void CMemSpyDwoProgressTracker::Start()
	{
	ASSERT( iProgressDialog == NULL );
	iProgressDialog = new( ELeave ) CAknProgressDialog( reinterpret_cast< CEikDialog** >( &iProgressDialog ), ETrue );
	iProgressDialog->PrepareLC( R_MEMSPY_DEVICE_WIDE_OPERATION_PROGRESS_DIALOG );
	iProgressDialog->SetCallback( this );
	iProgressDialog->SetGloballyCapturing( ETrue );
	iProgressInfo = iProgressDialog->GetProgressInfoL();	
	iProgressInfo->SetFinalValue( 100 );	
	
	iSession.NotifyDeviceWideOperationProgress( iProgress, iStatus );
	
	UpdateProcessDialogL( iProgress.Progress(), iProgress.Description() );
	
	SetActive();
	
	iProgressDialog->RunLD();
	}

void CMemSpyDwoProgressTracker::Cancel()
	{	
	}

void CMemSpyDwoProgressTracker::RunL()
    { 		
    // If an error occurred handle it in RunError().
    User::LeaveIfError(iStatus.Int());
 
    // Resubmit the request immediately    
    iSession.NotifyDeviceWideOperationProgress( iProgress, iStatus );     
    
    SetActive();            
    
    UpdateProcessDialogL( iProgress.Progress(), iProgress.Description() );    
    }
 
void CMemSpyDwoProgressTracker::DoCancel()
	{ 	
	}
 
TInt CMemSpyDwoProgressTracker::RunError(TInt aError)
	{ 
	// KErrNotReady and KErrCancel errors are OK, they just notify 
	// us about the outstanding notification request that won't be 
	// processed.		
    return KErrNone;
	}

void CMemSpyDwoProgressTracker::UpdateProcessDialogL( TInt aProgress, const TDesC& aProgressText )
	{
	if(iProgressDialog)
		{
		iProgressDialog->SetTextL( aProgressText );
	    }
	 
	if(iProgressInfo)
		{
	    iProgressInfo->SetAndDraw( aProgress );	
		}
	}

void CMemSpyDwoProgressTracker::DialogDismissedL(TInt aButtonId)
	{
	iProgressDialog = NULL;
	iProgressInfo = NULL;	
	 
	Cancel();
	}














CMemSpyDwoTracker::CMemSpyDwoTracker( RMemSpySession &aSession, TDeviceWideOperation aOperation )
:	CActive( EPriorityStandard ), 
	iSession( aSession ),
	iProgressTracker(new CMemSpyDwoProgressTracker( aSession )),
	iOperation( aOperation )
	{
	CActiveScheduler::Add(this);
	}

CMemSpyDwoTracker::~CMemSpyDwoTracker()
	{		
	Cancel();
	
	delete iProgressTracker;
	}

void CMemSpyDwoTracker::Start()
{	
	void (RMemSpySession::*functions[])(TRequestStatus&) = { 
		&RMemSpySession::OutputPhoneInfo,
		&RMemSpySession::OutputDetailedPhoneInfo,
		&RMemSpySession::OutputHeapInfo,
		&RMemSpySession::OutputCompactHeapInfo,
		&RMemSpySession::OutputHeapCellListing,
		&RMemSpySession::OutputHeapData,
		&RMemSpySession::OutputStackInfo,
		&RMemSpySession::OutputCompactStackInfo,
		&RMemSpySession::OutputUserStackData,
		&RMemSpySession::OutputKernelStackData };
		
	(iSession.*functions[iOperation])(iStatus);			
	
	SetActive();
	
	iProgressTracker->Start();				
}

void CMemSpyDwoTracker::Cancel()
{
}
 
void CMemSpyDwoTracker::RunL()
    { 
    // If an error occurred handle it in RunError().
    User::LeaveIfError(iStatus.Int());
 
    if( !IsActive() )
    	{
		iProgressTracker->ProgressDialog()->ProcessFinishedL();
    	}
    }
 
void CMemSpyDwoTracker::DoCancel()
{ 
	// Cancel progress tracker
	iProgressTracker->Cancel();
	
	iSession.CancelDeviceWideOperationL();	
}
 
TInt CMemSpyDwoTracker::RunError(TInt aError)
{ 
	// Emit the finished(false) signal to notify user 
	// operation was canceled
	//emit finished(false); //TODO:
	
    return KErrNone;
}
