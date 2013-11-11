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
#include "cphonestateincall.h"
#include "phoneappcommands.hrh"

CPhoneStateInCall::CPhoneStateInCall(
        MPhoneStateMachine* aStateMachine, 
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneCustomization* aPhoneCustomization ): 
        CPhoneState( aStateMachine, aViewCommandHandle, aPhoneCustomization ),
        iDtmfWaitCharTimer( NULL )
    {
    }

CPhoneStateInCall::~CPhoneStateInCall()
    {
    
    }
        
void CPhoneStateInCall::ConstructL()
    {
    
    }
    
void CPhoneStateInCall::HandleKeyMessageL(
    TPhoneKeyEventMessages /*aMessage*/,
    TKeyCode /*aScanCode*/ )
    {
    
    }

void CPhoneStateInCall::HandlePhoneEngineMessageL(
    const TInt /*aMessage*/, 
    TInt /*aCallId*/ )
    {
    
    }

TBool CPhoneStateInCall::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EPhoneCmdOptions:
        break;
        default:
        break;
        }
    }
        
void CPhoneStateInCall::CallFetchedNumberL( const TDesC& /*aFetchedNumber*/ )
    {
    
    }

void CPhoneStateInCall::HandleNumberEntryClearedL()
    {
    
    }

TBool CPhoneStateInCall::IsVideoCallActiveL()
    {
    
    }

TBool CPhoneStateInCall::GetRingingCallL(  )
    {
    
    }

TBool CPhoneStateInCall::IsVideoCallRingingL()
    {
    
    }


void CPhoneStateInCall::DisconnectOutgoingCallL()
    {
    
    }

void CPhoneStateInCall::HandleIdleL( TInt aCallId )
    {
    
    }

//  End of File  

