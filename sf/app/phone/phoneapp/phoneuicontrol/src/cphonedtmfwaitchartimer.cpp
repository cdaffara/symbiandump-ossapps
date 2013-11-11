/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CPhoneDtmfWaitCharTimer implementation.
*
*/


// INCLUDE FILES
#include "cphonedtmfwaitchartimer.h"
#include "phoneconstants.h"
#include "phonelogger.h"
#include "cphonestatehandle.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPhoneDtmfWaitCharTimer::CPhoneDtmfWaitCharTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneDtmfWaitCharTimer::CPhoneDtmfWaitCharTimer( MPhoneStateMachine* aStateMachine ) :
    iTimer( NULL ),
    iStateMachine( aStateMachine )
    {
        
    }

// Destructor
CPhoneDtmfWaitCharTimer::~CPhoneDtmfWaitCharTimer()
    {
    if( iTimer )
        {
        if( iTimer->IsActive() )
            {
            iTimer->CancelTimer();              
            }
        }
        delete iTimer;
    }

// -----------------------------------------------------------
// CPhoneDtmfWaitCharTimer::NewL()
// Two-phased constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneDtmfWaitCharTimer* CPhoneDtmfWaitCharTimer::NewL( 
    MPhoneStateMachine* aStateMachine )
    {
    CPhoneDtmfWaitCharTimer* self = 
        new( ELeave ) CPhoneDtmfWaitCharTimer( aStateMachine );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CPhoneDtmfWaitCharTimer::ConstructL()
// EPOC default constructor can leave.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPhoneDtmfWaitCharTimer::ConstructL()
    {
    }

// ---------------------------------------------------------
// CPhoneNoteTimer::HandleTimeOutL
// ---------------------------------------------------------
//
void CPhoneDtmfWaitCharTimer::HandleTimeOutL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, 
        "CPhoneDtmfWaitCharTimer::HandleTimeOutL( ) ");
    
    iStateMachine->SendPhoneEngineMessage( 
        MPEPhoneModel::EPEMessageContinueDTMFSending );
    }

// ---------------------------------------------------------
// CPhoneDtmfWaitCharTimer::ButtonPressedL
// ---------------------------------------------------------
//
void CPhoneDtmfWaitCharTimer::ButtonPressedL()
    {
    if( !iTimer )
        {
        iTimer = CPhoneTimer::NewL();           
        }
    else
        {
        if( iTimer->IsActive() )
            {
            iTimer->CancelTimer();              
            }
        }   
    
    iTimer->After( KDtmfWaitNoteTimeoutValue, this );   
    }

// End of File
