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

#include "mphonemenuandcbaevents.h"

extern int m_command;
extern bool m_leave;

// FORWARD DECLARATIONS

class CPhoneUIController_Stub : 
    public MPhoneMenuAndCbaEvents
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

// From MPhoneMenuAndCbaEvents

        /**
        * HandleCommandL
        */
        TBool HandleCommandL( TInt aCommand );

        /**
        * Process command.
        */
        TBool ProcessCommandL( TInt aCommand );
        
    };

#endif      // CPHONEUICONTROLLER_STUB_H   
            
// End of File
