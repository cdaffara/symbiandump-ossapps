// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "agsbackuprestoreagent.h"
#include <connect/sbdefs.h> // For conn::EBURNormal etc.

CAgnServBackupRestoreAgent* CAgnServBackupRestoreAgent::NewL(CAgnServFileMgr& aFileMgr)
	{
	CAgnServBackupRestoreAgent* self = new(ELeave) CAgnServBackupRestoreAgent(aFileMgr);
	return self;
	}

CAgnServBackupRestoreAgent::~CAgnServBackupRestoreAgent()
	{
	Cancel();
	iBackupRestoreNotification.Close();
	}

CAgnServBackupRestoreAgent::CAgnServBackupRestoreAgent(CAgnServFileMgr& aFileMgr)
	: CActive(CActive::EPriorityStandard), iFileMgr(aFileMgr)
	{
	CActiveScheduler::Add(this);
	
	// Attach and subscribe to the Backup/Restore property used by the SBEngine
	// component.
	iBackupRestoreNotification.Attach(KUidSystemCategory, conn::KUidBackupRestoreKey);
	iBackupRestoreNotification.Subscribe(iStatus);
	}

/** Indicates if a Backup operation is currently in progress.
@return ETrue if operation in progress else EFalse.
*/
TBool CAgnServBackupRestoreAgent::BackupInProgress() const
	{
	return (iCurrentState & (conn::EBURBackupPartial | conn::EBURBackupFull));
	}

/** Indicates if a Restore operation is currently in progress.
@return	ETrue if operation in progress else EFalse.
*/
TBool CAgnServBackupRestoreAgent::RestoreInProgress() const
	{
	return (iCurrentState & (conn::EBURRestorePartial | conn::EBURRestoreFull));
	}

/** Instructs the agent to listen for backup or restore events. */
void CAgnServBackupRestoreAgent::Start()
	{
	// Check to see if a Backup/Restore is currently in progress.
	TInt newState = 0;
	if (iBackupRestoreNotification.Get(newState) != KErrNotFound)
		{
		iCurrentState = newState;
		}
	SetActive();
	}

/**
Called when a backup or restore event is received.

When a backup or restore event is received we change the state so that BackupInProgress() and
RestoreInProgress() return the correct state. These functions will be queried when a file is being closed
to check if they should be closed immediately or after a small delay.

In addition to this we also call CloseScheduledFilesNow() on the file manager to close all files that have
been scheduled for close so they are closed immediately.
*/
void CAgnServBackupRestoreAgent::RunL()
	{
	// Resubscribe before dealing with the current notification.
	iBackupRestoreNotification.Subscribe(iStatus);
	SetActive();

	TInt newState = 0;
	
	// Flag updated.
	if (iBackupRestoreNotification.Get(newState) != KErrNotFound)
		{
		if ( newState& (conn::EBURBackupPartial | conn::EBURBackupFull) && !BackupInProgress())
            {
            iFileMgr.CloseScheduledFilesImmediately();
            iFileMgr.BackupReStoreChanged(MCalChangeCallBack2::EBackupStart);
            }
        else if (newState & (conn::EBURRestoreFull | conn::EBURRestorePartial)&& !RestoreInProgress())
            {
            iFileMgr.CloseScheduledFilesImmediately();
            iFileMgr.BackupReStoreChanged(MCalChangeCallBack2::ERestoreStart);      
            }
        else if (newState & (conn::EBURNormal | conn::EBURUnset) && BackupInProgress())
            {
            iFileMgr.BackupReStoreChanged(MCalChangeCallBack2::EBackupEnd); 
            }
        else if (newState & (conn::EBURNormal | conn::EBURUnset) && RestoreInProgress())
            {
            iFileMgr.BackupReStoreChanged(MCalChangeCallBack2::ERestoreEnd); 
            }
		iCurrentState = newState;
		}
	}

/** Called when an error occurs. If an error occurs we will not be notified of backup or restore 
events. This may cause problems during backup and restore but terminating the server is excessive as
it can continue normal operation. */
TInt CAgnServBackupRestoreAgent::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CAgnServBackupRestoreAgent::DoCancel()
	{
	iBackupRestoreNotification.Cancel();
	}
