/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CPhoneEngineHandler implementation.
*
*/


// INCLUDE FILES
#include "cphoneenginehandler.h"
#include "phonelogger.h"

// ================= MEMBER FUNCTIONS =======================

CPhoneEngineHandler::CPhoneEngineHandler( 
    MPhoneStateMachine* aStateMachine ) :
    iStateMachine( aStateMachine )
    {
    }

// -----------------------------------------------------------
// CPhoneEngineHandler::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneEngineHandler* CPhoneEngineHandler::NewL( 
    MPhoneStateMachine* aStateMachine )
    {
    CPhoneEngineHandler* self = 
        new (ELeave) CPhoneEngineHandler( aStateMachine );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneEngineHandler::DoHandleMessageL
// -----------------------------------------------------------
void CPhoneEngineHandler::DoHandleMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneEngineHandler::DoHandleMessageL( ) " );
    __PHONELOG2( EBasic, EPhoneControl,
        "CPhoneEngineHandler::DoHandleMessageL - Message received (aMessage=%d aCallId=%d)",
            aMessage, aCallId );
    iStateMachine->State()->HandlePhoneEngineMessageL( aMessage, aCallId );
    }

//  End of File  
