/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConBackupRestore header file
*
*/


#ifndef _SCONBACKUPRESTORE_H_
#define _SCONBACKUPRESTORE_H_

// INCLUDES

#include <e32base.h>
#include <e32cons.h>

class CSConSBEClient;
class CSConBackupRestoreQueue;
class RFs;

//============================================================
// Class CSConBackupRestore declaration
//============================================================	
NONSHARABLE_CLASS( CSConBackupRestore ): public CActive
	{
	public:
		/**
		 * Two-phase constructor
		 * @param aQueue The address of CSConBackupRestoreQueue
		 * @param aFs aReference to RFs connection.
    	 * @return CSConBackupRestore instance
		 */
		static CSConBackupRestore* NewL( 
			CSConBackupRestoreQueue* aQueue, 
			RFs& aFs );
		/**
		 * Destructor
		 * @return none
		 */
		~CSConBackupRestore();
		
		/**
		 * Starts backup-restore task
		 * @param aTaskId The task number
    	 * @return none
		 */
		void StartBackupRestore( TInt aTaskId );
		/**
		 * Stops executing backup-restore task
		 * @param aTaskId
    	 * @return none
		 */
		void StopBackupRestore( TInt aTaskId );
		/**
		 * Resets backup
		 * @return none
		 */
		void Reset();
		
		/**
		 * Returns the active status of the backup-restore
		 * @return ETrue if backup-restore is active, else EFalse
		 */
		TBool BackupRestoreActive() const;
		
	private:
		/**
		 * Constructor
		 * @param aQueue The address of CSConBackupRestoreQueue
		 * @param aFs aReference to RFs connection.
		 * @return none
		 */
		CSConBackupRestore( CSConBackupRestoreQueue* aQueue, RFs& aFs );
		/**
		 * Initializes member data
		 * @return none
		 */
		void ConstructL();
		/**
		 * Implementation of CActive::DoCancel()
		 * @return none
		 */
		void DoCancel();
		/**
		 * Implementation of CActive::RunL()
		 * @return none
		 */
		void RunL();
		
	private:
		CSConBackupRestoreQueue*		iQueue; 
		CSConSBEClient*					iSBEClient;
		TInt							iCurrentTask;
		TBool							iBackupRestoreActive;
		RFs&                            iFs;
	};
	
#endif // _SCONBACKUPRESTORE_H_

// End of file
