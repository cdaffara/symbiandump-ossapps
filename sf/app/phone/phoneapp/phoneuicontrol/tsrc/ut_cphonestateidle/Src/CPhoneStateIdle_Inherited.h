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
* Description:  CCPhoneStateIdle_Inherited declaration. 
*
*/

#ifndef CPHONESTATEIDLE_INHERITED_H
#define CPHONESTATEIDLE_INHERITED_H

// INCLUDES
#include <e32std.h>
#include "CPhoneStateIdle.h"

// CLASS DECLARATION

/**
 *  CPhoneStateIdle_Inherited
 * 
 */
class CPhoneStateIdle_Inherited : public CPhoneStateIdle
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPhoneStateIdle_Inherited();

    /**
     * Two-phased constructor.
     */
    static CPhoneStateIdle_Inherited* NewL(
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

    /**
     * Two-phased constructor.
     */
    static CPhoneStateIdle_Inherited* NewLC(
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

    void DialL_UserL( const TDesC& aNumber, 
        TPhoneNumberType aNumberType, TDialInitiationMethod aDialMethod );
    
    void HandleNumberEntryClearedL_UserL();
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CPhoneStateIdle_Inherited(
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

    };

#endif // CPHONESTATEIDLE_INHERITED_H
