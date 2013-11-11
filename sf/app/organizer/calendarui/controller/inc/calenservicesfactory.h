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
* Description:  Interface for creating calendar services
*
*/

#ifndef CALENSERVICESFACTORY_H
#define CALENSERVICESFACTORY_H

// Forward declarations
class MCalenServices;

/**
 * Calendar Services Factory
 */
NONSHARABLE_CLASS( MCalenServicesFactory )
    {
    public:
        /**
         * Create a new instance of MCalenServices and pass
         * ownership to the caller
         */
        virtual MCalenServices* NewServicesL() = 0;
    };

#endif // CALENSERVICESFACTORY_H

// End of file
