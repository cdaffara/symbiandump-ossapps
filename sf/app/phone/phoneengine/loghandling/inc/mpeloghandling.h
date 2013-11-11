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
* Description:  This file contains the header file of the MPELogHandling class.
*
*/


#ifndef MPELOGHANDLING_H
#define MPELOGHANDLING_H

//INCLUDES
#include <pevirtualengine.h>

// FORWARD DECLARATIONS
class MPEPhoneModelInternal;

/**
*  Virtual interface of log handling subsystem
*
*  @lib LogHandling.lib
*  @since Series60_4.0
*/
class MPELogHandling
    {
    public:
        /**
        * Destructor
        */
        virtual ~MPELogHandling( ) 
            {
            // empty destructor    
            };

        /**
        * Save call entry
        * @since N/A
        * @param: TInt, call id
        * @return: possible error code.
        */
        virtual TInt SaveCallEntry( const TInt aCallId ) = 0;
    };

#endif // MPELOGHANDLING_H

//End of file
