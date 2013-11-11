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
#include "phoneconstants.h"

int m_scan_code;
int m_code;
int m_scan_code_down;
int m_code_down;
int m_repeats;
bool m_leave;
TEventCode m_eventCode;


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
// CPhoneUIController_Stub::HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CPhoneUIController_Stub::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
{
    m_scan_code = 0;
    m_code = 0;
    m_scan_code_down = 0;
    m_code_down = 0;
    m_eventCode = aEventCode;
    
    if (m_leave) {
        User::Leave(KErrArgument);
    }
        
    m_repeats = aKeyEvent.iRepeats;
    
    if(EEventKeyUp == aEventCode) {
        m_scan_code = aKeyEvent.iScanCode;
        m_code = aKeyEvent.iCode;
    } else if(EEventKey == aEventCode) {
        m_scan_code_down = aKeyEvent.iScanCode;
        m_code_down = aKeyEvent.iCode;       	
    } else if(EEventLongPress) {
        m_scan_code = aKeyEvent.iScanCode;
        m_code = aKeyEvent.iCode;
    }
    
    return EKeyWasNotConsumed;
}



//  End of File  
