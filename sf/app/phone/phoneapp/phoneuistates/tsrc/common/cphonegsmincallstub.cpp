/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


// INCLUDE FILES
#include "cphonegsmincall.h"
#include "phoneappcommands.hrh"

CPhoneGsmInCall::CPhoneGsmInCall(
        MPhoneStateMachine* aStateMachine, 
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneCustomization* aPhoneCustomization ): 
        CPhoneStateInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

CPhoneGsmInCall::~CPhoneGsmInCall()
    {
    
    }

void CPhoneGsmInCall::ConstructL()
    {
    
    }

TInt CPhoneGsmInCall::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EPhoneCmdOptions:
        break;
        default:
        break;
        }
    
    }

void CPhoneGsmInCall::HandlePhoneEngineMessageL(
        const TInt /*aMessage*/, 
        TInt /*aCallId*/ )
    {
    
    }

void CPhoneGsmInCall::HandleColpNoteL( TInt /*aCallId*/ )
    {
    
    }

void CPhoneGsmInCall::BringIncomingToForegroundL()
    {
    
    }


//  End of File  

