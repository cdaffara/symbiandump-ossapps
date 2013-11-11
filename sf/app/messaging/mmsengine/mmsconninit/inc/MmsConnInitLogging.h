/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     This file defines the logging information
*
*/



#ifndef CMMSCONNINITLOGGING_H
#define CMMSCONNINITLOGGING_H

#ifdef _DEBUG
#define _MMSCONNINIT_LOGGING_
#endif // _DEBUG

#ifdef _MMSCONNINIT_LOGGING_
#include <flogger.h>
#endif


// INCLUDES
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

#ifdef _MMSCONNINIT_LOGGING_
class TMmsConnInitLogger
    {
    public:
        static void Log(TRefByValue<const TDesC> aFmt,...);
    };
#endif


#endif      // CMMSCONNINITLOGGING_H

// End of File
