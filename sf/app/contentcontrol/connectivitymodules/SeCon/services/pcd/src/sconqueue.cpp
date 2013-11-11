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
* Description:  Queue implementation
*
*/


// INCLUDE FILES

#include "sconqueue.h"
#include "sconpcdconsts.h"
#include "debug.h"
#include <SWInstDefs.h> // installer errors
#include <usif/usiferror.h>

// ============================= MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSConTaskQueue::~CSConTaskQueue()
// Destructor
// -----------------------------------------------------------------------------
//
CSConTaskQueue::~CSConTaskQueue()
    {
    TRACE_FUNC;
    iQueue.ResetAndDestroy();
    iQueue.Close(); 
    iTimer.Close();
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::GetQueueStatus( TInt aTask, TBool aAllTasks, 
//                                  CSConStatusReply*& aStatus )
// Returns the status of a specified task / all tasks
// -----------------------------------------------------------------------------
//
void CSConTaskQueue::GetQueueStatusL( TInt aTask, TBool aAllTasks, 
                                    CSConStatusReply*& aStatus )
    {
    RArray<TInt> completedTasks;
    CleanupClosePushL( completedTasks );
    if ( aAllTasks )
        {
        //if there are tasks
        if ( iQueue.Count() > 0 )
            {
            //set iNoTasks as EFalse
            aStatus->iNoTasks = EFalse;
            for ( TInt i = 0; i < iQueue.Count(); i++ )
                {
                //Fill reply object
                CSConTaskReply* taskReply = new (ELeave) CSConTaskReply();
                CleanupStack::PushL( taskReply );
                taskReply->InitializeL( *iQueue[i] );
                User::LeaveIfError( aStatus->iTasks.Append( taskReply ) );
                CleanupStack::Pop( taskReply );
                TBool complete = iQueue[i]->GetComplete();

                //Collect completed task numbers to array for deleting
                if ( complete )
                    {
                    completedTasks.Append( iQueue[i]->iTaskId );
                    }
                //Otherwise clean all unneccessary data from the reply packet
                else
                    {
                    taskReply->CleanTaskData(); 
                    }
                }
            }
        else
            {
            //no task in the queue
            aStatus->iNoTasks = ETrue;
            }

        //Remove completed tasks from queue
        for ( TInt j = 0; j < completedTasks.Count(); j++ )
            {
            RemoveTask( completedTasks[j] );
            }
        }
    else if ( aTask > 0 )
        {
        CSConTask* temp = new (ELeave) CSConTask();
        temp->iTaskId = aTask;
        TInt index = iQueue.Find( temp, CSConTaskQueue::Match );
        delete temp;
        
        TBool complete = EFalse;
        CSConTaskReply* taskReply(NULL);

        if ( index != KErrNotFound )
            {
            aStatus->iNoTasks = EFalse;
            //Fill reply object
            taskReply = new (ELeave) CSConTaskReply();
            CleanupStack::PushL( taskReply );
            taskReply->InitializeL( *iQueue[index] );
            User::LeaveIfError( aStatus->iTasks.Append( taskReply ) );
            CleanupStack::Pop( taskReply );
            complete = iQueue[index]->GetComplete();
            }
        else
            {
            //no task in the queue
            aStatus->iNoTasks = ETrue;
            }        
        
        //Delete completed tasks from queue
        if ( complete )
            {
            RemoveTask( aTask );
            }
        //Otherwise clean all unneccessary data from the reply packet
        else if ( taskReply )
            {
            taskReply->CleanTaskData(); 
            }
        }
    else
        {
        //no task in the queue
        aStatus->iNoTasks = ETrue;
        }
    CleanupStack::PopAndDestroy( &completedTasks ); // close
    }   
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::AddNewTask( CSConTask*& aNewTask, TInt aTaskId )
// Adds a new task to queue
// -----------------------------------------------------------------------------
//
TInt CSConTaskQueue::AddNewTask( CSConTask*& aNewTask, TInt aTaskId )
    {
    TInt ret( KErrNone );
    
    aNewTask->iTaskId = aTaskId;
    
    //Set progress value "task accepted for execution"
    aNewTask->SetProgressValue( KSConCodeTaskCreated );
    aNewTask->SetCompleteValue( EFalse );
    
    if ( iQueue.Count() == 0 )
        {
        StartQueue();
        }
    ret = iQueue.InsertInOrder( aNewTask, CSConTaskQueue::Compare );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::CompleteTask( TInt aTask, TInt aError )
// Set the task to completed -mode
// -----------------------------------------------------------------------------
//
TInt CSConTaskQueue::CompleteTask( TInt aTask, TInt aError )
    {
    LOGGER_WRITE_1( "CSConTaskQueue::CompleteTask aError: %d", aError );
    TInt ret(KErrNone);
    TInt index( KErrNotFound );
    
    CSConTask* temp = new CSConTask();
    if (!temp)
        return KErrNoMemory;
    
    temp->iTaskId = aTask;
    index = iQueue.Find( temp, CSConTaskQueue::Match );
    delete temp;
    
    if ( index != KErrNotFound )
        {
        TBool complete( ETrue );
        TBool notComplete( EFalse );
        TInt progress( KSConCodeTaskCompleted );        
        
        switch( aError )
            {
            case KErrNone :
                iQueue[index]->SetCompleteValue( complete );
                progress =  KSConCodeTaskCompleted;
                break;
            case KErrNotFound :
                iQueue[index]->SetCompleteValue( complete );
                progress =  KSConCodeNotFound;
                break;
            case KErrCompletion :
                iQueue[index]->SetCompleteValue( notComplete );
                progress = KSConCodeTaskPartiallyCompleted;
                break;

            // installer specific errors
            case SwiUI::KSWInstErrUserCancel:
            	LOGGER_WRITE("User cancelled the operation");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrUserCancel;
            	break;
            case SwiUI::KSWInstErrFileCorrupted:
            case KErrSifCorruptedPackage:
            	LOGGER_WRITE("File is corrupted");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrFileCorrupted;
            	break;
            case SwiUI::KSWInstErrInsufficientMemory:
            case KErrSifNotEnoughSpace:
            	LOGGER_WRITE("Insufficient free memory in the drive to perform the operation");
	            iQueue[index]->SetCompleteValue( complete );
	            progress = KSConCodeInstErrInsufficientMemory;	
	            break;
            case SwiUI::KSWInstErrPackageNotSupported:
            case KErrSifUnsupportedSoftwareType:
            	LOGGER_WRITE("Installation of the package is not supported");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrPackageNotSupported;
            	break;
            case SwiUI::KSWInstErrSecurityFailure:
            	LOGGER_WRITE("Package cannot be installed due to security error");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrSecurityFailure;
            	break;
            case SwiUI::KSWInstErrMissingDependency:
            case KErrSifMissingDependencies:
            	LOGGER_WRITE("Package cannot be installed due to missing dependency");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrMissingDependency;
            	break;
            case SwiUI::KSWInstErrFileInUse:
            	LOGGER_WRITE("Mandatory file is in use and prevents the operation");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrFileInUse;
            	break;
            case SwiUI::KSWInstErrGeneralError:
            case KErrSifUnknown:
            	LOGGER_WRITE("Unknown error");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrGeneralError;
            	break;
            case SwiUI::KSWInstErrNoRights:
            	LOGGER_WRITE("The package has no rights to perform the operation");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrNoRights;
            	break;
            case SwiUI::KSWInstErrNetworkFailure:
            	LOGGER_WRITE("Indicates that network failure aborted the operation");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrNetworkFailure;
            	break;
            case SwiUI::KSWInstErrBusy:
            	LOGGER_WRITE("Installer is busy doing some other operation");
            	iQueue[index]->SetCompleteValue( complete );
        		progress = KSConCodeInstErrBusy;
            	break;
            case SwiUI::KSWInstErrAccessDenied:
            	LOGGER_WRITE("Target location of package is not accessible");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstErrAccessDenied;
            	break;
            case SwiUI::KSWInstUpgradeError:
            	LOGGER_WRITE("The package is an invalid upgrade");
            	iQueue[index]->SetCompleteValue( complete );
            	progress = KSConCodeInstUpgradeError;
            	break;
            	
            case KErrSifMissingBasePackage:
                LOGGER_WRITE("KErrSifMissingBasePackage");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifMissingBasePackage;
                break;
            case KErrSifOverflow:
                LOGGER_WRITE("KErrSifOverflow");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifOverflow;
                break;
            case KErrSifSameVersionAlreadyInstalled:
                LOGGER_WRITE("KErrSifSameVersionAlreadyInstalled");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifSameVersionAlreadyInstalled;
                break;
            case KErrSifNewerVersionAlreadyInstalled:
                LOGGER_WRITE("KErrSifNewerVersionAlreadyInstalled");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifNewerVersionAlreadyInstalled;
                break;
            case KErrSifAlreadyActivated:
                LOGGER_WRITE("KErrSifAlreadyActivated");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifAlreadyActivated;
                break;
            case KErrSifAlreadyDeactivated:
                LOGGER_WRITE("KErrSifAlreadyDeactivated");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifAlreadyDeactivated;
                break;
            case KErrSifBadComponentId:
                LOGGER_WRITE("KErrSifBadComponentId");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifBadComponentId;
                break;
            case KErrSifBadInstallerConfiguration:
                LOGGER_WRITE("KErrSifBadInstallerConfiguration");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifBadInstallerConfiguration;
                break;
            case KErrSifPackageCannotBeInstalledOnThisDevice:
                LOGGER_WRITE("KErrSifPackageCannotBeInstalledOnThisDevice");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifPackageCannotBeInstalledOnThisDevice;
                break;
            case KErrSifUnsupportedLanguage:
                LOGGER_WRITE("KErrSifUnsupportedLanguage");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrSifUnsupportedLanguage;
                break;
            case KErrScrWriteOperationInProgress:
                LOGGER_WRITE("KErrScrWriteOperationInProgress");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrScrWriteOperationInProgress;
                break;
            case KErrScrReadOperationInProgress:
                LOGGER_WRITE("KErrScrReadOperationInProgress");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrScrReadOperationInProgress;
                break;
            case KErrScrNoActiveTransaction:
                LOGGER_WRITE("KErrScrNoActiveTransaction");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrScrNoActiveTransaction;
                break;
            case KErrScrUnsupportedLocale:
                LOGGER_WRITE("KErrScrUnsupportedLocale");
                iQueue[index]->SetCompleteValue( complete );
                progress = KSConCodeInstErrScrUnsupportedLocale;
                break;
            
            default :
                iQueue[index]->SetCompleteValue( complete );
                if ( aError < KErrNone && aError >= KErrCorruptSurrogateFound )
                    {
                    // aError is always negative
                    //  -> returned errorcode is from KSConCodeFirstSymbianErr...n
                    progress = KSConCodeFirstSymbianErr - aError;
                    }
                else
                    {
                    progress = KSConCodeConflict;
                    }
                
                break;
            }
            
        iQueue[index]->SetProgressValue( progress );
        }
    else
        {
        ret = KErrNotFound;
        }
    StartQueue();
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::SetTaskProgress( TInt aTask, TInt aProgressValue )
// Set the task progress value
// -----------------------------------------------------------------------------
//
TInt CSConTaskQueue::SetTaskProgress( TInt aTask, TInt aProgressValue )
    {
    TInt ret( KErrNone );
    TInt index( KErrNotFound );

    CSConTask* temp = new CSConTask();
    if (temp)
        {
        temp->iTaskId = aTask;
        index = iQueue.Find( temp, CSConTaskQueue::Match );
        delete temp;
        
        if ( index != KErrNotFound )
            {
            iQueue[index]->SetProgressValue( aProgressValue );
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    else
        {
        ret = KErrNoMemory;
        }
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::GetTask( TInt aTaskId, CSConTask*& aTask )
// Receives a specified task
// -----------------------------------------------------------------------------
//
TInt CSConTaskQueue::GetTask( TInt aTaskId, CSConTask*& aTask )
    {
    TInt ret( KErrNone );
    TInt index;
    
    CSConTask* temp = new CSConTask();
    if (temp)
        {
        temp->iTaskId = aTaskId;
        index = iQueue.Find( temp, CSConTaskQueue::Match );
        delete temp;
    
        if ( index != KErrNotFound )
            {
            aTask = iQueue[index];
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    else
        {
        ret = KErrNoMemory;
        }
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::RemoveTask( TInt aTask )
// Removes a task from the queue
// -----------------------------------------------------------------------------
//
TInt CSConTaskQueue::RemoveTask( TInt aTask )
    {
    TInt ret(KErrNone);
    TInt index( KErrNotFound );
    
    CSConTask* temp = new CSConTask();
    if (temp)
        {
        temp->iTaskId = aTask;
        index = iQueue.Find( temp, CSConTaskQueue::Match );
        delete temp;
        
        if ( index != KErrNotFound ) 
            {
            delete iQueue[index];
            iQueue.Remove( index );
            iQueue.Compress();
            }
        else
            {
            ret = KErrNotFound;
            }
        
        if ( iQueue.Count() == 0 )
            {
            StopQueue();
            iQueue.Reset();
            }
        }
    else
        {
        ret = KErrNoMemory;
        }
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::CancelTask( TInt aTask, TBool aAllTasks )
// Cancels a task
// -----------------------------------------------------------------------------
//
void CSConTaskQueue::CancelTask( TInt aTask, TBool aAllTasks )
    {
    TRACE_FUNC_ENTRY;
    
    //Remove the task from the queue
    if ( aTask > 0 && !aAllTasks )
        {
        RemoveTask( aTask );
        }
        
    //Remove all tasks from the queue
    if ( aAllTasks )
        {
        iQueue.ResetAndDestroy();
        }
    
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::QueueProcessActive()
// The status of the process
// -----------------------------------------------------------------------------
//  
TBool CSConTaskQueue::QueueProcessActive() const
    {
    return iQueueProcessActive;
    }

// -----------------------------------------------------------------------------
// CSConTaskQueue::ChangeQueueProcessStatus()
// Changes the status of the queue process
// -----------------------------------------------------------------------------
//  
void CSConTaskQueue::ChangeQueueProcessStatus()
    {
    iQueueProcessActive = !iQueueProcessActive;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::Reset()
// Resets the queue
// -----------------------------------------------------------------------------
//
void CSConTaskQueue::Reset()
    {
    TRACE_FUNC_ENTRY;
    iTimer.Cancel();
    iQueue.ResetAndDestroy();
    TRACE_FUNC_EXIT;
    }

// ---------------------------------------------------------
// CSConTaskQueue::Compare( const CSConTask& aFirst, 
//                          const CSConTask& aSecond )
// Compares task numbers
// ---------------------------------------------------------    
TInt CSConTaskQueue::Compare( const CSConTask& aFirst, 
                            const CSConTask& aSecond )
    {
    if ( aFirst.iTaskId < aSecond.iTaskId )
        {
        return -1;
        }
    else if ( aFirst.iTaskId > aSecond.iTaskId )
        {
        return 1;
        }
    
    return 0;
    }
    
// -----------------------------------------------------------------------------
// CSConTaskQueue::Match( const CSConTask& aFirst, const CSConTask& aSecond )
// Matches the task numbers
// -----------------------------------------------------------------------------
//
TInt CSConTaskQueue::Match( const CSConTask& aFirst, const CSConTask& aSecond )
    {
    if ( aFirst.iTaskId == aSecond.iTaskId )
        {
        return ETrue;
        }
        
    return EFalse;
    }
// End of file
