/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Installer Queue implementation
*
*/


// INCLUDE FILES
#include "sconinstqueue.h"
#include "sconbrqueue.h"
#include "sconinstaller.h"
#include "sconpcdconsts.h"
#include "debug.h"

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCSConInstallerQueue* CSConInstallerQueue::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConInstallerQueue* CSConInstallerQueue::NewL( RFs& aFs )
	{
	TRACE_FUNC_ENTRY;
	CSConInstallerQueue* self = new (ELeave) CSConInstallerQueue( aFs );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	TRACE_FUNC_EXIT;
    return self;
	}
	
// -----------------------------------------------------------------------------
// CSConInstallerQueue::CSConInstallerQueue()
// Constructor
// -----------------------------------------------------------------------------
//
CSConInstallerQueue::CSConInstallerQueue( RFs& aFs ) : CActive( EPriorityStandard ), iFs( aFs )
	{
    CActiveScheduler::Add( this );
	}
	
// -----------------------------------------------------------------------------
// CSConInstallerQueue::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::ConstructL()
	{
	TRACE_FUNC_ENTRY;
	iInstaller = CSConAppInstaller::NewL( this, iFs );
	User::LeaveIfError( iTimer.CreateLocal() );
	TRACE_FUNC_EXIT;
	}
	
// -----------------------------------------------------------------------------
// CSConInstallerQueue::~CSConInstallerQueue()
// Destructor
// -----------------------------------------------------------------------------
//
CSConInstallerQueue::~CSConInstallerQueue()
	{
	TRACE_FUNC_ENTRY;
	Cancel();
	if( iInstaller )
		{
		iInstaller->Cancel();
		delete iInstaller;
		iInstaller = NULL;
		}
	TRACE_FUNC_EXIT;
	}
	
// -----------------------------------------------------------------------------
// CSConInstallerQueue::StartQueue()
// Starts queue polling
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::StartQueue()
	{
	TRACE_FUNC;
	if( IsActive() )
		{
		Cancel();
		}
		
	iTimer.After( iStatus, KSConTimerValue );
	SetActive();
	}
	
// -----------------------------------------------------------------------------
// CSConInstallerQueue::StartQueue()
// Stops queue polling
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::StopQueue()	
	{
	TRACE_FUNC_ENTRY;
	iTimer.Cancel();
	TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// CSConInstallerQueue::CancelTask( TInt aTask, TBool aAllTasks )
// Cancels a task
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::CancelTask( TInt aTask, TBool aAllTasks )
	{
	TRACE_FUNC_ENTRY;
	//Stop the installer
	if( aTask && !aAllTasks )
		{
		iInstaller->StopInstaller( aTask );
		}
	
	if( aAllTasks )
		{
		iInstaller->Cancel();
		}
	CSConTaskQueue::CancelTask( aTask, aAllTasks );
	TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// CSConInstallerQueue::QueueAddress( CSConBackupRestoreQueue*& aTaskQueue )
// An address pointer to another queue
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::QueueAddress( CSConBackupRestoreQueue*& aTaskQueue )
	{
	TRACE_FUNC;
	iBRQueueAddress = aTaskQueue;
	}

// -----------------------------------------------------------------------------
// CSConInstallerQueue::PollQueue()
// Polls queue
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::PollQueue()
	{
	TRACE_FUNC;
	// find and start next task if installer and BR is inactive
	if( !iInstaller->InstallerActive()
		&& !iBRQueueAddress->QueueProcessActive() )
		{
		//find next task
		for( TInt i = 0; i < iQueue.Count(); i++ )
			{
			TBool complete = iQueue[i]->GetCompleteValue();
			
			if( complete == EFalse )
				{
				iInstaller->StartInstaller( iQueue[i]->iTaskId );
				i = iQueue.Count() + 1; // jump out from loop
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSConInstallerQueue::DoCancel()
// Implementation of CActive::DoCancel()
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::DoCancel()
	{
	TRACE_FUNC;
	iTimer.Cancel();
	}

// -----------------------------------------------------------------------------
// CSConInstallerQueue::RunL()
// Implementation of CActive::RunL()
// -----------------------------------------------------------------------------
//
void CSConInstallerQueue::RunL()
	{
	TRACE_FUNC_ENTRY;
	LOGGER_WRITE_1( "There are still %d tasks in this queue", iQueue.Count() );
	if( iQueue.Count() > 0 )
		{
		PollQueue();
		StartQueue();
		}
	TRACE_FUNC_EXIT;
	}

// End of file
