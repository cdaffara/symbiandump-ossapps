/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Queue header file
*
*/


#ifndef _SCONQUEUE_H_
#define _SCONQUEUE_H_

// INCLUDES

#include <e32base.h>
#include <e32cons.h>

#include "sconconmltask.h"

class CSConAppInstaller;
class CSConBackupRestore;
class CSConInstallerQueue;
class CSConBackupRestoreQueue;

//============================================================
// Class CSConTaskQueue declaration
//============================================================	
NONSHARABLE_CLASS ( CSConTaskQueue )
	{
	public:
		/**
		 * Destructor
		 * @return none
		 */
		virtual ~CSConTaskQueue();
		/**
		 * Returns the status of a specified task / all tasks
		 * @param aTask The task number
		 * @param aAllTasks If ETrue => Receive status of all tasks
		 * @param aStatus The reply
    	 * @return none
		 */
		virtual void GetQueueStatusL( TInt aTask, TBool aAllTasks, 
									CSConStatusReply*& aStatus );
		/**
		 * Adds a new task to queue
		 * @param aNewTask A new task to be added
		 * @param aTaskId The task number
    	 * @return KErrNone if no errors, else system wide error codes
		 */
		virtual TInt AddNewTask( CSConTask*& aNewTask, TInt aTaskId );
		/**
		 * Set the task to completed -mode
		 * @param aTask The task number
		 * @param aError The task error code
    	 * @return none
		 */
		virtual TInt CompleteTask( TInt aTask, TInt aError );
		/**
		 * Set the task progress value
		 * @param aTask The task number
		 * @param aProgressValue The progress value
    	 * @return none
		 */
		virtual TInt SetTaskProgress( TInt aTask, TInt aProgressValue );
		/**
		 * Receives a specified task
		 * @param aTaskId The task number
		 * @param aTask The address to the task
    	 * @return KErrNone if no errors, else system wide error codes
		 */
		virtual TInt GetTask( TInt aTaskId, CSConTask*& aTask );
		/**
		 * Removes a task from the queue
		 * @param aTask The task number
    	 * @return KErrNone if no errors, else system wide error codes
		 */
		virtual TInt RemoveTask( TInt aTask );	
		/**
		 * Cancels a task
		 * @param aTask The task number
		 * @param aAllTasks If ETrue => cancel all tasks
    	 * @return none
		 */	
		virtual void CancelTask( TInt aTask, TBool aAllTasks );
		/**
		 * The status of the process
    	 * @return ETrue if process active, else EFalse
		 */
		virtual TBool QueueProcessActive() const;
		/**
		 * Changes the status of the queue process
    	 * @return none
		 */
		virtual void ChangeQueueProcessStatus();
		/**
		 * Resets the queue
    	 * @return none
		 */
		virtual void Reset();		
		/**
		 * Compares task numbers
		 * @param aFirst The first task
		 * @param aSecon The second task
    	 * @return -1 if the first task number is smaller than the second, 
    	 * 1 the first task number is greater than the second, 0 if task 
    	 * numbers are equal
		 */
		static TInt Compare( const CSConTask& aFirst, 
							 const CSConTask& aSecond );
		/**
		 * Matches the task numbers
		 * @param aFirst The first task
		 * @param aSecon The second task
    	 * @return ETrue if task numbers match, else EFalse
		 */
		static TBool Match( const CSConTask& aFirst, 
							const CSConTask& aSecond );
		
	private:
		/**
		 * Pure virtual function for starting the queue observation
		 * @return none
		 */
		virtual void StartQueue() = 0;
		/**
		 * Pure virtual function for stoping the queue observation
		 * @return none
		 */
		virtual void StopQueue() = 0;
		/**
		 * Pure virtual function for polling the queue
		 * @return none
		 */
		virtual void PollQueue() = 0;
		
	protected:
		RPointerArray<CSConTask>	iQueue;
		RTimer 						iTimer;
		TBool						iQueueProcessActive;
	};
	
#endif

// End of file
