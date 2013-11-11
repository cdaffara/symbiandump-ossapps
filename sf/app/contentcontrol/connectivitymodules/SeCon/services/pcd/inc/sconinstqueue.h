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
* Description:  Installer Queue header file
*
*/


#ifndef _SCONINSTQUEUE_H_
#define _SCONINSTQUEUE_H_

#include "sconqueue.h"


//============================================================
// Class CSConInstallerQueue declaration
//============================================================	
NONSHARABLE_CLASS ( CSConInstallerQueue ): public CActive, public CSConTaskQueue
	{
	public:
		/**
		 * Two-phase constructor
		 * @return CSConInstallerQueue instance
		 */
		static CSConInstallerQueue* NewL( RFs& aFs );
		/**
		 * Destructor
		 * @return none
		 */
		~CSConInstallerQueue();
		
		/**
		 * Cancels a task
		 * @param aTask The task number
		 * @param aAllTasks If ETrue => cancel all tasks
		 * @param aStatus The reply
    	 * @return none
		 */	
		void CancelTask( TInt aTask, TBool aAllTasks );
		
		/**
		 * An address pointer to another queue
		 * @param aTaskQueue The address to another queue
    	 * @return none
		 */			
		void QueueAddress( CSConBackupRestoreQueue*& aTaskQueue );
		
	private:
		/**
		 * Constructor
		 * @return none
		 */
		CSConInstallerQueue( RFs& aFs );
		/**
		 * Initializes member data
		 * @return none
		 */
		void ConstructL();
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
		CSConAppInstaller* 			iInstaller;	
		CSConBackupRestoreQueue*	iBRQueueAddress;
		RFs&                        iFs; 
	};


#endif

// End of file
