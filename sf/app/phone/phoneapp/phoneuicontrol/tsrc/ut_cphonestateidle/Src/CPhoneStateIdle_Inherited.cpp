/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CCPhoneStateIdle_Inherited implementation. 
*
*/

#include "CPhoneStateIdle_Inherited.h"


CPhoneStateIdle_Inherited::CPhoneStateIdle_Inherited(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    : CPhoneStateIdle( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    // No implementation required
    }

CPhoneStateIdle_Inherited::~CPhoneStateIdle_Inherited()
    {
    }

CPhoneStateIdle_Inherited* CPhoneStateIdle_Inherited::NewLC(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneStateIdle_Inherited* self = new ( ELeave )
        CPhoneStateIdle_Inherited( aStateMachine, aViewCommandHandle, aPhoneCustomization );
    CleanupStack::PushL(self);
    self->ConstructL();     // base class
    return self;
    }

CPhoneStateIdle_Inherited* CPhoneStateIdle_Inherited::NewL(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneStateIdle_Inherited* self = CPhoneStateIdle_Inherited::NewLC(
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    CleanupStack::Pop(); // self;
    return self;
    }

void CPhoneStateIdle_Inherited::DialL_UserL( const TDesC& aNumber, 
        TPhoneNumberType aNumberType, TDialInitiationMethod aDialMethod )
    {
    DialL( aNumber, aNumberType, aDialMethod );
    }

void CPhoneStateIdle_Inherited::HandleNumberEntryClearedL_UserL()
    {
    HandleNumberEntryClearedL();
    }
