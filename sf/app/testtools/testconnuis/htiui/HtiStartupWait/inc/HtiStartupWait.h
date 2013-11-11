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
* Description:  CHtiStartupWait class declaration.
*
*/


#ifndef __HTISTARTUPWAIT_H
#define __HTISTARTUPWAIT_H

//  INCLUDES
#include <e32std.h>
#include <HtiStartupWaitInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*
*/
NONSHARABLE_CLASS(CHtiStartupWait) : public MHtiStartupWaitInterface
    {
    public: // from MHtiStartupWaitInterface
        virtual TInt WaitForStartup( TInt aMaxWaitTime );
    };

#endif // __HTISTARTUPWAIT_H

// End of File
