/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration for CMMCScBkupState
*     
*
*/

#ifndef __CMMCSCBKUPSTATE_H__
#define __CMMCSCBKUPSTATE_H__

// System includes
#include <e32base.h>

// User includes
#include "MMMCScBkupDriver.h"
#include "MMCScBkupStateIds.h"
#include "RMMCScBkupPointerArray.h"
#include "MMMCScBkupArchiveDataInterface.h"

// Namespaces
using namespace conn;

// Classes referenced
class CMMCScBkupState;

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupState) : public CActive
    {
    public:

        /**
        * Enumeration specifying how state is supposed to be handled
        */
        enum TStateExecution
            {
            EStateOnce = 0,
            EStatePerCategory,
            EStatePerCategoryCommon
            };

        /**
        * Destructor
        */
        ~CMMCScBkupState();

    protected:
    
        /**
        * C++ default constructor
        */
        CMMCScBkupState( MMMCScBkupDriver& aDriver, TInt aPriority = CActive::EPriorityIdle );

    public:

        /**
        * Return the unique RTTI id of this state
        */
        virtual TMMCScBkupStateId StateId() const = 0;

        /**
        * Return the next state that should be executed
        * after this state has finished. This allows
        * for dynamic changes in the state machine
        */
        virtual TMMCScBkupStateId NextStateId() const = 0;

        /**
        * Return false if no need for category specific actions, otherwise true
        */
        virtual TStateExecution CategorySpecific() const { return EStateOnce; }

        /**
        * Perform the action associated with this
        * state
        */
        void ExecuteL(TRequestStatus& aObserver);

    protected: // Virtual framework

        /**
        * Perform the initial action associated with this
        * state
        */
        virtual void PerformStateInitL() = 0;

        /**
        * Perform some asynchronous operation
        */
        virtual void PerformAsynchronousStateStepL();

        /**
        * Perform some asynchronous cancellation
        */
        virtual void PerformAsynchronousCancellation();

        /**
        * Called just before observer's request is completed
        * in the case of an error-free operation. Not called
        * when errors occur during state processing.
        */
        virtual void PerformLastRightsL();

        /**
        * Called from RunError
        */
        virtual TBool PerformAsynchronousErrorCleanup(TInt aError);

    private: // Virtual framework

        /**
        * Default behaviour is to panic if called
        */
        void RunL();

        /**
        * 
        */
        void DoCancel();

        /**
        * Default behaviour is to return KErrNone
        */
        TInt RunError(TInt aError);

    protected: // Internal API

        /**
        *
        */
        inline MMMCScBkupDriver& Driver() const { return iDriver; }

        /**
        *
        */
        inline MMMCScBkupArchiveDataInterface& ADI() const { return iDriver.DrvADI(); }

        /**
        *
        */
        void CompleteSelf(TInt aCompletionCode = KErrNone);

        /**
        *
        */
        void SetObserver(TRequestStatus& aStatus);

    private: // Internal API

        /**
        *
        */
        void CompleteObserver(TInt aCompletionCode);

    private: // Member data

        // Driver
        MMMCScBkupDriver& iDriver;
        // Observer
        TRequestStatus* iObserver;
    };





#endif // __CMMCSCBKUPSTATE_H__

//End of File
