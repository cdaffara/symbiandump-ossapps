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
*     Phone UI controller class.
*
*/


#ifndef CPHONEUICONTROLLER_STUB_H
#define CPHONEUICONTROLLER_STUB_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>

#include "MPhoneKeyEventHandler.h"

extern int m_scan_code;
extern int m_code;
extern int m_scan_code_down;
extern int m_code_down;
extern int m_repeats;
extern bool m_leave;
extern TEventCode m_eventCode;

// FORWARD DECLARATIONS

class CPhoneUIController_Stub : 
    public MPhoneKeyEventHandler
    {
    public:
        
        /**
        * C++ default constructor.
        */
        CPhoneUIController_Stub();
        
        /**
        * Destructor.
        */
        virtual ~CPhoneUIController_Stub();


        TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aEventCode );


        
    };

#endif      // CPHONEUICONTROLLER_STUB_H   
            
// End of File
