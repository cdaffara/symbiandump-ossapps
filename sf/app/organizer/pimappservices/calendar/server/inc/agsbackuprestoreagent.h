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

#ifndef __AGSBACKUPRESTOREAGENT_H__
#define __AGSBACKUPRESTOREAGENT_H__

#include <e32base.h>
#include <e32property.h>
#include "agsfilemanager.h"

/**
This class listens for backup and restore events.

The server needs to be informed about these events because during normal operation 
calendar files are not closed immediately but after a small delay. When a backup or
restore operation is taking place we want files to be close immediately. This is 
to allow the backup and restore engine to access these files.

@internalComponent
*/
class CAgnServBackupRestoreAgent : public CActive
	{
public:
 	static CAgnServBackupRestoreAgent* NewL(CAgnServFileMgr& aFileMgr);
	~CAgnServBackupRestoreAgent();
	
	TBool BackupInProgress() const;
	TBool RestoreInProgress() const;
	void Start();
	
private:
	CAgnServBackupRestoreAgent(CAgnServFileMgr& aFileMgr);
	
private:	// from CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
private:
	/**
	Used to subscribe to the SBEngine P&S property KUidBackupRestoreKey.  The
	SBEngine uses the property KUidBackupRestoreKey to notify subscribers of
	Backup/Restore events.
	*/
	RProperty iBackupRestoreNotification;
	TInt iCurrentState;
	CAgnServFileMgr& iFileMgr;
	};

#endif
