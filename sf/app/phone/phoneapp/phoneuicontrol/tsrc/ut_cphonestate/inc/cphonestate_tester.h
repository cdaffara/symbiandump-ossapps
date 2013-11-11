/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef cphonestate_tester_h
#define cphonestate_tester_h

#include <CPhoneState.h>
#include "TPhoneCmdParamCallHeaderData.h"

NONSHARABLE_CLASS( cphonestate_tester ) : public CPhoneState
    {
public:
        
    cphonestate_tester(  
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aCustomization );

    ~cphonestate_tester();
    
    void T_DisplayCallTerminationNoteL();
    
    TBool T_iDialRequest();
    
    void T_HandleVideoCallSetupFailedCreateVoiceCallToTheSameContactL();
    
    void T_DialVoiceCallL();
    
    void T_HandlePhoneEngineMessageL(
                const TInt aMessage, 
                TInt aCallId );  
    
    TBool CheckIfShowCallTerminationNote()
        {
        return ETrue;
        }
    
    void T_ShowNumberBusyNoteL();
        
    void T_UpdateRemoteInfoDataL( TInt aCallId );
    
    void T_StartShowSecurityNoteL();
    
    void T_UpdateSingleActiveCallL( TInt aCallId );
    
    void T_HandleDisconnectingL( TInt aCallId );
    
private:

    // Data
     
    };

cphonestate_tester::cphonestate_tester(  
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aCustomization ) : CPhoneState(aStateMachine, aViewCommandHandle, aCustomization)
    {
    }
            
cphonestate_tester::~cphonestate_tester() 
    {
    }

void cphonestate_tester::T_DisplayCallTerminationNoteL()
    {
    DisplayCallTerminationNoteL();
    }

void cphonestate_tester::T_DialVoiceCallL()
    {
    DialVoiceCallL();
    }

void cphonestate_tester::T_HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId )
    {
    HandlePhoneEngineMessageL( aMessage, aCallId );
    }

void cphonestate_tester::T_ShowNumberBusyNoteL()
    {
    ShowNumberBusyNoteL();
    }

void cphonestate_tester::T_UpdateRemoteInfoDataL( TInt aCallId )
    {
    UpdateCallHeader( aCallId );
    }

void cphonestate_tester::T_StartShowSecurityNoteL()
    {
    StartShowSecurityNoteL();
    }

void cphonestate_tester::T_UpdateSingleActiveCallL( TInt aCallId )
    {
    UpdateSingleActiveCallL( aCallId );
    }

void cphonestate_tester::T_HandleDisconnectingL( TInt aCallId )
    {
    HandleDisconnectingL( aCallId );
    }


#endif // cphonestate_tester_h

//End of file
