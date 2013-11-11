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
* Description:  Implementation of the CLcVtActiveExec class.
*
*/



// INCLUDE FILES
#include "clcvtactiveexec.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLcVtActiveExec::CLcVtActiveExec
// -----------------------------------------------------------------------------
//
CLcVtActiveExec::CLcVtActiveExec(
        const TInt aPriority )
    : CActive( aPriority )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLcVtActiveExec::~CLcVtActiveExec
// -----------------------------------------------------------------------------
//
CLcVtActiveExec::~CLcVtActiveExec()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CLcVtActiveExec::Start
// -----------------------------------------------------------------------------
//
void CLcVtActiveExec::Start( 
        TInt aStartState, 
        MLcVtActiveExec& aExec )
    {
    Cancel();

    iInitialState = aStartState;
    iCurrentState = aStartState;
    iNextState = aStartState;

    iActiveExec = &aExec;

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CLcVtActiveExec::InitialState
// -----------------------------------------------------------------------------
//
TInt CLcVtActiveExec::InitialState() const
    {
    return iInitialState;
    }

// -----------------------------------------------------------------------------
// CLcVtActiveExec::RequestStatus
// -----------------------------------------------------------------------------
//
TInt CLcVtActiveExec::RequestStatus() const
    {
    return iStatus.Int();
    }

// -----------------------------------------------------------------------------
// CLcVtActiveExec::RunL
// -----------------------------------------------------------------------------
//
void CLcVtActiveExec::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    TBool synch = ETrue;

    // Execute to the next asynchronous operation.
    while ( synch )
        {
        iCurrentState = iNextState;
        if ( iCurrentState )
            {
            synch = iActiveExec->ActiveExecExecuteL( 
                *this,
                iCurrentState, 
                iNextState, 
                iStatus );

            if ( !synch )
                {
                SetActive();
                }
            }
        else
            {   
            iActiveExec->ActiveExecDone( *this, iInitialState );
            if ( !IsActive() )
                {
                iInitialState = 0;
                }

            synch = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLcVtActiveExec::DoCancel
// -----------------------------------------------------------------------------
//
void CLcVtActiveExec::DoCancel()
    {
    if ( iActiveExec )
        {
        iActiveExec->ActiveExecCancel( *this, iCurrentState );
        
        iCurrentState = 0;
        iActiveExec = NULL;
        iInitialState = 0;
        }
    }

// -----------------------------------------------------------------------------
// CLcVtActiveExec::RunError
// -----------------------------------------------------------------------------
//
TInt CLcVtActiveExec::RunError( TInt aError )
    {
    if ( iActiveExec->ActiveExecContinue( *this, iCurrentState, aError ) )
        {
        iNextState = iCurrentState;

        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    else
        {
        if ( !IsActive() )
            {
            iInitialState = 0;
            }
        }
    
    return KErrNone;
    }

//  End of File  
