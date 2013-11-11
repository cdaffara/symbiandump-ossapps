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
* Description:  This module contains the implementation of CPETimer class 
*
*/


// INCLUDE FILES
#include "cpetimer.h"
#include "mpephonemodelinternal.h"

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
//None

// ==================== LOCAL FUNCTIONS ========================================
// None.

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CPETimer::CPETimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPETimer::CPETimer
        (
        MPEPhoneModelInternal& aModel   //MPEPhoneModelInternal is used to message sending.
        ) : CActive( EPriorityStandard ), 
            iModel( aModel )
    {
    CActiveScheduler::Add( this );
    }
// -----------------------------------------------------------------------------
// CPETimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPETimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// -----------------------------------------------------------------------------
// CPETimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPETimer* CPETimer::NewL(
    MPEPhoneModelInternal& aModel )
    {
    CPETimer* self = new ( ELeave ) CPETimer ( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPETimer::~CPETimer
        (
        // None.
        )
    {
    Cancel();
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CPETimer::ResetTimer
// Starts CPEtimer object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPETimer::ResetTimer
        (
        // None
        )
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CPETimer::StartTimer
// Starts CPEtimer object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPETimer::StartTimer
        (
        const TTimeIntervalMicroSeconds32 aSeconds, // includes timer delay
        const MEngineMonitor::TPEMessagesFromPhoneEngine aResponseMessage,          // the identification number of the response message
        const TInt aCallId                          // the identification number of the call
        )
    {
    TInt errorCode( ECCPErrorAlreadyInUse );
    if ( !IsActive() )
        {
        iTimer.Cancel();
        iResponseMessage = aResponseMessage;
        iCallId = aCallId;
        iTimer.After( iStatus,  aSeconds );
        SetActive( );
        errorCode = ECCPErrorNone;    
        }
    return errorCode;
    
    }

// -----------------------------------------------------------------------------
// CPETimer::DoCancel
// Cancels active object requests
// Method switches the asyncronous request by call status
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPETimer::DoCancel
        (
        // None
        )
    {
    iTimer.Cancel();
    iCallId = KPECallIdNotUsed;
    }

// -----------------------------------------------------------------------------
// CPETimer::GetCallId
// Gets the identification number of the call from intance variable.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPETimer::GetCallId
        (
        TInt& aCallId   // Includes identification number of the call.
        ) const
    {
    aCallId = iCallId;
    }

// -----------------------------------------------------------------------------
// CPETimer::RunL
// Method gets notification from etel that asyncronous request is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPETimer::RunL
        (
        // None.
        )
    {
    if (iStatus != KErrCancel)
        {   
        iModel.SendMessage( iResponseMessage, iCallId );
        }
    }
    
// ================= OTHER EXPORTED FUNCTIONS ===============================
//None
    
// End of File
