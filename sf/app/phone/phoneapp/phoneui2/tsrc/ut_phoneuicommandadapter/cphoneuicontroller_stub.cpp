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
* Description: Implementation of CPhoneUIController class.
*
*/


// INCLUDE FILES

#include "CPhoneUIController_Stub.h"

int m_command;
bool m_leave;


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------
// CPhoneUIController::CPhoneUIController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------
//
CPhoneUIController_Stub::CPhoneUIController_Stub()  
    {
    m_leave = false;	
    }



// -----------------------------------------------------------
// Destructor
// 
// -----------------------------------------------------------

CPhoneUIController_Stub::~CPhoneUIController_Stub()
    {

    }



// ---------------------------------------------------------
// CPhoneUIController_Stub::HandleCommandL
// ---------------------------------------------------------
//
TBool CPhoneUIController_Stub::HandleCommandL( TInt aCommand )
    {
    m_command = 0;
    
    if (m_leave)
        {
        User::Leave(KErrArgument);
        }
        
    m_command = aCommand;
    
    return ETrue;
    }

// ---------------------------------------------------------
// CPhoneUIController_Stub::ProcessCommandL
// ---------------------------------------------------------
//
TBool CPhoneUIController_Stub::ProcessCommandL( TInt /*aCommand*/ )
    {  
    return ETrue;
    }

//  End of File  
