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
#include "cphonestatestartup.h"
#include "phoneappcommands.hrh"

CPhoneStateStartup::CPhoneStateStartup(
        MPhoneStateMachine* aStateMachine, 
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneCustomization* aPhoneCustomization ): 
        CPhoneState( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

CPhoneStateStartup::~CPhoneStateStartup()
    {
    
    }

void CPhoneStateStartup::ConstructL()
    {
    
    }

void CPhoneStateStartup::HandlePhoneEngineMessageL(
    const TInt /*aMessage*/, 
    TInt /*aCallId*/ )
    {
    
    }

void CPhoneStateStartup::HandleKeyMessageL(
    TPhoneKeyEventMessages /*aMessage*/,
    TStdScanCode /*aScanCode*/ )
    {
    
    }

void CPhoneStateStartup::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aEventCode*/ )
    {
   
    }

void CPhoneStateStartup::HandlePhoneStartupL()
    {
    
    }

//  End of File  

