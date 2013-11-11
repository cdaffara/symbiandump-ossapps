/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the MPEContactHandling class.
*
*/


#ifndef MPECONTACTHANDLING_H
#define MPECONTACTHANDLING_H

//  INCLUDES
#include <e32base.h>
#include <pevirtualengine.h>

// CLASS DECLARATION

/**
*  Interface class provides for the access to contact information services.
*
*  @lib ContactHandling.lib
*  @since Series60_4.0
*/
class MPEContactHandling
    {
    public:
        /**
        * Destructor.
        */
        virtual ~MPEContactHandling(){};

        /**
        * Synchronous method for searching contact information.
        * @param aCallId        call id from phone engine
        *        aFindKey       defines the key to search the contact data with
        *        aSearchFrom    search from key
        * @return possible Symbian OS error code
        */
        virtual TInt FindContactInfoSync( 
            const TInt aCallId,
            const TPEFindContactKey aFindKey ) = 0;
    };

#endif      // MPECONTACTHANDLING_H
            
// End of File
