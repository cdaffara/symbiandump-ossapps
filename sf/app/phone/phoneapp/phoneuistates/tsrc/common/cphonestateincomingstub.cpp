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
#include "cphonestateincoming.h"
#include "phoneappcommands.hrh"

CPhoneStateIncoming::CPhoneStateIncoming(
        MPhoneStateMachine* aStateMachine, 
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneCustomization* aPhoneCustomization ): 
        CPhoneState( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

CPhoneStateIncoming::~CPhoneStateIncoming()
    {
    
    }

void CPhoneStateIncoming::ConstructL()
    {
    
    }

void CPhoneStateIncoming::HandlePhoneEngineMessageL(
    const TInt /*aMessage*/, 
    TInt /*aCallId*/ )
    {
    
    }

TBool CPhoneStateIncoming::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EPhoneCmdOptions:
             OpenMenuBarL();
        break;
        default:
        break;
        }
    }

void CPhoneStateIncoming::HandleNumberEntryClearedL()
    {
    
    }

void CPhoneStateIncoming::HandleKeyMessageL(
    TPhoneKeyEventMessages /*aMessage*/,
    TKeyCode /*aScanCode*/ )
    {
    
    }

void CPhoneStateIncoming::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aEventCode*/ )
    {
   
    }

void CPhoneStateIncoming::DynInitMenuPaneL( 
    TInt /*aResourceId*/, 
    CEikMenuPane* /*aMenuPane*/ )
    {
    
    }

void CPhoneStateIncoming::HandleAudioPlayStoppedL()
    {
    
    }

void CPhoneStateIncoming::OpenMenuBarL()
    {
    
    }

//  End of File  

