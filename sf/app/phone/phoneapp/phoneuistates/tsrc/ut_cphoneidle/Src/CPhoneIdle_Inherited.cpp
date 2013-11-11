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
* Description:  CPhoneIdle_Inherited implementation. 
*
*/

#include "CPhoneIdle_Inherited.h"


CPhoneIdle_Inherited::CPhoneIdle_Inherited(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    : CPhoneIdle( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    // No implementation required
    }

CPhoneIdle_Inherited::~CPhoneIdle_Inherited()
    {
    }

CPhoneIdle_Inherited* CPhoneIdle_Inherited::NewLC(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneIdle_Inherited* self = new ( ELeave )
        CPhoneIdle_Inherited( aStateMachine, aViewCommandHandle, aPhoneCustomization );
    CleanupStack::PushL(self);
    self->ConstructL();     // base class
    return self;
    }

CPhoneIdle_Inherited* CPhoneIdle_Inherited::NewL(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneIdle_Inherited* self = CPhoneIdle_Inherited::NewLC(
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    CleanupStack::Pop(); // self;
    return self;
    }

void CPhoneIdle_Inherited::User_HandleErrorL( const TPEErrorInfo& aErrorInfo )
    {
    HandleErrorL(aErrorInfo);
    }
