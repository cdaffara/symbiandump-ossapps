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
* Description:  CPhoneIdle_Inherited declaration. 
*
*/

#ifndef CPhoneIdle_Inherited_H
#define CPhoneIdle_Inherited_H

// INCLUDES
#include <e32std.h>
#include "cphoneidle.h"

// CLASS DECLARATION

/**
 *  CPhoneIdle_Inherited
 * 
 */
class CPhoneIdle_Inherited : public CPhoneIdle
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPhoneIdle_Inherited();

    /**
     * Two-phased constructor.
     */
    static CPhoneIdle_Inherited* NewL(
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

    /**
     * Two-phased constructor.
     */
    static CPhoneIdle_Inherited* NewLC(
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );
    
    void User_HandleErrorL( const TPEErrorInfo& aErrorInfo );
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CPhoneIdle_Inherited(
            MPhoneStateMachine* aStateMachine, 
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneCustomization* aPhoneCustomization );

    };

#endif // CPhoneIdle_Inherited_H
