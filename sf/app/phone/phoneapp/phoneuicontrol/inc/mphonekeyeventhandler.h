/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Interface class for handling key events.
*
*/


#ifndef __MPHONEKEYEVENTHANDLER_H
#define __MPHONEKEYEVENTHANDLER_H

//  INCLUDES

#include    <w32std.h>
#include    <coedef.h>

// CLASS DECLARATION
/**
*  Interface class for executing UI commands
*/
class MPhoneKeyEventHandler
    {
    public:
        /**
        * Destructor
        */
        virtual ~MPhoneKeyEventHandler(){};

        /**
        * Handles key events
        */
        virtual TKeyResponse HandleKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aEventCode ) = 0;
    };

#endif      // __MPHONEKEYEVENTHANDLER_H   
            
// End of File
