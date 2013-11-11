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
* Description:  Backup-Restore Queue header file
*
*/


#ifndef _SCONBRQUEUE_H_
#define _SCONBRQUEUE_H_

#include "sconqueue.h"

class CSConTaskQueue;

//============================================================
// Class CSConBackupRestoreQueue declaration
//============================================================	
NONSHARABLE_CLASS ( CSConBackupRestoreQueue ) : public CActive, public CSConTaskQueue
	{
	public:
		/**
		 * Two-phase constructor
		 * @param aFs aReference to RFs connection.
		 * @return CSConBackupRestoreQueue instance
		 */
		static CSConBackupRestoreQueue* NewL( RFs& aFs );
		/**
		 * Destructor
		 * @return CSConBackupRestoreQueue instance
		 */
		~CSConBackupRestoreQueue();
		/**
		 * Adds a new task to queue
		 * @param aNewTask A new task to be added
		 * @param aTaskId The task number
    	 * @return KErrNone if no errors, else system wide error codes
		 */
		TInt AddNewTask( CSConTask*& aNewTask, TInt aTaskId );
		/**
		 * Cancels a task
		 * @param aTask The task number
		 * @param aAllTasks If ETrue => cancel all tasks
		 * @param aStatus The reply
    	 * @return none
		 */	
		void CancelTask( TInt aTask, TBool aAllTasks );
		/**
		 * Resets the queue
    	 * @return none
		 */
		void Reset();
		/**
		 * An address pointer to another queue
		 * @param aTaskQueue The address to another queue
    	 * @return none
		 */
		virtual void QueueAddress( CSConInstallerQueue*& aTaskQueue );
		/**
		 * Returns the task type
		 * @param aTaskId The task number
    	 * @return task type
		 */	
		TSConMethodName GetTaskMethodL( TInt aTaskId );
		
	private:
		/**
		 * Constructor
		 * @return none
		 */
		CSConBackupRestoreQueue();
		/**
		 * Initializes member data
		 * @param aFs aReference to RFs connection.
		 * @return none
		 */
		void ConstructL( RFs& aFs );
		/**
		 * Polls queue
		 * @return none
		 */
		void PollQueue();
		/**
		 * Starts queue polling
		 * @return none
		 */
		void StartQueue();
		/**
		 * Stops queue polling
		 * @return none
		 */
		void StopQueue();
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
		CSConBackupRestore*			iBackupRestore;
		CSConInstallerQueue*		iInstQueueAddress;
	};


#endif

// End of file
