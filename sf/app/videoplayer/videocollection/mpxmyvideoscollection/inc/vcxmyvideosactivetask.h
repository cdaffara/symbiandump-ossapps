/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Active object to split up long running tasks*
*/



#ifndef VCXMYVIDEOSACTIVETASK_H
#define VCXMYVIDEOSACTIVETASK_H

#include <mpxcommand.h>
#include <mpxmessage2.h>

/**
* Observer class to the active task object
*/
NONSHARABLE_CLASS( MVcxMyVideosActiveTaskObserver )
    {
public:
    
    enum TStepResult
        {
        EMoreToCome,
        EDone,
        /**
         * Observer is responsible for calling Done or ContinueStepping if returning this.
         */
        EStopStepping
        };
    
    /**
    * Handle the execution of a step
    * @return TStepResult
    */
    virtual TStepResult HandleStepL() = 0;
    /**
    * Handle the completion of the operation
    */
    virtual void HandleOperationCompleted( TInt aErr ) = 0;
    };

/**
 *  Active object to execute tasks in multiple steps.
 *  Each collection plugin has its own task queue, which serializes all
 *  operations for a plugin.
 */
NONSHARABLE_CLASS( CVcxMyVideosActiveTask ) : public CActive
    {
public:
    /**
    * Two-phased constructor
    */
    static CVcxMyVideosActiveTask* NewL( MVcxMyVideosActiveTaskObserver& aObserver );

    /**
    * Destructor
    */
    virtual ~CVcxMyVideosActiveTask();

public:    // Accessor functions

    /**
    * Start the async command task
    * @param aTask task id
    * @param aCommand command for the operation
    */
    void StartL( TMPXCommandId aTask, const CMPXCommand& aCommand );

    /**
    * Get the current step for the current task
    * @return the current step
    */
    TInt GetStep();

    /*
    * Get the command for the current task
    * @return the current task
    */
    TMPXCommandId GetTask();

    /**
    * Get the command for the current task
    * @return the current command
    */
    CMPXCommand& GetCommand();

    /**
    * Get the command for the current task.
    * @return the current command, ownership does not move.
    */        
    CMPXMedia* Command();
    
    /**
     * If observer returns EStopStepping from HandleStepL, then observer must call
     * this or ContinueStepping when sub operation is finished.
     * Can be used to run async operations.
     */
    void Done();

    /**
     * If observer returns EStopStepping from HandleStepL, then observer must call
     * this or Done when sub operation is finished.
     * Can be used to run async operations.
     */
    void ContinueStepping();

    /**
     * Add some functionality to CActive::Cancel
     */
    void Cancel();

protected: // From base class

    /**
    * From CActive
    */
    void RunL();
    
    /**
    * From CActive
    */
    void DoCancel();

    /**
    * From CActive
    */
    TInt RunError( TInt aError );

private:

    /**
    * Default Constructor
    */
    CVcxMyVideosActiveTask(MVcxMyVideosActiveTaskObserver& aObserver);

    /**
    * 2nd-phase Constructor
    */
    void ConstructL();

public:
    /**
     * Set to ETrue when stepping is stopped.
     * Cancel or Done has to be called.
     */
    TBool iStopped;

private: // data

    CMPXCommand*  iCurCommand;  // Current Command
    TMPXCommandId iCurTask;  // The task command id
    TInt          iCurStep;  // The n-th stop in the operation
    MVcxMyVideosActiveTaskObserver& iObserver; // Observer
    };

#endif // VCXMYVIDEOSACTIVETASK_H
