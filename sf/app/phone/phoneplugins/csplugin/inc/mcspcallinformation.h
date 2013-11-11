/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for MCSPCallInformation class
*
*/



#ifndef MCSPCALLINFORMATION_H
#define MCSPCALLINFORMATION_H

//INCLUDES
#include <etelmm.h>

// FORWARD DECLARATIONS
class CSPCall;

// CLASS DECLARATION

/**
*  Interface for browsing calls.
*
*/
class MCSPCallInformation
    {
    public:

        /**
        * Finds call based on the name.
        * @param aCallName call name (etel)
        * @return call object or NULL if not found.
        */
        virtual CSPCall* FindCall( const TName& aCallName ) = 0;

    };

#endif // MINCOMINGCALLOBSERVER_H

//End of file
