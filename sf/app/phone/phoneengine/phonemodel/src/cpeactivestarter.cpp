/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEActiveStarter class 
                 member functions.
*
*/


//  INCLUDE FILES
#include "cpeactivestarter.h"
#include <talogger.h>

// EXTERNAL DATA STRUCTURES
//None

// EXTERNAL FUNCTION PROTOTYPES  
//None;

// CONSTANTS
//None

// MACROS
//None

// LOCAL CONSTANTS AND MACROS
//None

// MODULE DATA STRUCTURES
//None

// LOCAL FUNCTION PROTOTYPES
//None

// FORWARD DECLARATIONS
//None

// ============================= LOCAL FUNCTIONS ===============================
//None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEActiveStarter::CPEActiveStarter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEActiveStarter::CPEActiveStarter(
    MPEActiveStarter* aActiveStarter ) 
    : CActive( CActive::EPriorityStandard ), 
      iActiveStarter ( aActiveStarter )
    {
        
    CActiveScheduler::Add( this );
    
    }

// -----------------------------------------------------------------------------
// CPEActiveStarter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEActiveStarter* CPEActiveStarter::NewL( 
    MPEActiveStarter* aActiveStarter )
    {
        
    return new( ELeave ) CPEActiveStarter( aActiveStarter );
    
    }

// Destructor
CPEActiveStarter::~CPEActiveStarter()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CPEActiveStarter::StartUp
// Starts the Phone Engine modules asynchronically.
// -----------------------------------------------------------------------------
//
void CPEActiveStarter::StartUp()
    {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone ); // complete request.
    }

// -----------------------------------------------------------------------------
// CPEActiveStarter::DoCancel
// Method cancels asynchronous request.
// -----------------------------------------------------------------------------
//
void CPEActiveStarter::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CPEActiveStarter::RunError
// Method recalls Phone Engine module starts.
// -----------------------------------------------------------------------------
//
TInt CPEActiveStarter::RunError( TInt aError )
    {
    TEFLOGSTRING2( KTAOBJECT, "CPEActiveStarter::RunError(): Leave %d in step.", aError );
    aError = aError; // for compiler warning
    iActiveStarter->RecallSteps();

    delete this; // ugly suicide thingie so don't implement any new code that reserver anything after this
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEActiveStarter::RunL
// Starts all Phone Engine modules asynchronically.
// -----------------------------------------------------------------------------
//
void CPEActiveStarter::RunL()
    { 
    // Protocol specific step.
    TEFLOGSTRING( KTAOBJECT, "CPEActiveStarter::RunL(): Starting step." );

    TBool continueStep = iActiveStarter->StepL( );

    TEFLOGSTRING( KTAOBJECT, "CPEActiveStarter::RunL(): Completed step." );
    if ( continueStep ) 
        {
        // Start the next module.
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    else
        {
        delete this;// ugly suicide thingie so don't implement any new code that reserver anything after this
        }
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================
//None    


//  End of File 
