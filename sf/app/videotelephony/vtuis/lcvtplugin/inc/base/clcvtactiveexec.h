/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Asynchronous execution class for LC VT plugin.
*
*/



#ifndef CLCVTACTIVEEXEC_H
#define CLCVTACTIVEEXEC_H

//  INCLUDES
#include    <e32base.h>

// FORWARD DECLARATIONS
class CLcVtActiveExec;

// CLASS DECLARATION

/**
*  Interface for asynchronous execution.
*
*  @since Series 60 2.6
*/
class MLcVtActiveExec
    {
    public: // New functions

        /**
        * Starts execution of a command.
        * @param aActiveExec active executor.
        * @param aState state based on which execution can be started.
        * @param aNextState next state.
        * @param aRequest request status, which should be used if operation is 
        *        asynchronous.
        * @return ETrue if operation was synchronous, otherwise EFalse.
        */
        virtual TBool ActiveExecExecuteL( 
            CLcVtActiveExec& aActiveExec,
            const TInt aState, 
            TInt& aNextState,
            TRequestStatus& aRequest ) = 0; 

        /**
        * Decide whether to continue or not.
        * @param aActiveExec active executor.
        * @param aState current state, should be updated to new state.
        * @param aError error code.
        * @return ETrue if continued, EFalse if stopped.
        */
        virtual TBool ActiveExecContinue(
            CLcVtActiveExec& aActiveExec,
            TInt& aState, 
            const TInt aError ) = 0;

        /**
        * Cancels operation of specific state.
        * @param aActiveExec active executor.
        * @param aState state.
        */
        virtual void ActiveExecCancel( 
            CLcVtActiveExec& aActiveExec,
            const TInt aState ) = 0;

        /**
        * Informs that operation has been finished.
        * @param aActiveExec active executor.
        * @param aInitialState initial state.
        */
        virtual void ActiveExecDone( 
            CLcVtActiveExec& aActiveExec,
            const TInt aInitialState ) = 0;

    };

/**
*  Asynchronous execution class for LC VT plugin.
*
*  @since Series 60 2.6
*/
class CLcVtActiveExec 
    : public CActive
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        * @param aPriority priority of the active object.
        */
        CLcVtActiveExec(
            const TInt aPriority );

        /**
        * Destructor.
        */
        ~CLcVtActiveExec();

    public: // New functions

        /**
        * Starts execution.
        * @param aStartState initial state.
        * @param aExec executor.
        */
        void Start( 
            TInt aStartState, 
            MLcVtActiveExec& aExec );

        /**
        * Initial state of currently
        * active operation.
        *
        * @return initial state, or KErrNotFound if no operation ongoing.
        */
        TInt InitialState() const;

        /**
        * Returns request status.
        * @return request status.
        */
        TInt RequestStatus() const;

    protected: // Functions from base classes

        /**
        * @see CActive::RunL.
        */
        virtual void RunL();

        /**
        * @see CActive::DoCancel.
        */
        virtual void DoCancel();

        /**
        * @see CActive::RunError.
        */
        virtual TInt RunError( TInt aError );

    private:

        // Ref to active executor.
        MLcVtActiveExec* iActiveExec;

        // Initial state.
        TInt iInitialState;

        // Current state.
        TInt iCurrentState;

        // Next state.
        TInt iNextState;

    };

#endif // CLCVTACTIVEEXEC_H

// End of File
