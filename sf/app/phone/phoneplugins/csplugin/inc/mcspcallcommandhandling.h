/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declarations for class mcspcallcommandhandling
*
*/



#ifndef MCSPCALLCOMMANDHANDLING_H
#define MCSPCALLCOMMANDHANDLING_H

//  INCLUDES
#include <e32std.h>

class CSPCall;

// CLASS DECLARATION


/**
*  Observer interface for call command handling
*
*/
class MCCPCallCommandHandling
    {
    public:  // New functions

        /**
        * Perform answer command.
        * @return system wide error code
        */
        virtual TInt PerformAnswerRequest() = 0;
        
        /**
        * Perform dial command.
        * @return system wide error code
        */
        virtual TInt PerformDialRequest() = 0;

    };

#endif // MCSPCALLCOMMANDHANDLING_H 

// End of File
