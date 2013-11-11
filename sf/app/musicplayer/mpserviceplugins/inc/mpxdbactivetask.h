/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to split up long running tasks
*
*/


#ifndef C_MPXDBACTIVETASK_H
#define C_MPXDBACTIVETASK_H

#include <mpxcommand.h>
#include <mpxmessage2.h>

/**
* Observer class to the active task object
*/
NONSHARABLE_CLASS( MMPXDbActiveTaskObserver )
    {
public:
    /**
    * Handle the execution of a step
    * @return ETrue if complete, EFalse if more to do
    */
    virtual TBool HandleStepL() = 0;
    /**
    * Handle the completion of the operation
    */
    virtual void HandleOperationCompleted( TInt aErr ) = 0;
    };

/**
 *  Active object to execute tasks in multiple steps
 *  Each collection plugin has its own task queue, which serializes all
 *  operations for a plugin
 *  @lib mpxdbplugin
 *  @since S60
 */
NONSHARABLE_CLASS( CMPXDbActiveTask ) : public CActive
    {
public:
    /**
    * Two-phased constructor
    */
    IMPORT_C static CMPXDbActiveTask* NewL( MMPXDbActiveTaskObserver& aObserver );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CMPXDbActiveTask();

public:    // Accessor functions

    /**
    * Start the async command task
    * @param aTask task id
    * @param aCommand command for the operation
    */
    IMPORT_C void StartL( TMPXCommandId aTask, const CMPXCommand& aCommand );

    /**
    * Get the current step for the current task
    * @return the current step
    */
    IMPORT_C TInt GetStep();

    /*
    * Get the command for the current task
    * @return the current task
    */
    IMPORT_C TMPXCommandId GetTask();

    /**
    * Get the command for the current task
    * @return the current command
    */
    IMPORT_C CMPXCommand& GetCommand();

    /**
    * Get the current change message array
    * @return get the change messages
    */
    IMPORT_C CMPXMessageArray& GetChangeMessages();

    enum TChangeVisibility
        {
        ENotVisibile,
        ESingleVisible,
        EAllVisible
        };
    /**
    * Set if the change is visible
    * @param aChange change state
    */
    IMPORT_C void SetVisibleChange( TChangeVisibility aChange );

    /**
    * Return if the change is visible
    * @return visible change state
    */
    IMPORT_C TChangeVisibility GetVisibleChange();

protected: // From base class

    /**
    * From CActive
    */
    IMPORT_C void RunL();

    /**
    * From CActive
    */
    IMPORT_C void DoCancel();

    /**
    * From CActive
    */
    IMPORT_C TInt RunError( TInt aError );

private:

    /**
    * Default Constructor
    */
    CMPXDbActiveTask(MMPXDbActiveTaskObserver& aObserver);

    /**
    * 2nd-phase Constructor
    */
    void ConstructL();

private: // data

    CMPXCommand*  iCurCommand;  // Current Command
    TMPXCommandId iCurTask;  // The task command id
    TInt          iCurStep;  // The n-th stop in the operation
    TChangeVisibility iCurVisibleChange; // Is the operation UI visible
    CMPXMessageArray* iChangeMessages; // Change message array
    MMPXDbActiveTaskObserver& iObserver; // Observer
    };

#endif // C_MPXDBACTIVETASK_H
