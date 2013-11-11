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
#include <memspyui.rsg>

#include "MemSpyAsyncTracker.h"

CMemSpyAsyncTracker::CMemSpyAsyncTracker(RMemSpySession& aSession, void (RMemSpySession::*function)(TRequestStatus&)) :
	CActive(EPriorityIdle), 
	iFunction(function),
	iSession(aSession)
    {
	CActiveScheduler::Add(this);
    }

CMemSpyAsyncTracker::~CMemSpyAsyncTracker()
    {
    delete iWaitNote;
    }

void CMemSpyAsyncTracker::RunL()
    { 
    // If an error occurred handle it in RunError().
    User::LeaveIfError(iStatus.Int());
 
    // Operation has finished successfully 
    iWaitNote->DialogDismissedL(0);        
    
    delete this;
    }

void CMemSpyAsyncTracker::DoCancel()
    {
	// nothing to do here
    }

TInt CMemSpyAsyncTracker::RunError(TInt aError)
	{ 	
	return KErrNone;
	}

void CMemSpyAsyncTracker::StartL()
	{
	(iSession.*iFunction)(iStatus);
	
	SetActive();
	
	iWaitNote = new (ELeave) CMemSpyWaitNote;
		
	iWaitNote->StartWaitNoteL();
	}











CMemSpyWaitNote::~CMemSpyWaitNote()
	{
	delete iWaitDialog;
	}

void CMemSpyWaitNote::StartWaitNoteL()
	{
	iWaitDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**, &iWaitDialog)), ETrue);
	iWaitDialog->PrepareLC( R_MEMSPY_WAIT_NOTE );
	iWaitDialog->SetCallback(this);
	iWaitDialog->RunLD();
	}

void CMemSpyWaitNote::DialogDismissedL(TInt aButtonId)
	{		
	iWaitDialog->ProcessFinishedL();
	}
