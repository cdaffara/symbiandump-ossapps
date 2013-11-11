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
#include "cphonestateidle.h"
#include "phoneappcommands.hrh"

CPhoneStateIdle::CPhoneStateIdle(
        MPhoneStateMachine* aStateMachine, 
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneCustomization* aPhoneCustomization ): 
        CPhoneState( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

CPhoneStateIdle::~CPhoneStateIdle()
    {
    
    }

void CPhoneStateIdle::ConstructL()
    {
    
    }

void CPhoneStateIdle::HandlePhoneEngineMessageL(
    const TInt /*aMessage*/, 
    TInt /*aCallId*/ )
    {
    
    }

TBool CPhoneStateIdle::HandleCommandL( TInt /*aCommand*/ )
    {

    }

void CPhoneStateIdle::HandleNumberEntryClearedL()
    {
    
    }

void CPhoneStateIdle::HandleKeyMessageL(
    TPhoneKeyEventMessages /*aMessage*/,
    TKeyCode /*aScanCode*/ )
    {
    
    }

void CPhoneStateIdle::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aEventCode*/ )
    {
   
    }

TBool CPhoneStateIdle::HandleRemConCommandL(
    TRemConCoreApiOperationId /*aOperationId*/, 
    TRemConCoreApiButtonAction /*aButtonAct*/ )
    {
   
    }

void CPhoneStateIdle::HandlePhoneFocusLostEventL()
    {
    
    }

void CPhoneStateIdle::DialL( const TDesC& /*aNumber*/, 
    TPhoneNumberType /*aNumberType*/, TDialInitiationMethod /*aDialMethod*/ )
    {
    
    }

void CPhoneStateIdle::HandleDialingL( TInt /*aCallId*/ )
    {
    
    }

void CPhoneStateIdle::HandleSendCommandL()
    {
    
    }

//  End of File  

