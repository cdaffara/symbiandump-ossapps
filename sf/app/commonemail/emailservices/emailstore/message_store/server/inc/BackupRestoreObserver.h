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
* Description:  Backup/restore observer.
*
*/



#ifndef __BACKUP_RESTORE_OBSERVER_H__
#define __BACKUP_RESTORE_OBSERVER_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <e32property.h>
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

// =======
// CLASSES
// =======

// =============================
// CLASS: MBackupRestoreObserver
// =============================
class MBackupRestoreObserver
	{
	public:

        // ==============
        // PUBLIC METHODS
        // ==============
        	
		virtual void BackupOrRestoreInProgress( TBool aIsARestore ) = 0;
		
		virtual void BackupOrRestoreCompleted() = 0;
	
	}; // end class MBackupRestoreObserver
	
// =============================
// CLASS: CBackupRestoreNotifier
// =============================
class CBackupRestoreNotifier : private CActive
	{
	public:
	
        // ==============
        // PUBLIC METHODS
        // ==============
        	
	    static CBackupRestoreNotifier* NewL( MBackupRestoreObserver& aObserver );
	
		virtual ~CBackupRestoreNotifier();	
	
    	void ProcessEvent( TUint aEvent );            
		            
	private:
	
        // ===============
        // PRIVATE METHODS
        // ===============
        	
		CBackupRestoreNotifier( MBackupRestoreObserver& aObserver );
		
		void ConstructL();

		// inherited from CActive
		virtual void DoCancel();
		virtual void RunL();
		
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
        	
		MBackupRestoreObserver& iObserver;
		RProperty               iProperty;
		TBool                   iInProgress;
		
		__LOG_DECLARATION
			
	}; // end class CBackupRestoreNotifier
	
#endif
