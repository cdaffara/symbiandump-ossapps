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
*     Describes all possible System events.
*
*/


#ifndef __MPHONESYSTEMEVENTHANDLER_H
#define __MPHONESYSTEMEVENTHANDLER_H

//  INCLUDES
#include <aknappui.h>

// CLASS DECLARATION

/**
*  All possible system events
*/
class MPhoneSystemEventHandler
    {
    public:

        /**
        * Destructor
        */
        virtual ~MPhoneSystemEventHandler(){};

        /**
        * Handles startup of the phone application 
        */
        virtual void HandlePhoneStartupL() = 0;
    };

#endif      // __MPHONESYSTEMEVENTHANDLER_H   
            
// End of File
